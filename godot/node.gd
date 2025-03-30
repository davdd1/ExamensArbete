extends Node

# Ange websocket-URL:en (inkludera ws:// och porten)
@export var websocket_url: String = "ws://localhost:8080/ws"

var player_list_node = null

var red_icon = preload("res://RÖD.png")
var green_icon = preload("res://GRÖN.png")
var blue_icon = preload("res://BLÅ.png")

func register_player_list(node):
	player_list_node = node
# Skapa en WebSocketPeer-instans
var socket: WebSocketPeer = WebSocketPeer.new()

# Variabel för att lagra senaste mottagna gyro_y
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
			
			# Använd JSON-klassen för att parsa den mottagna strängen.
			var json := JSON.new()
			var parse_error = json.parse(received)
			if parse_error == OK:
				#print(received)
				# Hämta resultatet (ett Dictionary om allt gick bra)
				var data = json.get_data()
				if typeof(data) == TYPE_DICTIONARY:
					sensor_gyro_y = float(data.get("gyro_y", 0))
					# print("Mottaget sensor data: ", data)
					if data.has("color"):
						color = data["color"]
						print(color)
					# Kolla om vi fått en lista över MAC-adresser:
					if data.has("mac_adresses"):
						# print("has mac")
						var mac_list = data["mac_adresses"]
						if typeof(mac_list) == TYPE_ARRAY:
							update_mac_list_ui(mac_list, color)
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

#lägger till nya macaddresser i listan
func update_mac_list_ui(mac_addresses: Array, color: String) -> void:
	if player_list_node:
		#player_list_node.clear()
		
		for mac in mac_addresses:
			var exists = false
			var item_count = player_list_node.get_item_count()
			
			#kolla om mac address finns
			for i in range(item_count):
				if player_list_node.get_item_text(i) == mac:
					exists = true
					#print("mac finns redan")
					break
			if not exists:
				if color == "red":
					player_list_node.add_item(mac, red_icon, true)
					#print("added mac to list with red")	
				elif color == "blue":
					player_list_node.add_item(mac, blue_icon, true)
					#print("added mac to list with BLUE")	
				else:
					player_list_node.add_item(mac, green_icon, true)
					print("added mac to list with GREEN")	
		
	pass
