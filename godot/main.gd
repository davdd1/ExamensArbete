#main.gd
#main script for pong
extends Sprite2D

var score := [0,0] #0:player 1:CPU

@onready var left_paddle = $leftPaddle
@onready var right_paddle = $rightPaddle
@onready var cpu = $CPUPaddle

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	WebSocketManager.set_current_scene(self)
	var map_macs = WebSocketManager.get_active_macs_map()
	var macs = map_macs.keys() # Get oss alla macs bara i listan
	
	#Assigna första två
	if macs.size() >= 1:
		left_paddle.assigned_mac = macs[0]
		_set_paddle_color(left_paddle, map_macs[macs[0]])
		left_paddle.is_cpu=false
	else:
		left_paddle.is_cpu
		
	if macs.size() >= 2:
		right_paddle.assigned_mac = macs[1]
		_set_paddle_color(right_paddle, map_macs[macs[1]])
		right_paddle.is_cpu = false
	else:
		right_paddle.is_cpu = true
		
	#Dölj CPUn om den inte används
	cpu.visible = false
	if cpu.has_node("CollisionShape2D"):
		var shape = cpu.get_node("CollisionShape2D")
		shape.disabled = not cpu.visible
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass


func _on_ball_timer_timeout() -> void:
	$ball.new_ball()


func _on_score_left_body_entered(body: Node2D) -> void:
	score[1] += 1
	$Hud/cpuScore.text = str(score[1])
	$BallTimer.start()

func _on_score_right_body_entered(body: Node2D) -> void:
	score[0] += 1
	$Hud/playerScore.text = str(score[0])
	$BallTimer.start()
	
func _set_paddle_color(paddle: Node, color_str: String) -> void:
	var c = Color(color_str)
	if paddle.has_node("ColorRect"):
		paddle.get_node("ColorRect").color = c
	else:
		print("Nope inge färg")
