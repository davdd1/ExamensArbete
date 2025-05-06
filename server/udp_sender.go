package main

import(
	"encoding/json"
	"fmt"
	"log"
	"github.com/gorilla/websocket"
)


func BroadcastSensorData(data SensorData) {
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