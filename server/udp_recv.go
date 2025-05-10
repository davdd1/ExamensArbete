package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"math"
	"net"
)

func decodePacket(buf []byte) Packet {
	// buf must be at least packetSize bytes
	return Packet{
		Type:    int32(binary.LittleEndian.Uint32(buf[0:])),
		MacAddr: [6]byte{buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]},
		// skip buf [10-11], padding
		GyroX:       math.Float32frombits(binary.LittleEndian.Uint32(buf[12:])),
		GyroY:       math.Float32frombits(binary.LittleEndian.Uint32(buf[16:])),
		GyroZ:       math.Float32frombits(binary.LittleEndian.Uint32(buf[20:])),
		AccelX:      math.Float32frombits(binary.LittleEndian.Uint32(buf[24:])),
		AccelY:      math.Float32frombits(binary.LittleEndian.Uint32(buf[28:])),
		AccelZ:      math.Float32frombits(binary.LittleEndian.Uint32(buf[32:])),
		JoystickX:   math.Float32frombits(binary.LittleEndian.Uint32(buf[36:])),
		JoystickY:   math.Float32frombits(binary.LittleEndian.Uint32(buf[40:])),
		ButtonState: buf[44],
		// skip buf [45-47], padding
	}
}

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

		if n < packetSize && buf[0] != 0 {
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
			pkt := decodePacket(buf)
			handleSensor(pkt, addr)
		}
	}
}

func isValidMACAddress(mac string) bool {
	// Basic format check: XX:XX:XX:XX:XX:XX where X is hex digit
	if len(mac) != 17 {
		return false
	}

	// Check format with colons
	for i, c := range mac {
		if i%3 == 2 {
			if c != ':' && i < 15 {
				return false
			}
		} else {
			if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
				return false
			}
		}
	}

	// Check it's not all zeroes
	if mac == "00:00:00:00:00:00" {
		return false
	}

	return true
}

func handle_ACK_request(conn *net.UDPConn, addr *net.UDPAddr, buf []byte) {
	// Validate packet format and extract MAC address
	if len(buf) < 10 {
		log.Println("Invalid connection request packet: too short")
		return
	}

	// MAC-adressen är i bytes 4-9
	macAddr := buf[4:10]
	macStr := fmt.Sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
		macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5])

	// Validate MAC address format (simple check)
	if !isValidMACAddress(macStr) {
		log.Printf("Rejected invalid MAC address: %s from %s", macStr, addr.String())
		return
	}

	// Check for all zeros MAC address which is invalid
	allZeros := true
	for _, b := range macAddr {
		if b != 0 {
			allZeros = false
			break
		}
	}

	if allZeros {
		log.Printf("Rejected all-zeros MAC address from %s", addr.String())
		return
	}

	// Lock access to device data structures
	deviceMapMu.Lock()
	defer deviceMapMu.Unlock()
	// Check if this UDP address is already mapped to a different MAC
	addrStr := addr.String()
	existingMAC, hasMapping := udpToMACMap[addrStr]
	if hasMapping && existingMAC != macStr {
		log.Printf("Warning: UDP address %s changing MAC from %s to %s",
			addrStr, existingMAC, macStr)

		// Remove the old UDP-to-MAC mapping
		if oldDevice, exists := deviceMap[existingMAC]; exists {
			delete(oldDevice.UDPAddresses, addrStr)
			log.Printf("Removed mapping from %s to %s", addrStr, existingMAC)
		}

		// If we're getting multiple MACs from the same UDP address,
		// something unusual is happening - log it
		log.Printf("⚠️ Multiple MACs from same UDP address: %s → %s, now %s",
			addrStr, existingMAC, macStr)
	}

	// Get or create device entry
	device, exists := deviceMap[macStr]
	if !exists {
		// Create new device entry
		device = &Device{
			MacAddress:   macStr,
			Color:        colorList[colorIndex%len(colorList)],
			LastSeen:     currentTimestamp(),
			UDPAddresses: make(map[string]bool),
		}
		deviceMap[macStr] = device
		colorIndex++
		log.Printf("New device registered: %s with color %s", macStr, device.Color)
	} else {
		// Update existing device
		device.LastSeen = currentTimestamp()
		log.Printf("Known device reconnected: %s with color %s", macStr, device.Color)
	}

	// Update address mappings
	device.UDPAddresses[addrStr] = true

	// Update bidirectional maps
	udpToMACMap[addrStr] = macStr
	macToUDPMap[macStr] = addrStr

	// Find color index for response
	var assignedIndex int
	for i, c := range colorList {
		if c == device.Color {
			assignedIndex = i
			break
		}
	}

	// Send ACK with color index
	ack := []byte{1, byte(assignedIndex)}
	_, err := conn.WriteToUDP(ack, addr)
	if err != nil {
		log.Println("Failed to send connection acknowledgment with index:", err)
	} else {
		log.Printf("ACK sent to %s with color index %d (%s)\n", addrStr, assignedIndex, device.Color)
	}

	fmt.Println("Device mapped:", addrStr, "->", macStr)
}

func handleSensor(pkt Packet, addr *net.UDPAddr) {
	addrStr := addr.String()

	// Get the device lock for safe access
	deviceMapMu.Lock()
	defer deviceMapMu.Unlock()

	// Hämta MAC-sträng för den här UDP-adressen
	macStr := udpToMACMap[addrStr]
	if macStr == "" {
		// Try to get MAC address from the packet itself
		macFromPkt := fmt.Sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
			pkt.MacAddr[0], pkt.MacAddr[1], pkt.MacAddr[2],
			pkt.MacAddr[3], pkt.MacAddr[4], pkt.MacAddr[5])

		// Check for all zeros MAC (invalid)
		allZeros := true
		for _, b := range pkt.MacAddr {
			if b != 0 {
				allZeros = false
				break
			}
		}

		if allZeros {
			log.Printf("Rejected sensor data with all-zeros MAC from %s", addrStr)
			return
		}

		// Additional validation for reasonable MAC values
		// Prevent obviously invalid MACs (like FF:FF:FF:FF:FF:FF broadcast)
		if macFromPkt == "FF:FF:FF:FF:FF:FF" {
			log.Printf("Rejected sensor data with broadcast MAC from %s", addrStr)
			return
		}
		if isValidMACAddress(macFromPkt) {
			log.Printf("Auto-registering device from data packet: %s", macFromPkt)

			// Check if this MAC is already registered with a different UDP
			existingAddr, hasMACMapping := macToUDPMap[macFromPkt]
			if hasMACMapping && existingAddr != addrStr {
				log.Printf("⚠️ MAC %s previously seen at %s, now at %s",
					macFromPkt, existingAddr, addrStr)

				// Handle MAC address spoofing - decide whether to allow or block
				// For now, log but allow the connection
				log.Printf("⚠️ Possible MAC address spoofing detected! %s", macFromPkt)
			}

			// This is a valid MAC but we don't have a mapping yet
			// Let's create a device entry on the fly
			device, exists := deviceMap[macFromPkt]
			if !exists {
				device = &Device{
					MacAddress:   macFromPkt,
					Color:        colorList[colorIndex%len(colorList)],
					LastSeen:     currentTimestamp(),
					UDPAddresses: make(map[string]bool),
				}
				deviceMap[macFromPkt] = device
				colorIndex++
				log.Printf("New device auto-registered: %s with color %s", macFromPkt, device.Color)
			}

			// Update mappings
			device.UDPAddresses[addrStr] = true
			udpToMACMap[addrStr] = macFromPkt
			macToUDPMap[macFromPkt] = addrStr

			// Now we have a valid MAC
			macStr = macFromPkt
		} else {
			log.Printf("Invalid MAC address format in sensor data: %s from %s", macFromPkt, addrStr)
			return
		}
	}

	// Get device and update last seen timestamp
	device, exists := deviceMap[macStr]
	if !exists {
		log.Printf("Inconsistent state: MAC in mapping but not in deviceMap: %s", macStr)
		return
	}

	// Update last seen timestamp
	device.LastSeen = currentTimestamp()

	// Bygg upp SensorData med alla fält
	data := SensorData{
		GyroX:       pkt.GyroX,
		GyroY:       pkt.GyroY,
		GyroZ:       pkt.GyroZ,
		AccelX:      pkt.AccelX,
		AccelY:      pkt.AccelY,
		AccelZ:      pkt.AccelZ,
		JoystickX:   pkt.JoystickX,
		JoystickY:   pkt.JoystickY,
		ButtonState: pkt.ButtonState != 0, // konvertera uint8 → bool
		MacAddr:     macStr,
		Color:       device.Color,
		// BatteryLevel lämnas utelämnad tills vi börjar skicka den
	}

	// Skicka vidare som JSON till alla WebSocket-klienter
	BroadcastSensorData(data)
}
