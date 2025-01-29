#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_SDA_IO 6  // GPIO6 (LP_I2C_SDA)
#define I2C_MASTER_SCL_IO 7  // GPIO7 (LP_I2C_SCL)
#define I2C_MASTER_FREQ_HZ 100000  // 100 kHz standard I2C
#define I2C_MASTER_NUM I2C_NUM_0  // Använd I2C-buss 0

#define MPU6050_ADDR 0x68  // Standardadressen
#define PWR_MGMT_1 0x6B  // Strömhanteringsregister
#define ACCEL_XOUT_H 0x3B  // Accelerometerdata börjar här

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

    printf("✅ I2C driver installerad!\n");
}

// 📌 Skriver till ett register
void write_register(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

// 📌 Läser 6 byte från MPU6050 (Accelerometer eller Gyroskop)
void read_mpu6050_data(uint8_t start_reg, int16_t *data) {
    uint8_t raw_data[6];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, start_reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, raw_data, 6, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // Konvertera från två bytes till en 16-bitars signed integer
    for (int i = 0; i < 3; i++) {
        data[i] = (int16_t)((raw_data[i * 2] << 8) | raw_data[i * 2 + 1]);
    }
}

void app_main() {
    i2c_master_init();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Aktivera MPU6050 genom att skriva 0x00 till PWR_MGMT_1 (default är sleep mode)
    write_register(PWR_MGMT_1, 0x00);
    printf("🔋 MPU6050 aktiverad!\n");

    int16_t accel[3], gyro[3];

    while (1) {
        // Läs accelerometerdata
        read_mpu6050_data(ACCEL_XOUT_H, accel);
        float ax = accel[0] / 16384.0;  // Skala till g-krafter
        float ay = accel[1] / 16384.0;
        float az = accel[2] / 16384.0;

        // Läs gyroskopdata
        read_mpu6050_data(0x43, gyro);
        float gx = gyro[0] / 131.0;  // Skala till grader per sekund
        float gy = gyro[1] / 131.0;
        float gz = gyro[2] / 131.0;

        printf("📊 Accelerometer: X=%.2fg, Y=%.2fg, Z=%.2fg\n", ax, ay, az);
        printf("🎛️ Gyroskop: X=%.2f°/s, Y=%.2f°/s, Z=%.2f°/s\n", gx, gy, gz);
        printf("\n");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Vänta 500 ms
    }
}
