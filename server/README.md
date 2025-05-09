# Sensor Server Docker Instructions

This project provides a server that handles sensor data from ESP devices over UDP and broadcasts it to clients via WebSockets.

## Docker Setup

### Building and Running with Docker

To build and run this containerized application, you need Docker installed and running on your system.

### Prerequisites

1. Install Docker Desktop for Windows: https://www.docker.com/products/docker-desktop/
2. Make sure Docker Desktop is running

### Using Docker Compose (recommended)


Bring the service down (if running), rebuild the image, and start it in detached mode in Docker Desktop — all in one go:

```bash
docker-compose down; docker-compose up --build -d
```

## Get Logs

```bash
docker-compose logs -f
```

## Ports
 
- **8080/tcp**: WebSocket server
- **1234/udp**: UDP server for sensor data

## Notes

- The container exposes both TCP (WebSocket) and UDP ports
- Sensor data is received via UDP and forwarded to WebSocket clients
- The server assigns colors to connected devices
