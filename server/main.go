package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"math"
	"net"
)

const (
	// Vi förväntar oss 20 bytes per paket enligt standard‑alignment
	expectedSize = 20
	port         = ":1234"
)

// SensorPayload motsvarar ESP32:s sensor_payload_t
type SensorPayload struct {
	Type     uint8   // 1 byte
	PlayerID uint32  // 4 byte
	GyroX    float32 // 4 byte
	GyroY    float32 // 4 byte
	GyroZ    float32 // 4 byte
}

func main() {
	// Lös upp UDP-adressen och starta en UDP-lyssnare
	udpAddr, err := net.ResolveUDPAddr("udp", port)
	if err != nil {
		log.Fatalf("Kunde inte lösa UDP-adress: %v", err)
	}

	conn, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		log.Fatalf("Kunde inte starta UDP-lyssnare: %v", err)
	}
	defer conn.Close()

	fmt.Println("Lyssnar på UDP", port)

	for {
		// Läs in ett paket (använd en buffer som är större än paketstorleken)
		var buf [1024]byte
		n, addr, err := conn.ReadFromUDP(buf[:])
		if err != nil {
			log.Printf("Läsfel: %v", err)
			continue
		}

		// Kontrollera att vi fått rätt antal byten
		if n != expectedSize {
			log.Printf("Ovntad paketstorlek: %d byte (förväntat %d)", n, expectedSize)
			continue
		}

		// Avkoda den binära datan
		var payload SensorPayload
		// Byte 0: type
		payload.Type = buf[0]
		// Bytes 1-3: padding (hoppa över)
		// Bytes 4-7: player_id
		payload.PlayerID = binary.LittleEndian.Uint32(buf[4:8])
		// Bytes 8-11: gyro_x
		payload.GyroX = math.Float32frombits(binary.LittleEndian.Uint32(buf[8:12]))
		// Bytes 12-15: gyro_y
		payload.GyroY = math.Float32frombits(binary.LittleEndian.Uint32(buf[12:16]))
		// Bytes 16-19: gyro_z
		payload.GyroZ = math.Float32frombits(binary.LittleEndian.Uint32(buf[16:20]))

		// Skriv ut det mottagna paketet
		fmt.Printf("Mottaget från %v:\n  Type: %d\n  PlayerID: %d\n  GyroX: %.2f\n  GyroY: %.2f\n  GyroZ: %.2f\n",
			addr, payload.Type, payload.PlayerID, payload.GyroX, payload.GyroY, payload.GyroZ)

		// Skicka tillbaka ett svar (valfritt)
		_, err = conn.WriteToUDP([]byte("Mottaget OK\n"), addr)
		if err != nil {
			log.Printf("Fel vid sändning av svar: %v", err)
		}
	}
}
