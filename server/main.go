package main

import (
	"fmt"
	"net"
)

const (
	port = "1234"
)

func main() {	

	udpAddr, err := net.ResolveUDPAddr("udp", port)

	if(err != nil) {
		fmt.Println("Error: ", err)
		return
	}

	conn, err := net.ListenUDP("udp", udpAddr)
	
	if(err != nil) {
		fmt.Println("Error: ", err)
		return
	}
	for {
		var buf [512]byte
		_, addr, err := conn.ReadFromUDP(buf[0:])
		if err != nil {
			fmt.Println(err)
			return
		}

		fmt.Print("> ", string(buf[0:]))

		// Write back the message over UPD
		conn.WriteToUDP([]byte("Hello UDP Client\n"), addr)
	}

}
