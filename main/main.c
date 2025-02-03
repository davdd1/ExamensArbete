#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include <esp_task.h>
#include "handle_sensor.h"

void app_main() {
    xTaskCreate(handle_sensor_task, "sensor_task", 4096, NULL, 5, NULL);
}
