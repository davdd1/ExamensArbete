# Changelog

All notable changes to the IoT Wireless Controller project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- Comprehensive project documentation
  - LICENSE (MIT)
  - REFERENCES.md with technical sources
  - CONTRIBUTING.md with contribution guidelines
  - HARDWARE.md with wiring diagrams and assembly instructions
  - SECURITY.md with security policy and recommendations
  - godot/README.md for Godot client setup
  - Root .gitignore file
- Enhanced README.md with links to all documentation

### Changed
- Updated README.md references section to link to REFERENCES.md
- Improved "How to Run" section with links to component-specific READMEs

---

## [1.0.0] - 2025-05-XX (Initial Release)

### Project Overview
IoT Wireless Controller - Real-Time Open-Source IoT Gamepad

**Thesis Project:** Jesper Morais & David Stenman, IoT23, May 2025

### Features

#### Hardware
- ESP32-S3 based controller with Wi-Fi connectivity
- MPU6050 6-axis IMU (accelerometer + gyroscope)
- 2-axis analog joystick
- LiPo battery support (1200mAh tested)
- Breadboard prototype design

#### Firmware (ESP32)
- ESP-IDF v5.x based C firmware
- FreeRTOS task management
- I2C communication with MPU6050
- ADC reading for analog joystick
- UDP packet transmission over Wi-Fi
- Battery voltage monitoring
- (Optional) LED strip support for visual feedback

#### Server (Go)
- UDP server for receiving sensor data
- WebSocket server for client connections
- JSON data format
- Multi-device support with color assignment
- MAC address-based device identification
- Docker containerization support

#### Client (Godot)
- Godot Engine 4.x based visualization
- WebSocket client implementation
- Real-time sensor data rendering
- Interactive game demos
- Multi-controller support

### Performance Metrics
- **Latency:** 9-35ms (typically 15-25ms) - ✅ Meets <30ms target
- **Range:** 20+ meters indoors - ✅ Stable
- **Packet Loss:** <1% - ✅ Excellent reliability
- **Battery Life:** 3+ hours active use - ✅ Meets requirement
- **Stability:** 2+ hours stress-tested without crash

### Known Limitations
- Prototype stage (breadboard assembly)
- No custom PCB or enclosure
- No encryption or authentication
- Basic power management (no deep sleep)
- Wi-Fi only (BLE not implemented)
- Intended for local networks only

### Documentation
- Comprehensive README.md with system overview
- Component-specific README files (embedded, server)
- Hardware specifications and BOM
- Software architecture documentation

### Future Improvements
- Custom PCB design
- 3D-printed enclosure
- Advanced IMU (ICM-20948 or BNO085)
- TLS/SSL encryption
- Device authentication
- Deep sleep power modes
- OTA firmware updates
- Mobile app client

---

## Version History

### Versioning Strategy

- **Major version (X.0.0):** Significant architecture changes, breaking API changes
- **Minor version (1.X.0):** New features, enhancements, non-breaking changes
- **Patch version (1.0.X):** Bug fixes, security patches, minor updates

### Pre-Release Versions

Development versions may use tags like:
- `v1.0.0-alpha.1` - Alpha release
- `v1.0.0-beta.1` - Beta release
- `v1.0.0-rc.1` - Release candidate

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute to this project.

---

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) for details.

---

*This changelog will be updated with each significant change to the project.*
