extends Control

var selected_game_index = -1

var blob_node
var blob_velocity := Vector2.ZERO
var blob_speed = 200
var smooth_velocity := Vector2.ZERO
var MainScene := preload("res://main.tscn") as PackedScene
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
	print("Button press handled for MAC:", mac, " Item:", txt)
	if txt == "Start":
		# Bekräfta valt spel
		if selected_game_index >= 1:
			match selected_game_index:
				1:
					if not ResourceLoader.exists("res://main.tscn"):
						push_error("COULD NOT FIND FILE")
						return
					print("Försöker gå in tree")
					get_tree().change_scene_to_packed(MainScene)
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


func is_valid_mac(mac: String) -> bool:
	# Must be 17 chars long (XX:XX:XX:XX:XX:XX)
	if mac.length() != 17:
		return false
		
	# Check format
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

func update_blob(data):
	var json = data
	var player_list_node = get_node("PanelContainer2/VBoxContainer2/VBoxContainer/playerList")
	var mac_from_data = json.get("mac_address", "")
	
	# First validate the incoming MAC address
	if !is_valid_mac(mac_from_data):
		print("⚠️ Ignoring data with invalid MAC:", mac_from_data)
		return
		
	#Extraherar Mac-addresser från noden
	var macs = []
	if player_list_node:
		# Only process MACs that match the one in the data
		for i in range($PanelContainer2/VBoxContainer2/VBoxContainer/playerList.get_item_count()):
			var mac_address = $PanelContainer2/VBoxContainer2/VBoxContainer/playerList.get_item_text(i)
			if mac_address == mac_from_data:
				macs.append(mac_address)
				break
	else:
		#ifall vi inte hittar player_list_node
		print("⚠️ Could not find player_list_node")
		pass
	
	if macs.size() <= 0:
		#ifall ingen MAC matchar den i data
		# Add MAC to player list if it's valid but not yet in the list
		if is_valid_mac(mac_from_data):
			print("📋 Adding missing MAC to player list: ", mac_from_data)
			macs.append(mac_from_data)
			# The WebSocketManager will add it to the player list on next update
		else:
			return
		
	var color = Color(json.get("color", "white"))
	var gyro_x = json.get("joystick_x", 0.0)
	var gyro_y = -json.get("joystick_y", 0.0)
	
	var velocity = Vector2(gyro_x, gyro_y)
	
	for mac in macs:
		# Skip invalid MACs (double check, though our is_valid_mac already checked)
		if !is_valid_mac(mac):
			print("⚠️ Skipping invalid MAC:", mac)
			continue
			
		# Skapa blob om den inte finns and track separately to avoid redeclaration issue
		var blob_instance
		if not blobs.has(mac):
			blob_instance = blob_scene.instantiate()
			add_child(blob_instance)
			blob_instance.position = get_viewport_rect().size / 2  # börja i mitten
			blob_instance.color = color  # förutsatt att Blob har .color
			blob_instance.mac = mac      # om du vill spara MAC internt
			blobs[mac] = blob_instance
			print("🆕 Created blob for", mac)
			
			# Initialize menu navigation state for this controller
			current_index[mac] = 0
			last_axis_y[mac] = 0.0
		else:
			# Get existing blob if already created
			blob_instance = blobs[mac]
			
		# Uppdatera rörelsen
		blob_instance.set_velocity(velocity)
		_navigate_menu(mac, velocity)
		
		if json.get("button_state", false):
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
