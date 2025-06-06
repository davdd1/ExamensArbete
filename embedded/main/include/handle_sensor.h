#pragma once

#include <stdio.h>

#define I2C_MASTER_SDA_IO 5 // GPIO5 (LP_I2C_SDA)
#define I2C_MASTER_SCL_IO 6 // GPIO6 (LP_I2C_SCL)
#define I2C_MASTER_FREQ_HZ 100000 // 100 kHz standard I2C
#define I2C_MASTER_NUM I2C_NUM_0 // Använd I2C-buss 0

#define MPU6050_ADDR 0x68 // Standardadressen
#define PWR_MGMT_1 0x6B // Strömhanteringsregister
#define ACCEL_XOUT_H 0x3B // Accelerometerdata börjar här
#define ACCEL_CONFIG 0x1C // Accelerometerkonfiguration

// FOR ESP32-S3: Verify ADC channel configuration for GPIO17 (ADC_CHANNEL_6) and GPIO18 (ADC_CHANNEL_7).
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define VRX_CH ADC_CHANNEL_7 // GPIO18
#define VRY_CH ADC_CHANNEL_6 // GPIO17
#define ADC2_WIDTH ADC_WIDTH_BIT_12
#define ADC2_ATTEN ADC_ATTEN_DB_11 // 0–3.3V range
#endif

#ifdef CONFIG_IDF_TARGET_ESP32C6
#define VRX_CH ADC1_CHANNEL_4 // GPIO3
#define VRY_CH ADC1_CHANNEL_2 // GPIO4
#endif

// Joystick center / deadzone
#define JOY_CENTER 2048
#define JOY_DEADZONE 200

// Button
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define SW_GPIO GPIO_NUM_16
#endif

#ifdef CONFIG_IDF_TARGET_ESP32C6
#define SW_GPIO GPIO_NUM_10
#endif

void write_register(uint8_t reg, uint8_t value);
void read_mpu6050_data(uint8_t start_reg, int16_t* data);
void i2c_master_init();
void handle_sensor_task(void* params);