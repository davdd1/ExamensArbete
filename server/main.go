package main

import (
	"encoding/binary"
	"encoding/json"
	"fmt"
	"log"
	"math"
	"net"
	"net/http"
	"sync"
	"time"

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
	clients    = make(map[*websocket.Conn]bool)
	clientsMu  sync.Mutex
	macMapping = make(map[string]string) // UDP address string -> MAC string
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

func wsHandler(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println("Fel vid uppgradering till websocket:", err)
		return
	}

	clientsMu.Lock()
	clients[conn] = true
	clientsMu.Unlock()
	log.Println("Ny websocket-klient ansluten.")

	for {
		if _, _, err := conn.ReadMessage(); err != nil {
			log.Println("Websocket läsfel:", err)
			break
		}
	}

	clientsMu.Lock()
	delete(clients, conn)
	clientsMu.Unlock()
	conn.Close()
	log.Println("Websocket-klient frånkopplad.")
}

func startWebSocketServer() {
	http.HandleFunc("/ws", wsHandler)
	addr := ":8080"
	log.Println("Websocket-server startad på", addr)
	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal("Fel vid start av websocket-server:", err)
	}
}

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

func udpReceiver() {
	udpAddr, err := net.ResolveUDPAddr("udp", ":1234")
	if err != nil {
		log.Fatalf("Kunde inte lösa UDP-adress: %v", err)
	}

	conn, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		log.Fatalf("Kunde inte starta UDP-lyssnare: %v", err)
	}
	defer conn.Close()
	log.Println("UDP-mottagare körs på port 1234")

	buf := make([]byte, 1024)
	for {
		n, addr, err := conn.ReadFromUDP(buf)
		if err != nil {
			log.Println("UDP läsfel:", err)
			continue
		}

		if n < expectedSize && buf[0] != 0 {
			log.Printf("För kort paket: %d byte (förväntat %d)", n, expectedSize)
			continue
		}

		if buf[0] == 0 {
			fmt.Println("Connection request received")
			handle_ACK_request(conn, addr, buf)
		} else {
			gyroX := math.Float32frombits(binary.LittleEndian.Uint32(buf[8:12]))
			gyroY := math.Float32frombits(binary.LittleEndian.Uint32(buf[12:16]))

			const factor = 0.5
			deltaX := float64(gyroX) * factor
			deltaY := float64(gyroY) * factor
			if math.Abs(deltaX) < 0.05 {
				deltaX = 0
			}
			if math.Abs(deltaY) < 0.05 {
				deltaY = 0
			}

			macStr := macMapping[addr.String()]
			if macStr == "" {
				log.Println("MAC-adress saknas för", addr.String())
				continue
			}

			broadcastSensorData(gyroX, gyroY, macStr, "blue")
			time.Sleep(10 * time.Millisecond)
		}
	}
}

func handle_ACK_request(conn *net.UDPConn, addr *net.UDPAddr, buf []byte) {
	macAddr := buf[1:7]
	macStr := fmt.Sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
		macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5])

	macMapping[addr.String()] = macStr
	fmt.Println("Device mapped:", addr.String(), "->", macStr)

	ack := []byte{1}
	_, err := conn.WriteToUDP(ack, addr)
	if err != nil {
		log.Println("Failed to send connection acknowledgment:", err)
	} else {
		log.Println("Connection acknowledgment sent to", addr.String())
	}
}

func main() {
	go udpReceiver()
	go startWebSocketServer()
	select {}
}
