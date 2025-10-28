# Godot Client

This directory contains the Godot Engine client application for visualizing and interacting with the IoT wireless controller in real-time.

---

## Overview

The Godot client connects to the Go server via WebSocket and receives real-time sensor data (joystick position, IMU orientation) from connected ESP32 controllers. The data is used to control game elements and provide visual feedback.

---

## Prerequisites

- **Godot Engine 4.x**: Download from https://godotengine.org/download
- **Go Server**: Must be running and accessible (see `/server/README.md`)
- **ESP32 Controller**: Firmware flashed and connected to network (see `/embedded/README.md`)

---

## Getting Started

### 1. Install Godot

Download and install Godot Engine 4.x from the official website:
https://godotengine.org/download

### 2. Open the Project

1. Launch Godot Engine
2. Click "Import" on the project manager
3. Navigate to this `godot` directory
4. Select the `project.godot` file
5. Click "Import & Edit"

### 3. Configure Server Connection

The WebSocket connection settings can be configured in the main scene or relevant script files:

- **Default Server Address**: `ws://localhost:8080/ws`
- **Modify in**: Check `main.gd` or connection setup scripts

If your Go server is running on a different host or port, update the WebSocket URL accordingly.

### 4. Run the Project

1. In Godot, press **F5** or click the "Play" button (▶) to run the project
2. The client will attempt to connect to the WebSocket server
3. Once connected, move your controller and observe the real-time response

---

## Project Structure

- **Scenes/**: Contains game scenes and UI layouts
- **assets/**: Graphics, sprites, and other visual assets
- **sound/**: Audio files for feedback and effects
- **addons/**: Third-party Godot plugins (e.g., godot-git-plugin)
- **Scripts**:
  - `main.gd`: Main scene controller
  - `ball.gd`, `paddle.gd`, `player.gd`: Game element controllers
  - `control.gd`: UI and input handling
  - Additional `.gd` files for specific functionality

---

## Features

- **Real-Time Visualization**: See controller input in real-time
- **Multi-Controller Support**: Handle multiple connected controllers with color coding
- **Game Demos**: Interactive demonstrations using sensor data
- **WebSocket Integration**: Efficient bi-directional communication

---

## Configuration

### WebSocket Settings

Edit the relevant `.gd` script files to change:
- Server URL
- Connection timeout
- Reconnection logic
- Data parsing options

### Visual Customization

Modify scenes and scripts to:
- Change visual styles
- Add new game mechanics
- Implement custom sensor data handling
- Adjust responsiveness and smoothing

---

## Troubleshooting

### Cannot Connect to Server

1. Verify the Go server is running: `docker-compose logs -f` (see `/server/README.md`)
2. Check the WebSocket URL in the Godot scripts
3. Ensure no firewall is blocking port 8080
4. Verify network connectivity between client and server

### No Controller Data Received

1. Confirm ESP32 is powered on and connected to Wi-Fi
2. Check ESP32 serial output for connection status
3. Verify Go server is receiving UDP packets (check server logs)
4. Ensure the WebSocket connection is active in Godot

### Laggy or Delayed Response

1. Check network latency between devices
2. Reduce distance between ESP32 and Wi-Fi router
3. Ensure the Go server is not overloaded
4. Verify no other processes are consuming network bandwidth

---

## Development

### Adding New Features

1. Create new scenes or scripts in Godot
2. Extend existing game logic in `.gd` files
3. Parse additional sensor data from WebSocket JSON
4. Test changes by running the project in Godot

### Debugging

- Use Godot's built-in debugger (F6 or Debug menu)
- Check the output console for errors and warnings
- Add `print()` statements in GDScript for debugging
- Monitor WebSocket messages in browser developer tools (if applicable)

---

## Performance Tips

- **Reduce Update Frequency**: If performance is an issue, throttle WebSocket updates
- **Optimize Scripts**: Profile GDScript performance using Godot's profiler
- **Asset Optimization**: Compress textures and audio files

---

## Export & Deployment

To create a standalone executable:

1. Go to **Project > Export**
2. Select your target platform (Windows, Linux, macOS, etc.)
3. Configure export settings
4. Click "Export Project"

Refer to Godot's official documentation for detailed export instructions:
https://docs.godotengine.org/en/stable/tutorials/export/

---

## Contributing

Contributions to improve the Godot client are welcome! Please:

- Follow GDScript style conventions
- Test changes thoroughly before submitting
- Document new features or changes in comments
- See `/CONTRIBUTING.md` for general guidelines

---

## License

This project is licensed under the MIT License. See `/LICENSE` for details.

---

## Resources

- **Godot Documentation**: https://docs.godotengine.org/
- **GDScript Reference**: https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/
- **WebSocket in Godot**: https://docs.godotengine.org/en/stable/classes/class_websocketclient.html

---

**Authors:**
- Jesper Morais
- David Stenman

**Last updated:** May 2025
