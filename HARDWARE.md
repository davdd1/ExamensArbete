# Hardware Setup Guide

This document provides detailed hardware assembly instructions, wiring diagrams, and component specifications for the IoT Wireless Controller.

---

## Bill of Materials (BOM)

| Component | Specification | Quantity | Notes |
|-----------|--------------|----------|-------|
| ESP32-S3 DevKit | Dual-core, built-in WiFi | 1 | Development board with USB-C |
| MPU6050 | 6-axis IMU (I2C) | 1 | Accelerometer + Gyroscope |
| Analog Joystick | 2-axis potentiometer | 1 | Standard thumbstick module |
| LiPo Battery | 3.7V, 1200mAh+ | 1 | JST connector recommended |
| Breadboard | Full-size or half-size | 1 | For prototyping |
| Jumper Wires | Male-to-Male, Male-to-Female | ~15 | Various lengths |
| USB-C Cable | For programming/charging | 1 | Data-capable cable |
| (Optional) LED Strip | WS2812B or similar | 1 | Visual feedback |
| (Optional) Switch | Power switch | 1 | For battery disconnect |

**Estimated Cost:** ~$20-30 USD (excluding battery charger and tools)

---

## Pin Connections

### ESP32-S3 Pinout

Below is the pin assignment for connecting peripherals to the ESP32-S3:

#### MPU6050 (I2C)
```
MPU6050 Pin  →  ESP32-S3 Pin
VCC          →  3.3V
GND          →  GND
SCL          →  GPIO 22 (I2C Clock)
SDA          →  GPIO 21 (I2C Data)
```

#### Analog Joystick
```
Joystick Pin  →  ESP32-S3 Pin
VCC           →  3.3V
GND           →  GND
VRX (X-axis)  →  GPIO 1 (ADC1_CH0)
VRY (Y-axis)  →  GPIO 2 (ADC1_CH1)
SW (Button)   →  GPIO 3 (Optional, digital input)
```

#### LiPo Battery
```
Battery  →  ESP32-S3 DevKit
Positive →  BAT+ (or JST connector on board)
Negative →  BAT- (or GND)
```

*Note: Most ESP32-S3 DevKits have a built-in LiPo charging circuit. Connect the battery to the JST connector or designated battery pins.*

#### Optional: LED Strip (WS2812B)
```
LED Strip Pin  →  ESP32-S3 Pin
VCC (5V)       →  5V or USB VBUS
GND            →  GND
DIN (Data In)  →  GPIO 8 (or configured pin)
```

---

## Wiring Diagram

```
                    ┌─────────────────────────┐
                    │     ESP32-S3 DevKit     │
                    │                         │
                    │  ┌───────────────────┐  │
                    │  │    Processor      │  │
   LiPo Battery ────┼──┤ BAT+          3.3V├──┼──── MPU6050 VCC
   (3.7V 1200mAh)   │  │ BAT-           GND├──┼──── MPU6050 GND
                    │  │              GPIO22├──┼──── MPU6050 SCL (I2C Clock)
                    │  │              GPIO21├──┼──── MPU6050 SDA (I2C Data)
                    │  │                    │  │
                    │  │               GPIO1├──┼──── Joystick VRX (X-axis)
                    │  │               GPIO2├──┼──── Joystick VRY (Y-axis)
                    │  │               GPIO3├──┼──── Joystick SW (Button, optional)
   USB-C Cable ─────┼──┤ USB             5V ├──┼──── Joystick VCC (via 3.3V or 5V)
   (Programming)    │  │                    │  │     Joystick GND
                    │  │              GPIO8 ├──┼──── LED Strip DIN (optional)
                    │  └───────────────────┘  │
                    └─────────────────────────┘

Common Ground (GND) connected to:
- MPU6050 GND
- Joystick GND
- LED Strip GND (if used)
- Battery GND (via DevKit)
```

---

## Assembly Instructions

### Step 1: Prepare Components

1. Gather all components listed in the BOM
2. Ensure the ESP32-S3 DevKit is functional (test with USB connection)
3. Verify the MPU6050 and joystick modules are intact

### Step 2: Connect MPU6050

1. Connect **VCC** on MPU6050 to **3.3V** on ESP32-S3
2. Connect **GND** on MPU6050 to **GND** on ESP32-S3
3. Connect **SCL** on MPU6050 to **GPIO 22** on ESP32-S3
4. Connect **SDA** on MPU6050 to **GPIO 21** on ESP32-S3

**Note:** Some MPU6050 modules require pull-up resistors (typically 4.7kΩ) on SDA and SCL lines. Many modules have these built-in; check your module's documentation.

### Step 3: Connect Analog Joystick

1. Connect **VCC** on joystick to **3.3V** on ESP32-S3
2. Connect **GND** on joystick to **GND** on ESP32-S3
3. Connect **VRX** (X-axis output) to **GPIO 1** on ESP32-S3
4. Connect **VRY** (Y-axis output) to **GPIO 2** on ESP32-S3
5. (Optional) Connect **SW** (button) to **GPIO 3** with a pull-up resistor

### Step 4: Connect Battery

1. If using a LiPo battery:
   - Connect battery to the JST connector on the ESP32-S3 DevKit
   - Or connect **BAT+** to the battery positive and **BAT-/GND** to battery negative
2. Ensure correct polarity (reversed polarity can damage components)
3. (Optional) Add a power switch between battery and DevKit for easy on/off control

### Step 5: (Optional) Connect LED Strip

1. Connect **VCC** on LED strip to **5V** or **USB VBUS** on ESP32-S3
2. Connect **GND** on LED strip to **GND** on ESP32-S3
3. Connect **DIN** (data input) on LED strip to **GPIO 8** on ESP32-S3
4. Note: LED strips can draw significant current; ensure your power supply can handle it

### Step 6: Double-Check Connections

1. Verify all ground connections are common
2. Ensure no short circuits between power and ground
3. Confirm correct GPIO pin assignments match firmware configuration
4. Check for loose wires or unstable breadboard connections

---

## Power Considerations

### Battery Life

- **1200mAh LiPo**: ~3-4 hours of active use (typical)
- **Deep Sleep Mode**: Can extend to days/weeks (not implemented by default)
- **Charging**: Most ESP32-S3 DevKits have built-in USB charging (4.2V cutoff)

### Power Consumption

| Component | Typical Current | Notes |
|-----------|----------------|-------|
| ESP32-S3 (WiFi active) | ~80-160mA | Varies with TX power |
| MPU6050 | ~3.5mA | Sleep mode: 10µA |
| Analog Joystick | ~1-2mA | Very low power |
| LED Strip (optional) | ~20mA/LED | Can be significant |

**Total (without LEDs):** ~85-165mA

**Estimated Battery Life:**  
1200mAh / 125mA (avg) ≈ 9.6 hours (theoretical)  
Real-world: ~3-4 hours (accounting for inefficiencies and higher peak usage)

---

## Troubleshooting Hardware Issues

### MPU6050 Not Detected

1. **Check I2C connections**: Ensure SCL and SDA are correct
2. **I2C address**: Default is 0x68; check with I2C scanner
3. **Pull-up resistors**: May be required on SDA/SCL (4.7kΩ to 3.3V)
4. **Power supply**: Verify 3.3V is stable and sufficient

### Joystick Not Responding

1. **Check ADC pins**: Ensure VRX/VRY are connected to correct GPIO
2. **Power supply**: Verify joystick has 3.3V/5V and GND
3. **Calibration**: Joystick may need software calibration for center position
4. **ADC configuration**: Ensure ADC is initialized in firmware

### Battery Not Charging

1. **USB connection**: Ensure USB cable supports data (not charge-only)
2. **Charger IC**: Check if DevKit has built-in charging circuit
3. **Battery polarity**: Verify correct positive/negative connections
4. **Temperature**: LiPo chargers may not charge if battery is too cold/hot

### Intermittent Connections

1. **Breadboard**: Use quality breadboard; check for worn contacts
2. **Wires**: Replace loose or damaged jumper wires
3. **Solder connections**: Consider soldering for permanent installation
4. **Vibration**: Secure components to prevent movement

---

## Safety Warnings

⚠️ **Important Safety Information**

1. **LiPo Battery Handling:**
   - Never over-discharge (below 3.0V per cell)
   - Never overcharge (above 4.2V per cell)
   - Store at room temperature, away from flammable materials
   - Do not puncture, crush, or short-circuit
   - Use proper LiPo charging equipment

2. **Electrical Safety:**
   - Always disconnect power before modifying connections
   - Check polarity before connecting batteries
   - Avoid short circuits
   - Use appropriate current ratings for all components

3. **Heat Dissipation:**
   - ESP32 can get warm during heavy WiFi usage
   - Ensure adequate ventilation
   - Do not enclose in non-ventilated containers

---

## Future Hardware Improvements

### Custom PCB

A custom PCB design could:
- Reduce size and weight
- Improve reliability (no loose wires)
- Add dedicated charging circuitry
- Include power switch, status LEDs
- Integrate all components in a compact form factor

### Enclosure

A 3D-printed case could:
- Protect components from damage
- Improve ergonomics and handling
- House battery securely
- Provide mounting points for joystick and buttons

### Advanced Sensors

Consider upgrading to:
- **ICM-20948**: 9-axis IMU (magnetometer included)
- **BNO085**: Sensor fusion with on-chip processing
- **Additional inputs**: Buttons, triggers, D-pad

---

## Resources

- **ESP32-S3 Pinout**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/
- **MPU6050 Datasheet**: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
- **LiPo Battery Safety**: https://www.batteryuniversity.com/

---

**Authors:**
- Jesper Morais
- David Stenman

**Hardware Advisor:**
- Tobias Forsén, Creekside AB

---

*Last updated: May 2025*
