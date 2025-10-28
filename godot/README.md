# Godot Client

Godot Engine client for visualizing controller data in real-time.

## Requirements

- Godot Engine 4.x (download from https://godotengine.org/download)
- Go server running (see `/server/README.md`)
- ESP32 controller connected (see `/embedded/README.md`)

## Setup

1. Install Godot Engine 4.x
2. Open Godot and click "Import"
3. Select `project.godot` from this directory
4. Click "Import & Edit"

## Configuration

Default WebSocket server: `ws://localhost:8080/ws`

To change the server address, edit the connection URL in `main.gd` or relevant scripts.

## Running

Press **F5** in Godot or click the Play button (▶).

The client will connect to the WebSocket server and display real-time sensor data from your controller.

## Project Files

- `Scenes/` - Game scenes and UI
- `assets/` - Graphics and sprites
- `sound/` - Audio files
- Scripts: `main.gd`, `ball.gd`, `paddle.gd`, `player.gd`, etc.

## Troubleshooting

**Can't connect to server:**
- Make sure the Go server is running
- Check the WebSocket URL in your scripts
- Verify no firewall is blocking port 8080

**No controller data:**
- Confirm ESP32 is on and connected to WiFi
- Check ESP32 serial output
- Verify server is receiving UDP packets

**Laggy:**
- Check network latency
- Move ESP32 closer to WiFi router
- Close other bandwidth-heavy apps

## Export

To create a standalone app:
1. Go to **Project > Export**
2. Select your platform (Windows/Linux/macOS)
3. Configure settings
4. Click "Export Project"

See https://docs.godotengine.org/en/stable/tutorials/export/ for details.

## Resources

- Godot docs: https://docs.godotengine.org/
- GDScript: https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/
- WebSocket: https://docs.godotengine.org/en/stable/classes/class_websocketclient.html
