# Changelog

## Recent Changes

### Documentation Added
- Added LICENSE, REFERENCES.md, CONTRIBUTING.md, HARDWARE.md, SECURITY.md
- Added godot/README.md and root .gitignore
- Updated README with proper links

## v1.0.0 (May 2025)

Initial release - thesis project by Jesper Morais & David Stenman, IoT23

### What's Working
- ESP32-S3 controller with MPU6050 IMU and analog joystick
- Wireless data transmission via UDP
- Go server with WebSocket support
- Godot client for visualization
- Latency: 9-35ms (usually 15-25ms)
- Range: 20+ meters indoors
- Battery: 3+ hours on 1200mAh LiPo

### Known Issues
- Breadboard prototype only (no PCB/enclosure)
- No encryption or authentication
- No deep sleep modes
- Local network only

### Future Ideas
- Custom PCB and 3D-printed case
- Better IMU sensor
- Add encryption
- Deep sleep for battery life
- OTA updates
