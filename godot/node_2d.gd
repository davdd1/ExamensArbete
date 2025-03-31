extends Node2D

var velocity = Vector2.ZERO

func _process(delta):
	position += velocity * delta * speed
