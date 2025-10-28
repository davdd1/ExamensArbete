# References & Technical Background

This document provides the technical sources and references used in the development of the IoT Wireless Controller project.

---

## Hardware Components

### ESP32-S3
- **ESP32-S3 Technical Reference Manual**: https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf
- **ESP32-S3 Datasheet**: https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- **ESP-IDF Programming Guide**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/

### MPU6050 IMU Sensor
- **MPU-6050 Product Specification**: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
- **MPU-6050 Register Map**: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
- **I2C Communication Protocol**: NXP I2C-bus specification and user manual

### Analog Joystick
- **ADC in ESP32**: ESP-IDF ADC API documentation
- **Analog Joystick Basics**: Standard 2-axis potentiometer-based joystick modules

---

## Wireless Communication

### Wi-Fi & UDP
- **UDP Protocol (RFC 768)**: https://www.rfc-editor.org/rfc/rfc768
- **Wi-Fi 802.11 Standards**: IEEE 802.11 specification
- **ESP32 Wi-Fi Driver**: ESP-IDF Wi-Fi API documentation
- **Why UDP for Real-Time Applications**: Low-latency, connectionless protocol suitable for time-sensitive data

### WebSocket
- **WebSocket Protocol (RFC 6455)**: https://www.rfc-editor.org/rfc/rfc6455
- **Gorilla WebSocket**: https://github.com/gorilla/websocket
- **WebSocket vs. HTTP Polling**: Comparison of real-time communication methods

---

## Software & Tools

### ESP-IDF (Espressif IoT Development Framework)
- **ESP-IDF Documentation**: https://docs.espressif.com/projects/esp-idf/en/latest/
- **FreeRTOS**: https://www.freertos.org/
- **ESP-IDF Build System**: CMake-based build system documentation

### Go Programming
- **Go Language Specification**: https://go.dev/ref/spec
- **Go Concurrency (Goroutines)**: https://go.dev/doc/effective_go#goroutines
- **JSON Encoding in Go**: https://pkg.go.dev/encoding/json
- **Go WebSocket Library (Gorilla)**: https://pkg.go.dev/github.com/gorilla/websocket

### Godot Engine
- **Godot Engine Documentation**: https://docs.godotengine.org/
- **WebSocket Client in Godot**: https://docs.godotengine.org/en/stable/classes/class_websocketclient.html
- **GDScript Reference**: https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/

---

## Networking & Latency

### Real-Time Systems
- **Real-Time Embedded Systems**: Liu, J. W. S. (2000). Real-Time Systems. Prentice Hall.
- **Latency in Wireless Networks**: IEEE papers on Wi-Fi latency and jitter
- **NTP (Network Time Protocol)**: https://www.ntp.org/

### Performance Optimization
- **Reducing Network Latency**: Techniques for minimizing end-to-end delay
- **Packet Loss Handling**: Forward error correction and redundancy strategies
- **Jitter Buffering**: Sliding window averaging and timestamp-based smoothing

---

## Power Management

### Battery & Energy Efficiency
- **LiPo Battery Basics**: Understanding lithium polymer battery characteristics
- **ESP32 Power Management**: Deep sleep modes and power consumption optimization
- **Battery Charging Circuits**: TP4056 and similar LiPo charging ICs

---

## Development & Testing

### Version Control
- **Git Documentation**: https://git-scm.com/doc
- **GitHub Flow**: https://guides.github.com/introduction/flow/

### Docker
- **Docker Documentation**: https://docs.docker.com/
- **Docker Compose**: https://docs.docker.com/compose/

### Testing Methodologies
- **Unit Testing in Go**: https://pkg.go.dev/testing
- **Integration Testing**: End-to-end testing of embedded-server-client pipeline
- **Performance Benchmarking**: Latency, throughput, and stability testing

---

## Related Projects & Inspiration

- **Open-Source Game Controllers**: Various DIY controller projects on GitHub
- **IoT Sensor Networks**: Research on low-latency sensor data transmission
- **Real-Time Web Applications**: WebSocket-based real-time communication patterns

---

## Academic & Technical Papers

- **Wireless Sensor Networks for Real-Time Applications**: Various IEEE papers
- **Low-Latency Communication in IoT**: Studies on UDP vs. TCP performance
- **IMU Sensor Fusion**: Complementary filtering and Kalman filtering techniques

---

## Future Reading & Improvements

### Advanced IMU
- **ICM-20948**: 9-axis motion tracking (accelerometer, gyroscope, magnetometer)
- **BNO085**: Sensor fusion with on-chip processing

### Security
- **TLS/SSL**: Transport Layer Security for encrypted communication
- **OAuth2**: Authentication and authorization for multi-user systems

### PCB Design
- **KiCad Documentation**: https://www.kicad.org/
- **PCB Design Best Practices**: Guidelines for layout and routing

---

## Contact & Contributions

For questions about specific references or to suggest additional resources, please open an issue or submit a pull request.

**Authors:**
- Jesper Morais
- David Stenman

**Advisor:**
- Tobias Forsén, Creekside AB

---

*Last updated: May 2025*
