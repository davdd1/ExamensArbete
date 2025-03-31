# 🎮 IoT Motion Controller  
### Jesper Morais & David Stenman

## Overview

This project explores the use of motion-controlled input using ESP32-based hardware in a scalable, wireless, and low-latency environment. Each device operates as a standalone motion controller using its onboard IMU (e.g., MPU6050), capable of tracking tilt, orientation, and acceleration.

Sensor data is transmitted via UDP to a **headless Go server**, which is responsible for interpreting input and broadcasting it to connected WebSocket clients. This modular design ensures that the server can be easily integrated into different applications and frontends – from games to industrial interfaces.

In the current implementation, a Godot-based UI visualizes the motion of each controller as a unique, colored on-screen entity (“blob”), enabling multiple users to interact in real time.

---

## 🧩 Architecture

### ESP32 Controller (Client)
- Collects motion data (accelerometer and gyroscope)
- Sends periodic updates via UDP
- Identified uniquely via MAC address

### Go Server (Headless, Modular)
- Listens for connection requests and sensor data
- Assigns a unique color index to each client
- Forwards processed motion data to WebSocket clients
- Designed to be frontend-agnostic

### Godot Client (Example Frontend)
- Receives WebSocket updates
- Renders and animates real-time motion of each player
- Associates blobs with MAC address and assigned color

---

## 💡 Use Cases

- Multiplayer mini-games controlled through motion input  
- Interactive installations where physical gestures control on-screen elements  
- Educational tools for demonstrating motion sensing and real-time networking  
- Custom IoT dashboards for device orientation feedback  
- Prototyping gesture-based interfaces for smart environments  

---

## 🎮 Interaction Design (Current Demo Capabilities)

The current demo implementation visualizes each controller as a moving blob in 2D space. While simple in presentation, it lays the foundation for more advanced interaction mechanics:

- **Tilt Control**: Smooth movement across screen based on device inclination  
- **Rotation Input**: Gyroscope data available for rotation-based gestures  
- **Impulse Recognition**: High acceleration along Z-axis can be interpreted as actions like jump, dash, or selection  
- **Modular Input Pipeline**: Easily extended to gesture recognition, custom thresholds, or contextual triggers  

---

This modular system highlights the potential of motion input in lightweight multiplayer experiences and embedded systems — all without the need for tethered hardware or high-cost sensors.
