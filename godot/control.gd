extends Control

var selected_game_index = -1

func _ready() -> void:
	WebSocketManager.register_player_list(get_node("PanelContainer2/VBoxContainer/playerList"))
	var game_list = get_node("GameChoice/GameList")
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


func _on_game_list_item_selected(index: int) -> void:
	selected_game_index = index
	print("Picked index:",index) 
