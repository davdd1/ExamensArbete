package main

import (
	"log"
	"net"
	"fmt"
	"math"
	"encoding/binary"
	"time"
)


func UdpReceiver() {
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

			color := macColorMap[macStr]
			broadcastSensorData(gyroX, gyroY, macStr, color)
			time.Sleep(10 * time.Millisecond)
		}
	}
}

func handle_ACK_request(conn *net.UDPConn, addr *net.UDPAddr, buf []byte) {
	// TODO FIXA SÅ VI KOLLAR MACADDRES PÅ RÄTT STÄLLE
	macAddr := buf[1:7]
	macStr := fmt.Sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
		macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5])

	macMapping[addr.String()] = macStr
	fmt.Println("Device mapped:", addr.String(), "->", macStr)

	// Tilldela unik färgindex
	var assignedIndex int
	if _, exists := macColorMap[macStr]; !exists {
		assignedIndex = colorIndex % len(colorList)
		macColorMap[macStr] = colorList[assignedIndex]
		colorIndex++
	} else {
		// Hitta index på redan tilldelad färg
		color := macColorMap[macStr]
		for i, c := range colorList {
			if c == color {
				assignedIndex = i
				break
			}
		}
	}

	// Skicka: byte 0 = ACK, byte 1 = färgindex 0 red, 1 green, 2 blue
	ack := []byte{1, byte(assignedIndex)}
	_, err := conn.WriteToUDP(ack, addr)
	if err != nil {
		log.Println("Failed to send connection acknowledgment with index:", err)
	} else {
		log.Printf("ACK sent to %s with color index %d (%s)\n", addr.String(), assignedIndex, colorList[assignedIndex])
	}
}