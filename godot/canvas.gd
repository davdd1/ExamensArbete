# game.gd
extends Node2D

var lives = 3
var bounce_count = 0

var macs = []
var current_mac_index = 0
@onready var paddle_container = $Paddle
@onready var ball_container = $Ball
@onready var spawn_timer = $Timer
@onready var tag = $DebugLable
@onready var game_over_panel = $GameOverPanel
@onready var game_over_label = $GameOverPanel/GameoverLable

var paddles = {} # mac -> Paddle Node

func _ready():
	WebSocketManager.register_drawing_scene(self)
	WebSocketManager.set_current_scene(self)
	$BRSLoop.play()
	var mac_map = WebSocketManager.get_active_macs_map()

	macs = mac_map.keys()
	current_mac_index = 0
	for i in range(macs.size()):
		var paddle = preload("res://Scenes/paddle.tscn").instantiate()
		paddle.assigned_mac = macs[i]
		paddle.position = Vector2( (i+1)*get_viewport().size.x/(macs.size()+1), get_viewport().size.y - 40 )
		paddle.set_color(mac_map[macs[i]])
		paddle_container.add_child(paddle)
		paddles[macs[i]] = paddle
	
	spawn_timer.start()

func _on_sensor_data(mac: String, _data: Dictionary):
	var smoothed_roll = WebSocketManager.get_smoothed_roll(mac)
	
	# Skicka till paddeln så den kan flytta sig
	if paddles.has(mac):
		paddles[mac].update_roll(smoothed_roll)

func _on_spawn_timer_timeout():
	#print("SPAWN BALL")
	var assigned_mac = macs[current_mac_index]
	current_mac_index = (current_mac_index + 1) % macs.size() # Växla varannan
	var ball = preload("res://Scenes/ball.tscn").instantiate()
	ball.assigned_mac = assigned_mac
	ball.position = Vector2(randf_range(30, get_viewport().size.x-30), -20)
	#ball.position = Vector2(300, 300)
	ball.velocity = Vector2(0, randf_range(200, 300)) # Fall rakt ner
	ball_container.add_child(ball)
	ball.connect("missed", Callable(self, "_on_ball_missed"))
	ball.connect("bounced", Callable(self, "_on_ball_bounced"))
	print("Ball added to ball_container: ", ball)
	if paddles.has(assigned_mac):
		var color = paddles[assigned_mac].color # eller hämta färg från paddel
		ball.set_color(color)
	
func _on_ball_missed():
	lives -= 1
	tag.text = "Liv: %d" % lives
	if lives <= 0:
		_game_over()

func _on_ball_bounced():
	$explo.play()
	bounce_count += 1
	print("Studs: %d" % bounce_count)
	# (Valfritt: uppdatera en label på skärmen)

func _game_over():
	print("Game over!")
	game_over_label.text = "GAME OVER\nStuds: %d" % bounce_count
	game_over_panel.visible = true
	$loss.play()
	get_tree().paused = true
	await get_tree().create_timer(3.0).timeout
	get_tree().paused = false
	get_tree().change_scene_to_file("res://Scenes/menu_scene.tscn") # Ändra sökvägen till din meny-scen
