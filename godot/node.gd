#node.gd
#handles websocket data mainly
extends Node

# Ange websocket-URL:en (inkludera ws:// och porten)
@export var websocket_url: String = "ws://localhost:8080/ws"
var player_list_node_txt: ItemList  # ← den andra ItemList för ms-text
var player_list_node = null
var current_scene_ref

var active_mac_map: Dictionary  = {}
var blob_data: Dictionary = {} # STORES RELATED DATA from all macs

var roll_history_per_mac = {}   # mac -> Array[float]
var offset_per_mac = {}         # mac -> float
var calibration_count = {}
var roll_calib_sum = {}

const HISTORY_SIZE = 50
const CALIB_SAMPLES = 30

func get_smoothed_roll(mac: String) -> float:
	if not roll_history_per_mac.has(mac):
		return 0.0
	var hist = roll_history_per_mac[mac]
	if hist.size() == 0:
		return 0.0
	var arr = hist.duplicate()
	arr.sort()
	var mid = arr.size() / 2
	if arr.size() % 2 == 1:
		return arr[mid]
	else:
		return (arr[mid - 1] + arr[mid]) / 2.0
	
var latency_samples : Dictionary = {}

var drawing_scene = null

func register_drawing_scene(scene):
	drawing_scene = scene

func register_player_list_txt(node: ItemList) -> void:
	player_list_node_txt = node

func update_blob_data(data: Dictionary) -> void:
	var mac = data.get("mac_address", "")
	if mac != "":
		# Hantera kalibrering och smoothing för roll
		var ay = float(data.get("accel_x", 0.0))
		var az = float(data.get("accel_z", 0.0))
		var roll = atan2(-ay, az)
		
		if not roll_history_per_mac.has(mac):
			roll_history_per_mac[mac] = []
			roll_calib_sum[mac] = 0.0
			calibration_count[mac] = 0
			offset_per_mac[mac] = 0.0
			
		# Kalibrera först (samla samples)
		if calibration_count[mac] < CALIB_SAMPLES:
			roll_calib_sum[mac] += roll
			calibration_count[mac] += 1
			if calibration_count[mac] == CALIB_SAMPLES:
				offset_per_mac[mac] = roll_calib_sum[mac] / CALIB_SAMPLES
				print("Kalibrering klar för %s! Offset: %f" % [mac, offset_per_mac[mac]])
		else:
			var roll_adj = roll - offset_per_mac[mac]
			roll_history_per_mac[mac].append(roll_adj)
			if roll_history_per_mac[mac].size() > HISTORY_SIZE:
				roll_history_per_mac[mac].pop_front()
				
		blob_data[mac] = data
	_add_latency_sample(mac)
	_refresh_player_list()
	
	if drawing_scene:
		drawing_scene._on_sensor_data(mac, data)

func _add_latency_sample(mac: String) -> void:
	var d = blob_data[mac]
	var sent   = int(d.get("sent_timestamp_ms", 0))
	#var server = int(d.get("server_time_ms",   0))
	var now_sec = Time.get_unix_time_from_system()
	var now_ms = int(now_sec * 1000)
	var total  = now_ms - sent
	if not latency_samples.has(mac):
		latency_samples[mac] = []
	var arr = latency_samples[mac]
	arr.append(total)
	if arr.size() > 100:
		arr.pop_front()

func _refresh_player_list()-> void:
	if player_list_node_txt == null:
		return
	player_list_node_txt.clear()
	
	for mac in active_mac_map.keys():
		if not latency_samples.has(mac) or latency_samples[mac].size() == 0:
			player_list_node_txt.add_item("%s – … ms" % mac)
		else:
			var arr = latency_samples[mac]
			var sum = 0
			for v in arr:
				sum += v
			var avg = int(sum / arr.size())
			player_list_node_txt.add_item("%s – avg %d ms" % [mac, avg])
	

func get_blob_data(mac: String) -> Dictionary:
	if blob_data.has(mac):
		return blob_data[mac]
	return {}

func update_mac_map(mac: String, _color: String) -> void:
	if active_mac_map.has(mac):
		return # It already exists
	#it didnt exist
	print("added "+ mac +" to list with color ", _color)
	active_mac_map[mac] = _color

#retunerar map och macs
func get_active_macs_map() -> Dictionary:
	return active_mac_map

func set_current_scene(scene):
	current_scene_ref = scene

var red_icon = preload("res://assets/RÖD.png")
var green_icon = preload("res://assets/GRÖN.png")
var blue_icon = preload("res://assets/BLÅ.png")

func register_player_list(node):
	player_list_node = node
	
# Skapa en WebSocketPeer-instans
var socket: WebSocketPeer = WebSocketPeer.new()

# Variabel för att lagra senaste mottagna gyro_y
var sensor_joystick_y: float = 0.0
var sensor_gyro_y: float = 0.0
var color = "blue"

func _ready() -> void:
	# Försök att ansluta till websocket-servern.
	var err = socket.connect_to_url(websocket_url)
	if err != OK:
		print("Kunde inte ansluta till websocket på ", websocket_url)
		set_process(false)
	else:
		print("Ansluten till websocket på ", websocket_url)

func _process(_delta: float) -> void:
	# Viktigt: poll() måste anropas regelbundet för att hantera in-/utdata.
	socket.poll()
	var state = socket.get_ready_state()
	if state == WebSocketPeer.STATE_OPEN:
		# Medan det finns meddelanden i kön, hämta dem.
		while socket.get_available_packet_count() > 0:
			var received: String = socket.get_packet().get_string_from_utf8()
			#print(received)
			# Använd JSON-klassen för att parsa den mottagna strängen.
			var json := JSON.new()
			var parse_error = json.parse(received)
			
			if parse_error == OK:
				#print(received)
				# Hämta resultatet (ett Dictionary om allt gick bra)
				var data = json.get_data()
				
				if typeof(data) == TYPE_DICTIONARY:
						
					sensor_gyro_y = float(data.get("gyro_y", 0))
					sensor_joystick_y = float(data.get("joystick_y", 0))
					update_blob_data(data)
					# print("Mottaget sensor data: ", data)
					if data.has("color"):
						color = data["color"]
						#print(color)
					# Kolla om vi fått en lista över MAC-adresser:
					if data.has("mac_address"):
						# print("has mac")
						var mac_addr = data["mac_address"]
						if typeof(mac_addr) == TYPE_STRING:
							update_mac_map(mac_addr, color)
							update_mac_list_ui(mac_addr, color)
				else:
					print("Ogiltigt dataformat (ej dictionary): ", received)
			else:
				print("Kunde inte parsa JSON: ", received)

	elif state == WebSocketPeer.STATE_CLOSING:
		# Hantera ev. stängningslogik om du vill.
		pass
	elif state == WebSocketPeer.STATE_CLOSED:
		var code = socket.get_close_code()
		print("WebSocket stängd med kod: %d. Clean: %s" % [code, code != -1])
		set_process(false)

func is_valid_mac_address(mac: String) -> bool:
	# Must be 17 chars long (XX:XX:XX:XX:XX:XX)
	if mac.length() != 17:
		return false
		
	# Check format with colons
	var regex = RegEx.new()
	regex.compile("^([0-9A-F]{2}:){5}[0-9A-F]{2}$")
	if !regex.search(mac):
		return false
		
	# Not all zeros
	if mac == "00:00:00:00:00:00":
		return false
		
	# Not broadcast
	if mac == "FF:FF:FF:FF:FF:FF":
		return false
		
	return true

#lägger till nya macaddresser i listan
func update_mac_list_ui(mac_address: String, device_color: String) -> void:
	#FINNS LISTAN AV SPELARE
	if player_list_node:
		# Validate the MAC address - reject invalid formats and all-zeros
		if !is_valid_mac_address(mac_address):
			print("⚠️ Ignoring invalid MAC address:", mac_address)
			return
			
		var item_count = player_list_node.get_item_count() #Hur många spelare finns i listan redan
			
		#kolla om mac address finns
		for i in range(item_count):
			if player_list_node.get_item_text(i) == mac_address:
				return
		#IFALL DEN INTE FINNS
		#KOLLA VILKEN FÄRG DEN HAR OCH LÄGG TILL I LISTAN MED FÄRG
		if device_color == "red":
			player_list_node.add_item(mac_address, red_icon, true)
			print("✅ Added player with MAC: " + mac_address + " (red)")
		elif device_color == "blue":
			player_list_node.add_item(mac_address, blue_icon, true)
			print("✅ Added player with MAC: " + mac_address + " (blue)")
		else:
			player_list_node.add_item(mac_address, green_icon, true)
			print("✅ Added player with MAC: " + mac_address + " (green)")
			
	pass
