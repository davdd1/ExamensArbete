extends Node2D

var velocity := Vector2.ZERO
var smooth_velocity := Vector2.ZERO
@export var speed := 500
@export var radius := 20
var color := Color.RED
var mac := "Jesper"

func _ready():
	$macname.text = mac
	queue_redraw()

func set_velocity(v: Vector2):
	velocity = v

func _process(delta):
	smooth_velocity = smooth_velocity.lerp(velocity, 0.1)
	
	#if smooth_velocity.length() < 0.05:
		#print("")
		##position = position.lerp(get_viewport_rect().size / 2, 0.01)
	#else:
	position += smooth_velocity * delta * speed
	
	#KAN INTE ÅKA UTANFÖR SKRÄMEN
	var screen_size = get_viewport_rect().size
	position.x = clamp(position.x, radius, screen_size.x - radius)
	position.y = clamp(position.y, radius, screen_size.y - radius)

	# Flytta texten ovanför blobben
	$macname.position = Vector2(0, -radius - 10)
	queue_redraw()

func _draw():
	draw_circle(Vector2.ZERO, radius, color)
