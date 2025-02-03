#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include <esp_task.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "handle_sensor.h"
#include "handle_wifi.h"
#include "esp_netif.h"

//BIT0 WIFI SET
//BIT1 GOT IP


void app_main() {

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    task_params_t params;
    params.event_handle = xEventGroupCreate();

    handle_wifi(&params);
    // xTaskCreate(handle_sensor_task, "sensor_task", 4096, NULL, 5, NULL);

    ESP_LOGI("main", "Waiting for wifi to connect");
    xEventGroupWaitBits(params.event_handle, BIT0, pdTRUE, pdTRUE, portMAX_DELAY);
    ESP_LOGI("main", "Connected to wifi, finished waiting");


    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
