package main

import (
	"log"
	"net/http"

	"github.com/gorilla/websocket"
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

func healthHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusOK)
	w.Write([]byte("OK"))
}

func StartWebSocketServer() {
	http.HandleFunc("/ws", wsHandler)
	http.HandleFunc("/health", healthHandler)
	addr := ":8080"
	log.Println("Websocket-server startad på", addr)
	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal("Fel vid start av websocket-server:", err)
	}
}
