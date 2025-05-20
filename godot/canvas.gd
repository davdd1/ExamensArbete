extends Node2D

# Smoothed display of roll, pitch and yaw using moving median over HISTORY_SIZE samples
# Initial calibration: hold controller flat & forward for CALIB_SAMPLES readings
@export var sensitivity := 1.0               # Scale factor for mapping normalized angles to screen
@export var yaw_range_deg := 45.0            # Degrees of yaw maps to full screen width
@export var pitch_range_deg := 45.0          # Degrees of pitch maps to full screen height
const HISTORY_SIZE := 100                   # Number of samples for median smoothing
const CALIB_SAMPLES := 50                   # Samples for initial offset calibration                   # Samples for initial offset calibration

# Per-device state
var gz_bias_per_device := {}                 # { mac: float }
var calibration_count := {}                  # { mac: int }
var pitch_calib_sum := {}                    # { mac: float }
var yaw_calib_sum := {}                      # { mac: float }
var offset_per_device := {}                  # { mac: { "pitch": float, "yaw": float } }

var yaw_history_per_device := {}             # { mac: Array }
var pitch_history_per_device := {}           # { mac: Array }
var roll_history_per_device := {}            # { mac: Array }
var pointer_per_device := {}                 # { mac: Sprite2D }

# Debug label (must exist as a child node named 'DebugLabel')
@onready var debug_label := $"../Label"

func _ready():
	WebSocketManager.register_drawing_scene(self)
	debug_label.text = "Place controller flat & forward for calibration..."

func _on_sensor_data(mac: String, data: Dictionary) -> void:
	# Read raw sensor data
	var ax = data.get("accel_x", 0.0)
	var ay = data.get("accel_y", 0.0)
	var az = data.get("accel_z", 0.0)
	var gz = data.get("gyro_z", 0.0)
	var dt = get_process_delta_time()

	# Initialize bias, calibration accumulators, and histories on first packet
	if not gz_bias_per_device.has(mac):
		gz_bias_per_device[mac] = gz
		calibration_count[mac] = 0
		pitch_calib_sum[mac] = 0.0
		yaw_calib_sum[mac] = 0.0
		yaw_history_per_device[mac] = []
		pitch_history_per_device[mac] = []
		roll_history_per_device[mac] = []
		debug_label.text = "Calibrating... 0%"
		print("[Debug] Initialization for ", mac)

	# Compute raw roll, pitch, yaw (gyro only)
	var roll = atan2(ay, az)
	var pitch = atan2(-ax, sqrt(ay * ay + az * az))
	# Calibrate gyro_z bias
	var gz_cal = gz - gz_bias_per_device[mac]
	# Integrate yaw from gyro_z
	var last_yaw := 0.0
	if yaw_history_per_device[mac].size() > 0:
		last_yaw = yaw_history_per_device[mac][yaw_history_per_device[mac].size() - 1]
	var yaw = last_yaw + deg_to_rad(gz_cal) * dt

	# Calibration phase: accumulate first CALIB_SAMPLES samples
	if calibration_count[mac] < CALIB_SAMPLES:
		pitch_calib_sum[mac] += pitch
		yaw_calib_sum[mac] += yaw
		calibration_count[mac] += 1
		debug_label.text = "Calibrating... %d%%" % int(calibration_count[mac] * 100 / CALIB_SAMPLES)
		if calibration_count[mac] == CALIB_SAMPLES:
			# Compute offsets
			var pitch_offset = pitch_calib_sum[mac] / CALIB_SAMPLES
			var yaw_offset = yaw_calib_sum[mac] / CALIB_SAMPLES
			offset_per_device[mac] = { "pitch": pitch_offset, "yaw": yaw_offset }
			debug_label.text = "Calibration complete!"
			print("[Debug] Offsets for %s -> pitch: %f, yaw: %f" % [mac, pitch_offset, yaw_offset])
		return

	# Adjust by offsets
	var pitch_adj = pitch - offset_per_device[mac]["pitch"]
	var yaw_adj   = yaw   - offset_per_device[mac]["yaw"]
	print("[Debug] Adjusted pitch: ", pitch_adj, ", yaw: ", yaw_adj)

	# Append to histories and trim
	_push_history(pitch_history_per_device[mac], pitch_adj)
	_push_history(yaw_history_per_device[mac], yaw_adj)
	_push_history(roll_history_per_device[mac], roll)

	# Median smoothing
	var med_pitch = _median_history(pitch_history_per_device[mac])
	var med_yaw   = _median_history(yaw_history_per_device[mac])
	var med_roll  = _median_history(roll_history_per_device[mac])
	print("[Debug] Median pitch: %f, yaw: %f" % [med_pitch, med_yaw])

	# Update debug label
	debug_label.text = "Roll: %.2f°, Pitch: %.2f°, Yaw: %.2f°" % [rad_to_deg(med_roll), rad_to_deg(med_pitch), rad_to_deg(med_yaw)]

	# Update pointer position using yaw (X) and pitch (Y)
	_update_pointer_position(mac, med_yaw, med_pitch)

func _push_history(hist: Array, value: float) -> void:
	hist.append(value)
	if hist.size() > HISTORY_SIZE:
		hist.clear()

func _median_history(hist: Array) -> float:
	if hist.size() == 0:
		return 0.0
	var arr = hist.duplicate()
	arr.sort()
	var mid = arr.size() / 2
	if arr.size() % 2 == 1:
		return arr[mid]
	else:
		return (arr[mid - 1] + arr[mid]) / 2.0

func _update_pointer_position(mac: String, yaw: float, pitch: float) -> void:
	# Normalize yaw [-π..π] to [-1..1]
	var norm_x = clamp(yaw / PI, -1, 1)
	# Normalize pitch [-π/2..π/2] to [-1..1]
	var norm_y = clamp(pitch / (PI / 2.0), -1, 1)
	print("[Debug] Normalized - X: %f, Y: %f" % [norm_x, norm_y])

	# Map to screen coordinates
	var vp = get_viewport().size
	var center = vp / 2
	var target_x = center.x + norm_x * center.x * sensitivity
	var target_y = center.y + norm_y * center.y * sensitivity
	var target_pos = Vector2(target_x, target_y)
	print("[Debug] Target position: %s" % str(target_pos))

	# Create pointer sprite if missing
	if not pointer_per_device.has(mac):
		var ptr = Sprite2D.new()
		ptr.texture = preload("res://assets/icon.svg")
		ptr.centered = true
		ptr.position = center
		add_child(ptr)
		pointer_per_device[mac] = ptr
		print("[Debug] Created pointer for %s" % mac)

	pointer_per_device[mac].position = target_pos
