#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include <esp_task.h>
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "handle_sensor.h"
#include "handle_wifi.h"
#include "handle_udp.h"
#include "global_params.h"
/*
ESP WORK FLOW:
1. Starta wifi -> connecta till UDP Server -> VÄnta på svar ( Få ID ); Efter vi fått ID betyder det att vi nu är anslutna till servern eller är med i spelet
2. Starta sensor -> skicka data till UDP Server i format
struct SensorPacket {
    uint8_t type;  // 1 = Sensor data / 0 för en ny connection
    uint32_t player_id;
    float gyro_x;
    float gyro_y;
    float gyro_z;
};
*/

void app_main() {

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    global_task_params.event_handle = xEventGroupCreate();

    // Run wifi FIRST to ensure that the sensor task can connect to the internet
    handle_wifi(&global_task_params);

    //task that will handle the sensor
    ESP_LOGI("main", "GOING INTO SENSOR TASK");
    xTaskCreate(handle_sensor_task, "sensor_task", 4096, NULL, 5, NULL);

    ESP_LOGI("main", "GOING INTO UDP TASK");
    xTaskCreate(run_udp_task, "udp_task", 4096, &global_task_params, 5, NULL);


}
