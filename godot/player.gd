#player.gd
extends StaticBody2D

var win_height : int 
var p_height : int
const PLAYER_SPEED: float = 500
var velocity: float = 0.0
var color = ""

@export var assigned_mac: String = ""
@export var is_cpu: bool = false

#sparar senast data per controller
var last_data := {}

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	win_height = get_viewport_rect().size.y
	p_height = $ColorRect.get_size().y
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if is_cpu:
		_process_cpu(delta)
		pass
	else:
		var data = WebSocketManager.get_blob_data(assigned_mac)
		
		var target_speed = -data.get("joystick_y", 0.0) * PLAYER_SPEED
		velocity = lerp(velocity, target_speed, 0.1)
		position.y += velocity * delta
		position.y = clamp(position.y, p_height/2, win_height-p_height/2)
	# Hämta noden som tar emot sensor-data

func _process_cpu(delta: float) -> void:
	var ball = $"../ball"
	var dist = position.y - ball.position.y
	var move_by = 0.0
	if abs(dist) > PLAYER_SPEED * delta:
		move_by = PLAYER_SPEED * delta * (dist / abs(dist))
	else:
		move_by = dist
		
	position.y -= move_by
	position.y = clamp(position.y, p_height/2, win_height-p_height/2)
