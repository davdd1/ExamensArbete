# IoT Wireless Controller – Real-Time Open-Source IoT Gamepad

**From sensor to user interface: Design, implementation, and testing of an open-source, real-time wireless IoT controller**  
*Examensarbete IoT23, Jesper Morais & David Stenman, Maj 2025*

---

## Abstract

This project details the design and implementation of a low-cost, open-source wireless IoT controller for responsive real-time applications.  
Our solution provides a highly customizable and accessible platform for hobbyists and developers seeking new interaction methods.

- **ESP32-S3-based controller:** Analog joystick & MPU6050 (IMU)
- **Wi-Fi/UDP transmission:** Minimal latency, robust for real-time use
- **Go backend:** Data aggregation, JSON, WebSocket relay
- **Godot client:** Real-time visualization/game demo
- **Performance:** Sub-30ms latency, 3+ hours battery, ~20m range, <1% packet loss

---

## Table of Contents

- [Background & Motivation](#background--motivation)
- [Goals](#goals)
- [System Overview](#system-overview)
- [Hardware](#hardware)
- [Software Architecture](#software-architecture)
- [Theory](#theory)
- [Results & Evaluation](#results--evaluation)
- [Limitations & Improvements](#limitations--improvements)
- [How to Run](#how-to-run)
- [Authors & Credits](#authors--credits)
- [References](#references)

---

## Background & Motivation

Most modern wireless game controllers (like Xbox controllers) are proprietary and expensive, which limits hobbyists and custom projects.  
Our aim: Create an open, low-cost, real-time controller platform, fully hackable and adaptable for research, prototyping, or fun interactive applications.

---

## Goals

- **End-to-end latency:** < 30 ms
- **Battery life:** ≥ 3 hours active use
- **Range:** ≥ 20 meters (indoors)
- **Packet loss:** < 1% at normal traffic
- **Integration:** Fully open source, JSON over WebSocket

---

## System Overview

**System diagram:**

[ESP32-S3 + Joystick + MPU6050] --(UDP/WiFi)--> [Go Server] --(WebSocket)--> [Godot Client]

- ESP32 samples sensors, sends minimal UDP packets (every ~10ms)
- Go server decodes packets, assigns MAC/color, broadcasts JSON via WebSocket
- Godot visualizes and interacts with sensor data in real-time

---

## Hardware

- **ESP32-S3 DevKit:** Dual-core, built-in WiFi, battery charging
- **MPU6050:** Accelerometer + gyroscope, I2C
- **Analog joystick:** 2-axis, read via ADC
- **LiPo battery:** 1200mAh tested
- **Prototype built on breadboard** (custom PCB is future work)

---

## Software Architecture

**ESP32 firmware:**  
- Written in C with ESP-IDF + FreeRTOS  
- I2C for IMU, ADC for joystick, UDP for networking

**Go server:**  
- `udp_recv.go`: Decodes UDP packets, handles MAC/color
- `udp_sender.go`: Broadcasts sensor data to WebSocket clients
- `websocket.go`: Manages WebSocket connections
- `main.go`: Server entrypoint, data structures

**Godot client:**  
- Receives WebSocket JSON, controls game/visual elements

---

## Theory

- **Why Wi-Fi/UDP?**  
  - Lower latency than BLE, better suited for continuous motion data  
  - UDP = no handshake, lower overhead, tolerates occasional packet loss (OK for real-time input)

- **Why Go server?**  
  - Goroutines for concurrency, simple JSON encoding, robust WebSocket handling

- **Why Godot?**  
  - Open source, rapid prototyping, built-in WebSocket support

- **Latency handling:**  
  - Timestamps/NTP sync, sliding window averaging in client
  - Jitter measured and minimized in software

---

## Results & Evaluation

- **Latency:** 9–35ms (typ 15–25ms), meets sub-30ms target
- **Range:** Stable up to 20m indoors (varies with WiFi)
- **Packet loss:** <1% in all tests
- **Battery:** >3h with 1200mAh LiPo, potential for more with sleep modes
- **Stability:** 2+ hour stress-tested without crash

**Table: System Verification**
| Feature                 | Status | Note                                  |
|-------------------------|--------|---------------------------------------|
| UDP communication       | ✅     | Stable, low-latency                   |
| WebSocket communication | ✅     | Robust, multi-client                  |
| MPU6050 handling        | ✅     | Consistent, some noise (calibrated)   |
| Battery life (3h)       | ✅*    | Tested, could optimize further        |
| Range (20m)             | ✅     | Stable in normal environments         |
| Packet loss (<1%)       | ✅     | No user-noticeable loss in use        |

---

## Limitations & Improvements

- **Prototype:** On breadboard; no custom PCB or enclosure (yet)
- **Energy saving:** Basic battery handling, no deep sleep implemented
- **BLE:** Not used (WiFi better for real-time, see theory)
- **Network:** Local server demo; plug-and-play networking not included
- **Security:** No encryption/auth (use trusted network for now)
- **Future:**  
  - Custom PCB + 3D-printed case  
  - Better IMU (ICM-20948/BNO085)  
  - TLS/secure networking  
  - Sleep modes and power switch  
  - Market research for productization

---

## How to Run

### Hardware

1. Assemble ESP32-S3 with MPU6050 (I2C) and joystick (ADC)
2. Power with LiPo battery

### Software

**ESP32:**  
- Flash with provided firmware

**Go Server:**  
```sh
go get github.com/gorilla/websocket
go run .
```

**Godot Client:**
- Open project in Godot, run


### Authors & Credits
- Jesper Morais: Godot UI, Go backend, embedded development
- David Stenman: Real-time comms, system design, CI/CD
- Hardware advisor: Tobias Forsén, Creekside AB

### References
- See [file] for full sources and technical background.


*This project is open source for research, education, and prototyping. Pull requests and forks welcome!*
