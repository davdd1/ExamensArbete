#include "driver/i2c.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "global_params.h"
#include "handle_led.h"
#include "handle_sensor.h"
#include "handle_udp.h"
#include "handle_wifi.h"
#include "nvs_flash.h"
#include <esp_task.h>
#include <stdio.h>

/*
TODO: Skriva workflow för hela systemet
*/

void app_main()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    global_task_params.event_handle = xEventGroupCreate();
    global_task_params.sensor_data_queue = xQueueCreate(1, sizeof(packet_t));
    
    init_wifi(&global_task_params);
    init_led();
    
    if (xTaskCreate(handle_sensor_task, "sensor_task", SENSOR_TASK_STACK_SIZE, &global_task_params, 5, NULL) != pdPASS) {
        ESP_LOGE("TASK", "Failed to create sensor task");
        vTaskDelete(NULL);
        return;
    }
    
    xEventGroupWaitBits(global_task_params.event_handle, BIT0 | BIT1, pdFALSE, pdTRUE, portMAX_DELAY);
    //Sätter tidszon

    if (xTaskCreate(run_udp_task, "udp_task", UDP_TASK_STACK_SIZE, &global_task_params, 5, NULL) != pdPASS) {
        ESP_LOGE("TASK", "Failed to create UDP task");
        vTaskDelete(NULL);
        return;
    }

    // set_led_rainbow();

    //set_led_green();
}
