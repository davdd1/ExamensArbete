#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"  
#include "driver/i2c.h"
#include <esp_log.h>

const char* TAG = "MAIN";

//funktion för att initiera I2C master
void i2c_master_init();

void app_main(void)
{
    i2c_master_init();
}

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 6,  // GPIO6 (LP_I2C_SDA)
        .scl_io_num = 7,  // GPIO7 (LP_I2C_SCL)
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000  // 100 kHz standard I2C
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));

    ESP_LOGI(TAG, "I2C driver installed");
}
