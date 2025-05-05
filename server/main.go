package main

import (
	"encoding/json"
	"fmt"
	"log"
	"sync"
	"github.com/gorilla/websocket"
)

const expectedSize = 20

// SensorData innehåller endast en MAC per meddelande
type SensorData struct {
	GyroX    float32  `json:"gyro_x"`
	GyroY    float32  `json:"gyro_y"`
	MacList  []string `json:"mac_adresses,omitempty"`
	Color    string   `json:"color"`
}

var (
	clients     = make(map[*websocket.Conn]bool)
	clientsMu   sync.Mutex
	macMapping  = make(map[string]string) // UDP address string -> MAC string
	macColorMap = make(map[string]string) // MAC string -> färg
	colorList   = []string{"red", "green", "blue"}
	colorIndex  = 0
)


func broadcastSensorData(gyroX, gyroY float32, macAddr string, color string) {
	data := SensorData{
		GyroX:   gyroX,
		GyroY:   gyroY,
		MacList: []string{macAddr},
		Color:   color,
	}

	message, err := json.Marshal(data)
	if err != nil {
		log.Println("Fel vid JSON-marshal:", err)
		return
	}

	fmt.Println("JSON message:", string(message))

	clientsMu.Lock()
	defer clientsMu.Unlock()
	for conn := range clients {
		if err := conn.WriteMessage(websocket.TextMessage, message); err != nil {
			log.Println("Fel vid sändning till klient, tar bort klienten:", err)
			conn.Close()
			delete(clients, conn)
		}
	}
}


func main() {
	go UdpReceiver()
	go StartWebSocketServer()
	select {}
}
