package main

import (
	"sync"
	"github.com/gorilla/websocket"
)

//Det packet vi tar emot från ESPn
type Packet struct {
	Type uint8
	_ [3]byte // padding - need exaplain why this is needed
	GyroX float32
	GyroY float32
	GyroZ float32
	AccelX float32
	AccelY float32
	AccelZ float32
	JoystickX float32
	JoystickY float32
	ButtonState uint8
	//Längre fram AccelX, AccelY, AccelZ, JoystickX, JoystickY, ButtonState, Batterylevel?
}

const packetSize = 1 /*Type*/ + 3 /*pad*/ + 8*4 /*8 floats*/ + 1 /*ButtonState*/ + 3 /*trailing pad*/
// -> 40

// SensorData representerar ett uppdaterat paket med alla sensordata och metadata.
type SensorData struct {
    GyroX        float32   `json:"gyro_x"`                    // Gyroskop X
    GyroY        float32   `json:"gyro_y"`                    // Gyroskop Y
    GyroZ        float32   `json:"gyro_z"`                    // Gyroskop Z
    AccelX       float32   `json:"accel_x,omitempty"`         // Accelerometer X (kan vara 0 om ej skickat)
    AccelY       float32   `json:"accel_y,omitempty"`         // Accelerometer Y
    AccelZ       float32   `json:"accel_z,omitempty"`         // Accelerometer Z
    JoystickX    float32   `json:"joystick_x"`      // Joystick X
    JoystickY    float32   `json:"joystick_y"`      // Joystick Y
    ButtonState  bool      `json:"button_state,omitempty"`    // Knapp intryckt eller inte
    BatteryLevel float32   `json:"battery_level,omitempty"`   // Batterinivå i Volt eller procent
    MacList      []string  `json:"mac_adresses"`   // Lista med MAC-adresser för enheter i närheten
    Color        string    `json:"color,omitempty"`           // Valfri färginformation, t.ex. för UI
}

// Varibler för servern att spara clienter och deras färger
var (
	clients     = make(map[*websocket.Conn]bool)
	clientsMu   sync.Mutex
	macMapping  = make(map[string]string) // UDP address string -> MAC string
	macColorMap = make(map[string]string) // MAC string -> färg
	colorList   = []string{"red", "green", "blue"}
	colorIndex  = 0
)


func main() {
	go UdpReceiver()
	go StartWebSocketServer()
	select {}
}
