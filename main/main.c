#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"  
#include "driver/i2c.h"
#include <esp_log.h>

const char* TAG = "MAIN";

#define I2C_MASTER_NUM I2C_NUM_0

// MPU6050:s I2C-adress
#define MPU6050_ADDR 0x68 // Standard I2C address for MPU6050
#define WHO_AM_I_REG 0x75 //register som identiferar vem enheten är (skall retunera 0x68)


/*
Bitshift förklaring:
MPU6050:s adress (0x68) måste shifta en bit vänster för att matcha I2C-protokollets read/write-bit.

(0x68 << 1) | I2C_MASTER_WRITE = 0xD0 (för skrivning)
(0x68 << 1) | I2C_MASTER_READ = 0xD1 (för läsning)
*/
void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER, // säger att ESP32 är master i denna "relation"
        .sda_io_num = 6,  // GPIO6 (LP_I2C_SDA)
        .scl_io_num = 7,  // GPIO7 (LP_I2C_SCL)
        .sda_pullup_en = GPIO_PULLUP_ENABLE, //sätt igång pullup på SDA
        .scl_pullup_en = GPIO_PULLUP_ENABLE, //sätt igång pullup på SCL
        .master.clk_speed = 100000  // sätter l2c klockan till 100kHz   
    };

    // Konfigurerar I2C med inställningarna ovan
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));

    // Installerar I2C-drivrutinen
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));

    ESP_LOGI(TAG, "I2C driver installed");
}

uint8_t read_register(uint8_t reg_addr) {
    uint8_t data;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Skickar en skriv begäran till MPU6050
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    
    // Skickar en läs begäran till MPU6050 för att läsa data från reg_addr
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    // Skickar kommandot och väntar på svar
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    // Returnerar datan som lästs från reg_addr
    return data;
}

void app_main(void)
{
    i2c_master_init();
    vTaskDelay(pdMS_TO_TICKS(1000));
    uint8_t who_am_i = read_register(WHO_AM_I_REG);
    printf("WHO_AM_I register: 0x%X\n", who_am_i);
}

