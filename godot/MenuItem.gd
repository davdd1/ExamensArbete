extends Button

var outline_color: Color = Color(0,0,0,0)

func set_outline(col: Color) -> void:
	outline_color = col
	queue_redraw() #tvingar omritning udaterar färg

func _draw() -> void:
	if outline_color.a > 0:
		#rita en rektangel runt knapp
		draw_rect(Rect2(Vector2.ZERO, size), outline_color, false, 3)
