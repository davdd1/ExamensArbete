#node.gd
#handles websocket data mainly
extends Node

# Ange websocket-URL:en (inkludera ws:// och porten)
@export var websocket_url: String = "ws://localhost:8080/ws"

var player_list_node = null
var current_scene_ref
var active_mac_map: Dictionary  = {}
var blob_data: Dictionary = {} # STORES LATED DATA from all macs

func update_blob_data(data: Dictionary) -> void:
	var mac = data.get("mac_address", "")
	if mac != "":
		blob_data[mac] = data

func get_blob_data(mac: String) -> Dictionary:
	if blob_data.has(mac):
		return blob_data[mac]
	return {}

func update_mac_map(mac: String, color: String) -> void:
	if active_mac_map.has(mac):
		return # It already existist
	#it didnt exist
	print("added "+ mac +" to list with color ", color)
	active_mac_map[mac] = color

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

func _process(delta: float) -> void:
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
