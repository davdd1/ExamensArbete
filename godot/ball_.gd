extends Node2D

var velocity = Vector2.ZERO

signal bounced
signal missed
var assigned_mac = ""
func _ready():
	print("Ball ready!")
	set_process(true) # Behövs inte egentligen på Node2D, men skadar inte

func set_color(c):
	$ColorRect.color = c # eller beroende på din Ball-scenstruktur

func _process(delta):
	position += velocity * delta

	# Kolla om bollen är utanför nedre skärmen
	if position.y > get_viewport().size.y:
		#print("Ball out of bounds, queue_free()")
		emit_signal("missed")
		queue_free() # Missad boll, ta bort

	# Kolla kollision mot alla paddlar
	# OBS: Anpassa path så det funkar med din scenstruktur!
	for paddle in get_parent().get_parent().paddle_container.get_children():
		if _intersects_paddle(paddle) and velocity.y > 0 and paddle.assigned_mac == assigned_mac:
			velocity.y = -abs(velocity.y)
			position.y = paddle.position.y - get_viewport().size.y/2 + 255 # hoppa uppåt direkt
			emit_signal("bounced")
			print("Ball bounced on paddle!")

# Kollision mellan bollen och en paddle
func _intersects_paddle(paddle):
	var my_rect = Rect2(position - Vector2(12, 12), Vector2(24, 24)) # 24x24 är ColorRect-storlek
	var pad_rect = Rect2(paddle.position - paddle.size/2, paddle.size)
	return my_rect.intersects(pad_rect)
