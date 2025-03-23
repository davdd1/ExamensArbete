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

// --- Konstanter och typer ---

// expectedSize definierar den förväntade storleken på inkommande UDP-paket (20 byte)
// Struktur: 1 byte (type) + 3 byte (padding) + 4 byte (player_id) + 4 byte (gyro_x) + 4 byte (gyro_y) + 4 byte (gyro_z)
const expectedSize = 20

// connectedDevices stores the MAC addresses of connected devices.
var connectedDevices []string

// SensorData representerar de sensorvärden vi tar emot (här fokuserar vi på gyro_x och gyro_y).
type SensorData struct {
	GyroX float32 `json:"gyro_x"`
	GyroY float32 `json:"gyro_y"`
}

// --- Globala variabler för websocket-hantering ---

// clients håller reda på alla anslutna websocket-klienter.
var (
	clients   = make(map[*websocket.Conn]bool)
	clientsMu sync.Mutex
)

// upgrader konverterar en inkommande HTTP-förfrågan till en websocket‑anslutning.
// Här tillåter vi anslutning från alla ursprung.
var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

// --- Websocket-hantering ---

// wsHandler hanterar nya websocket-anslutningar.
func wsHandler(w http.ResponseWriter, r *http.Request) {
	// Uppgradera HTTP-förfrågan till en websocket-anslutning.
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println("Fel vid uppgradering till websocket:", err)
		return
	}

	// Lägg till den nya klienten i vår globala lista.
	clientsMu.Lock()
	clients[conn] = true
	clientsMu.Unlock()
	log.Println("Ny websocket-klient ansluten.")

	// Håll anslutningen levande genom att läsa meddelanden från klienten.
	// Vi gör en evighetsloop; om ett läsfel uppstår (t.ex. att klienten kopplar ifrån) så tar vi bort klienten.
	for {
		if _, _, err := conn.ReadMessage(); err != nil {
			log.Println("Websocket läsfel:", err)
			break
		}
	}

	// Klienten kopplade ifrån – ta bort den från listan.
	clientsMu.Lock()
	delete(clients, conn)
	clientsMu.Unlock()
	conn.Close()
	log.Println("Websocket-klient frånkopplad.")
}

// startWebSocketServer startar en HTTP-server med websocket‑endpointen "/ws".
func startWebSocketServer() {
	http.HandleFunc("/ws", wsHandler)
	addr := ":8080" // Välj en port, här används 8080.
	log.Println("Websocket-server startad på", addr)
	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal("Fel vid start av websocket-server:", err)
	}
}

// broadcastSensorData skickar ett JSON‑meddelande med sensor‑data till alla anslutna websocket‑klienter.
func broadcastSensorData(gyroX, gyroY float32) {
	data := SensorData{
		GyroX: gyroX,
		GyroY: gyroY,
	}

	// Omvandla strukturen till en JSON‑sträng.
	message, err := json.Marshal(data)
	if err != nil {
		log.Println("Fel vid JSON-marshal:", err)
		return
	}

	// Skicka meddelandet till varje ansluten klient.
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

// --- UDP-mottagare ---

// udpReceiver lyssnar på UDP‑port 1234 och tar emot sensor‑data.
// När ett paket mottagits avkodas det och de relevanta värdena skickas vidare via websocket.
func udpReceiver() {
	// Lös upp adressen för UDP‑lyssning.
	udpAddr, err := net.ResolveUDPAddr("udp", ":1234")
	if err != nil {
		log.Fatalf("Kunde inte lösa UDP-adress: %v", err)
	}

	// Starta lyssnaren på UDP‑port 1234.
	conn, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		log.Fatalf("Kunde inte starta UDP-lyssnare: %v", err)
	}
	defer conn.Close()
	log.Println("UDP-mottagare körs på port 1234")

	// Buffer för inkommande data.
	buf := make([]byte, 1024)
	for {
		// Läs in data från UDP‑porten.
		n, adr, err := conn.ReadFromUDP(buf)
		if err != nil {
			log.Println("UDP läsfel:", err)
			continue
		}
		// Kontrollera att paketet har rätt storlek.
		if n < expectedSize && buf[0] != 0 {
			log.Printf("För kort paket: %d byte (förväntat %d)", n, expectedSize)
			continue
		}

		// ifall byte = 0 är det en connecton req, vi vill då spara dennes mac i en lista och skicka tillbaka en connection ack
		if buf[0] == 0 {

			print("Connection request received\n")
			handle_ACK_request(conn, adr, buf)

		} else {
			// Om det inte är en connection request så är det sensor data

			// Avkoda paketet enligt följande struktur:
			// Byte 0: type (ignoreras)
			// Byte 1-3: padding (ignoreras)
			// Byte 4-7: player_id (ignoreras här)
			// Byte 8-11: gyro_x (float32)
			// Byte 12-15: gyro_y (float32)
			// Byte 16-19: gyro_z (ignoreras)
			gyroX := math.Float32frombits(binary.LittleEndian.Uint32(buf[8:12]))
			gyroY := math.Float32frombits(binary.LittleEndian.Uint32(buf[12:16]))

			// Justera värdena med en skalfaktor och implementera en "dead zone"
			const factor = 0.5
			deltaX := float64(gyroX) * factor
			deltaY := float64(gyroY) * factor
			if math.Abs(deltaX) < 0.05 {
				deltaX = 0
			}
			if math.Abs(deltaY) < 0.05 {
				deltaY = 0
			}

			// Här skickar vi de mottagna värdena vidare via websocket.
			broadcastSensorData(gyroX, gyroY)

			// En kort paus för att undvika hög CPU-belastning.
			time.Sleep(10 * time.Millisecond)
		}
	}
}

// --- main ---

// I main() startar vi UDP-mottagaren och websocket-servern i separata gorutiner
// och blockerar sedan huvudtråden för att hålla programmet igång.
func main() {
	// Starta UDP-mottagaren.
	go udpReceiver()

	// Starta websocket-servern.
	go startWebSocketServer()

	// Blockera main-funktionen (vänta på evig körning).
	select {}
}

func handle_ACK_request(conn *net.UDPConn, addr *net.UDPAddr, buf []byte) {
	// printa ut mac adressen som kommer efter som Uint8_t [6] array
	// Hämta MAC-adress från buf[1:7]
	macAddr := buf[1:7]

	// Skriv ut MAC-adressen i hexadecimal form
	fmt.Printf("Mac address: %02X:%02X:%02X:%02X:%02X:%02X\n",
		macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5])
	// Convert MAC address to string format for comparison and storage.
	macAddrStr := fmt.Sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
		macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5])

	// Check if the MAC address is already in the list.
	deviceExists := false
	for _, device := range connectedDevices {
		if device == macAddrStr {
			deviceExists = true
			break
		}
	}

	// If not, add it to the list and send a connection acknowledgment.
	if !deviceExists {
		connectedDevices = append(connectedDevices, macAddrStr)
		fmt.Println("Device added to connected devices list:", macAddrStr)

		// Send connection acknowledgment (example response).
		ack := []byte{1}                     // Example ACK byte.
		_, err := conn.WriteToUDP(ack, addr) // Replace 'nil' with the sender's address if available.
		log.Println("Connection acknowledgment sent")
		if err != nil {
			log.Println("Failed to send connection acknowledgment:", err)
		}
	} else {
		fmt.Println("Device already connected:", macAddrStr)
	}
}
