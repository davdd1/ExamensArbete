extends Control

var selected_game_index = -1

var blob_node
var blob_velocity := Vector2.ZERO
var blob_speed = 200
var smooth_velocity := Vector2.ZERO

var blobs := {} # mac -> blob node
var blob_scene := preload("res://blob.tscn")

func _ready() -> void:
	WebSocketManager.set_current_scene(self)
	WebSocketManager.register_player_list(get_node("PanelContainer2/VBoxContainer2/VBoxContainer/playerList"))
	var game_list = get_node("GameChoice/VBoxContainer/GameList")
	game_list.add_item("Pong")
	game_list.add_item("Cool Game")
	game_list.add_item("Not so cool game")
	game_list.add_item("draw.io")

func _on_button_pressed() -> void:
	
	match selected_game_index:
		0:
			get_tree().change_scene_to_file("res://main.tscn")
		1: 
			print("selected Cool Game")
		2: 
			print("selected Not so cool game")
		_:
			print("nothing selected")
		
	return

func _process(delta: float) -> void:
	
	var screen_size = get_viewport_rect().size
	var center = screen_size / 2
	var blob_size = 20
	
func _on_game_list_item_selected(index: int) -> void:
	selected_game_index = index
	print("Picked index:",index)
	
func update_blob(data):
	var json = data

	var macs = json.get("mac_adresses", [])
	var color = Color(json.get("color", "white"))
	var gyro_x = json.get("joystick_x", 0.0)
	var gyro_y = -json.get("joystick_y", 0.0)
	
	var velocity = Vector2(gyro_x, gyro_y)
	
	for mac in macs:
		# Skapa blob om den inte finns
		if not blobs.has(mac):
			var blob = blob_scene.instantiate()
			add_child(blob)
			blob.position = get_viewport_rect().size / 2  # börja i mitten
			blob.color = color  # förutsatt att Blob har .color
			blob.mac = mac      # om du vill spara MAC internt
			blobs[mac] = blob
			print("🆕 Skapade blob för", mac)

		# Uppdatera rörelsen
		var blob = blobs[mac]
		blob.set_velocity(velocity)
	
	
	
