package main

import (
	"log"
	"sync"
	"time"

	"github.com/gorilla/websocket"
)

// Det packet vi tar emot från ESPn
type Packet struct {
	Type        int32
	MacAddr     [6]byte
	_           [2]byte // padding, for 4-byte alignment
	GyroX       float32
	GyroY       float32
	GyroZ       float32
	AccelX      float32
	AccelY      float32
	AccelZ      float32
	JoystickX   float32
	JoystickY   float32
	ButtonState uint8
	_           [3]byte // padding
	//Längre fram Batterylevel?
}

const packetSize = 4 /*Type*/ + 6 /*MacAddr*/ + 2 /*padding*/ + 8*4 /*8 floats*/ + 1 /*ButtonState*/ + 3 /*trailing padding*/
// -> 48 bytes

// SensorData representerar ett uppdaterat paket med alla sensordata och metadata.
type SensorData struct {
	GyroX        float32 `json:"gyro_x"`
	GyroY        float32 `json:"gyro_y"`
	GyroZ        float32 `json:"gyro_z"`
	AccelX       float32 `json:"accel_x,omitempty"`
	AccelY       float32 `json:"accel_y,omitempty"`
	AccelZ       float32 `json:"accel_z,omitempty"`
	JoystickX    float32 `json:"joystick_x"`
	JoystickY    float32 `json:"joystick_y"`
	ButtonState  bool    `json:"button_state,omitempty"`
	BatteryLevel float32 `json:"battery_level,omitempty"`
	MacAddr      string  `json:"mac_address"` // Ändrat till en sträng
	Color        string  `json:"color,omitempty"`
}

// Device represents a connected physical device
type Device struct {
	MacAddress   string          // MAC address as a string
	Color        string          // Assigned color
	LastSeen     int64           // Unix timestamp of last activity
	UDPAddresses map[string]bool // All known UDP addresses for this device
}

// Varibler för servern att spara clienter och deras färger
var (
	clients       = make(map[*websocket.Conn]bool)
	clientsMu     sync.Mutex
	deviceMap     = make(map[string]*Device) // MAC string -> Device
	deviceMapMu   sync.Mutex
	macToUDPMap   = make(map[string]string) // MAC string -> current UDP address
	udpToMACMap   = make(map[string]string) // UDP address -> MAC string
	colorList     = []string{"red", "green", "blue", "yellow", "purple", "orange", "cyan", "magenta"}
	colorIndex    = 0
	deviceTimeout = int64(5 * 60) // 5 minutes timeout in seconds
)

// Runs periodic maintenance tasks like cleaning up stale device entries
func maintenanceRoutine() {
	ticker := time.NewTicker(60 * time.Second) // Run every minute
	defer ticker.Stop()

	for {
		select {
		case <-ticker.C:
			log.Println("Running maintenance tasks...")
			cleanupStaleDevices()
		}
	}
}

func main() {
	log.Println("Starting server with", len(colorList), "available colors")

	// Start our main services
	go UdpReceiver()
	go StartWebSocketServer()
	go maintenanceRoutine()

	// Keep the main goroutine running
	select {}
}
