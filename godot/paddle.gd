# Paddle.gd
extends ColorRect
var assigned_mac = ""
var sensitivity = 1.0
var current_roll = 0.0
var smooth_speed = 8.0 # Ju högre, desto snabbare (testa dig fram)
var target_x = 0.0


func _set_color(color_str):
	color = Color(color_str)

func update_roll(smoothed_roll):
	current_roll = smoothed_roll

func _process(delta):
	if assigned_mac == "":
		return
	var norm_x = clamp(current_roll / (PI/2.0), -1, 1)
	var vp = get_viewport().size
	target_x = vp.x/2 + norm_x * (vp.x/2 - size.x/2) * sensitivity
	position.x = lerp(position.x, target_x, smooth_speed * delta)
