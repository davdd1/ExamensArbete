# Hardware Setup

## Parts List

| Part | Spec | Notes |
|------|------|-------|
| ESP32-S3 DevKit | Dual-core, WiFi | With USB-C |
| MPU6050 | 6-axis IMU (I2C) | Accel + Gyro |
| Analog Joystick | 2-axis | Standard thumbstick |
| LiPo Battery | 3.7V, 1200mAh+ | JST connector |
| Breadboard | Any size | For prototyping |
| Jumper Wires | ~15 wires | M-M and M-F |
| USB-C Cable | Data capable | For programming |
| (Optional) LED Strip | WS2812B | For effects |

Cost: ~$20-30 USD

## Wiring

### MPU6050 (I2C)
```
MPU6050  →  ESP32-S3
VCC      →  3.3V
GND      →  GND
SCL      →  GPIO 22
SDA      →  GPIO 21
```

### Joystick
```
Joystick  →  ESP32-S3
VCC       →  3.3V
GND       →  GND
VRX       →  GPIO 1
VRY       →  GPIO 2
SW        →  GPIO 3 (optional)
```

### Battery
```
Battery   →  ESP32-S3
Positive  →  BAT+ (or JST)
Negative  →  GND
```

Most ESP32-S3 boards have built-in charging.

### LED Strip (Optional)
```
LED Strip  →  ESP32-S3
VCC        →  5V
GND        →  GND
DIN        →  GPIO 8
```

## Assembly

1. Put ESP32-S3 on breadboard
2. Connect MPU6050 to 3.3V, GND, GPIO 22 (SCL), GPIO 21 (SDA)
3. Connect joystick to 3.3V, GND, GPIO 1 (X), GPIO 2 (Y)
4. Plug battery into JST connector
5. Check all grounds are connected
6. Double-check no shorts between power and ground

Note: Some MPU6050 modules need 4.7kΩ pull-up resistors on SDA/SCL (many have them built-in).

## Power

**Battery Life:**
- 1200mAh LiPo: ~3-4 hours active use
- ESP32-S3 (WiFi on): 80-160mA
- MPU6050: ~3.5mA
- Joystick: ~1-2mA
- Total: ~85-165mA

## Troubleshooting

**MPU6050 not working:**
- Check I2C wiring (SCL/SDA)
- Default I2C address is 0x68
- May need pull-up resistors

**Joystick not responding:**
- Verify ADC pins (GPIO 1 and 2)
- Check power (3.3V)
- May need calibration in code

**Battery not charging:**
- Ensure USB cable supports data
- Check polarity
- LiPo chargers won't charge if battery is too hot/cold

**Loose connections:**
- Use quality breadboard
- Replace worn jumper wires
- Consider soldering for permanent setup

## Safety

⚠️ **LiPo Battery:**
- Don't over-discharge (below 3.0V)
- Don't overcharge (above 4.2V)
- Store at room temperature
- Don't puncture or short-circuit

## Future Upgrades

Ideas for later:
- Custom PCB design
- 3D-printed case
- Better IMU (ICM-20948 or BNO085)
- More buttons/triggers

## Resources

- ESP32-S3 docs: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/
- MPU6050 datasheet: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
