extends Node

# Ange websocket-URL:en (OBS: inkludera ws:// och porten)
@export var websocket_url: String = "ws://localhost:8080/ws"

# Skapa en WebSocketPeer-instans.
var socket: WebSocketPeer = WebSocketPeer.new()

# Variabel för att lagra senaste mottagna gyro_y
var sensor_gyro_y: float = 0.0

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
				# Hämta resultatet (ett Dictionary om allt gick bra)
				var data = json.get_data()
				if typeof(data) == TYPE_DICTIONARY:
					sensor_gyro_y = float(data.get("gyro_y", 0))
					print("Mottaget sensor data: ", data)
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
