extends Control

var selected_game_index = -1

var blob_node
var blob_velocity := Vector2.ZERO
var blob_speed = 200
var smooth_velocity := Vector2.ZERO

var last_axis_y := {}       # mac → förra vel.x‐värdet

var menu_items := [] #kommer innehålla alla menuitems-noder
var item_positions := [] #Motsvarande globala positioner
var current_index := {} #mac_adress -> valt index

var blobs := {} # mac -> blob node
var blob_scene := preload("res://blob.tscn")

func _ready() -> void:
	WebSocketManager.set_current_scene(self)
	WebSocketManager.register_player_list(get_node("PanelContainer2/VBoxContainer2/VBoxContainer/playerList"))
	
	#hämta alla menyalternativ
	menu_items = get_tree().get_nodes_in_group("MenuItem")
	print("Hittade", menu_items.size(), "meny-items")
	# spara deras positioner
	for i in range(menu_items.size()):
		var it = menu_items[i]
		print("   [", i, "]", it.name, "@", it.global_position, "visible?", it.visible)
		item_positions.append(it.global_position)
	current_index.clear()

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

#Håller koll på vad som är vad i joystick datan
const DIRS = {
	Vector2(1,0): "Höger",
	Vector2(-1,0): "vänster",
	Vector2(0,1): "ner",
	Vector2(0,-1): "upp"
}

func _handle_button_press(mac:String) -> void:
	var idx = current_index[mac]
	var btn = menu_items[idx]            # MenuItem-knapp
	var txt = btn.text                   # “Start”, “Pong” osv

	if txt == "Start":
		# Bekräfta valt spel
		if selected_game_index >= 1:
			match selected_game_index:
				1:
					get_tree().change_scene_to_file("res://main.tscn")
				2:
					print("selected Cool Game")
				3:
					print("selected Not so cool game")
				4:
					print("selected draw.io")
		else:
			print("🚫 Inget spel valt – navigera till ett spel först!")
	else:
		# Det är ett spel-objekt → spara det i selected_game_index
		selected_game_index = idx
		print("✅ Valde spel:", txt)
		# (Frivilligt) sätt en visuell markering på just det spelet
		# t.ex. ändra outline-färg eller gör en blink-animation


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
			
			
			current_index[mac] = 0
			last_axis_y[mac]    = 0.0
		# Uppdatera rörelsen
		var blob = blobs[mac]
		blob.set_velocity(velocity)
		
	var vel = Vector2(data.get("joystick_x"), -data.get("joystick_y"))
	for mac in macs:
		var blob = blobs[mac]
		blob.set_velocity(vel)
		_navigate_menu(mac, vel)
		
		if data.get("button_state", false):
			_handle_button_press(mac)
	
func _navigate_menu(mac:String, vel:Vector2) -> void:
	var axis = vel.y
	var thr = 0.6   # justera 0.4–0.8 för mer/mindre känslighet

	# 1) Initiera vid behov
	if not current_index.has(mac):
		current_index[mac] = 0

	var idx    = current_index[mac]
	var prev   = last_axis_y[mac]
	var new_idx = idx

	# 2) Debounce på Y-axeln
	if axis > thr and prev <= thr:
		new_idx = min(idx + 1, menu_items.size() - 1)
	elif axis < -thr and prev >= -thr:
		new_idx = max(idx - 1, 0)

	last_axis_y[mac] = axis

	# 3) Om index ändrats – snap & outline
	if new_idx != idx:
		current_index[mac] = new_idx
		blobs[mac].global_position = item_positions[new_idx]
		_update_outline(mac, new_idx)
		print("📌 %s hoppade till [%d] %s" %
			  [mac, new_idx, menu_items[new_idx].name])
		
func _update_outline(mac:String, idx:int) -> void:
	#nolla alla
	for item in menu_items:
		if item.has_method("set_outline"):
			item.set_outline(Color(0,0,0,0))
	
	var chosen = menu_items[idx]
	if chosen.has_method("set_outline"):
		chosen.set_outline(blobs[mac].color)
		
	
	
	
