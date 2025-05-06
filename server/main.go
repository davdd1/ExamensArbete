package main

import (
	"sync"
	"github.com/gorilla/websocket"
)

//Det packet vi tar emot från ESPn
type Packet struct {
	Type uint8
	_ [3]byte // padding - need exaplain why this is needed
	PlayerID uint32
	GyroX float32
	GyroY float32
	GyroZ float32
	//Längre fram AccelX, AccelY, AccelZ, JoystickX, JoystickY, ButtonState, Batterylevel?
}

const packetSize = 1 + 3 + 4*4 // ÄNDRA DENNA OM DU LÄGGER TILL FLER 

// SensorData innehåller endast en MAC per meddelande
// detta skickas till Godot via WebSocket
type SensorData struct {
    PlayerID uint32    `json:"player_id"`
    GyroX    float32   `json:"gyro_x"`
    GyroY    float32   `json:"gyro_y"`
    GyroZ    float32   `json:"gyro_z"`
    // framöver: AccelX, AccelY, AccelZ, JoystickX, JoystickY…
    MacList  []string  `json:"mac_adresses"`
    Color    string    `json:"color"`
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
