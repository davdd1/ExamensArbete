package main

import (
	"log"
	"net"
	"fmt"
	"encoding/binary"
	"bytes"
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

		if n < packetSize {
			log.Printf("För kort paket: %d byte (förväntat %d)", n, packetSize)
			continue
		}

		switch buf[0] {
		// 0 = connection request, 1 = sensor data
		case 0:
			fmt.Println("Connection request received")
			//skickar ACK till klient med bla. färgindex
			handle_ACK_request(conn, addr, buf)
		case 1:
			//hantera sensor data
			var pkt Packet // SKAPA packet srtruct 
			//läser in i paketet från buffern

			if err := binary.Read(bytes.NewReader(buf[:packetSize]), binary.LittleEndian, &pkt); err != nil {
				log.Println("Binary read failed:", err)
				continue
			}
			//hanterar sensor datan
			handleSensor(pkt, addr)
			
		}
	}
}

func handle_ACK_request(conn *net.UDPConn, addr *net.UDPAddr, buf []byte) {
	// TODO FIXA SÅ VI KOLLAR MACADDRES PÅ RÄTT STÄLLE
	macAddr := buf[4:10] // MAC-adressen är i bytes 4-9
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


func handleSensor(pkt Packet, addr *net.UDPAddr) {
	//börjar med att kolla om vi har en MAC-mappning för den här adressen
    macStr := macMapping[addr.String()]
    if macStr == "" {
        log.Println("Ingen MAC-mappning för", addr)
        return
    }
	// Kolla färgen för Mac-adressen, så vi kan skicka den till godot
    color := macColorMap[macStr]

    // Skicka vidare ALLA fält till WebSocket
    data := SensorData{
        PlayerID: pkt.PlayerID,
        GyroX:    pkt.GyroX,
        GyroY:    pkt.GyroY,
        GyroZ:    pkt.GyroZ,
        MacList:  []string{macStr},
        Color:    color,
    }
    BroadcastSensorData(data)
}