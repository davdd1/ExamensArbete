extends StaticBody2D

var win_height : int 
var p_height : int
const PLAYER_SPEED: float = 500

var velocity: float = 0.0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	win_height = get_viewport_rect().size.y
	p_height = $ColorRect.get_size().y


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	# Hämta noden som tar emot sensor-data
	var ws_node = get_node("/root/Main/WebSocketNode")
	
	# Beräkna den önskade hastigheten baserat på sensor-värdet (gyro_y)
	# Justera gärna PEDDLE_SPEED eller multipliceringsfaktorn efter behov.
	var target_speed = -ws_node.sensor_joystick_y * PLAYER_SPEED
	
	# Använd linjär interpolation (lerp) för att gradvis ändra 'velocity' mot 'target_speed'.
	# Det tredje argumentet (0.1) bestämmer hur snabbt vi närmar oss målet:
	# Ett lägre värde blir långsammare övergång (mer glid), ett högre värde ger snabbare reaktion.
	velocity = lerp(velocity, target_speed, 0.1)
	
	# Uppdatera positionen med den smidigt interpolerade hastigheten.
	position.y += velocity * delta
	
	# Begränsa paddle-rörelsen så att den håller sig inom fönstret.
	position.y = clamp(position.y, p_height / 2, win_height - p_height / 2)
