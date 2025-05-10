package main

import (
	"log"
	"time"
)

// currentTimestamp returns the current Unix timestamp in seconds
func currentTimestamp() int64 {
	return time.Now().Unix()
}

// cleanupStaleDevices removes devices that haven't been seen for a while
func cleanupStaleDevices() {
	now := currentTimestamp()

	deviceMapMu.Lock()
	defer deviceMapMu.Unlock()

	for mac, device := range deviceMap {
		if now-device.LastSeen > deviceTimeout {
			log.Printf("Removing stale device: %s (last seen %d seconds ago)", mac, now-device.LastSeen)

			// Remove all UDP address mappings for this device
			for udpAddr := range device.UDPAddresses {
				delete(udpToMACMap, udpAddr)
			}

			// Remove the MAC mapping
			delete(macToUDPMap, mac)
			delete(deviceMap, mac)
		}
	}
}
