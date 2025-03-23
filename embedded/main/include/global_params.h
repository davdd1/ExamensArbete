#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define GOT_WIFI_BIT BIT0
#define GOT_IP_BIT BIT1

typedef struct task_params {
    EventGroupHandle_t event_handle;
    QueueHandle_t sensor_data_queue;
    EventGroupHandle_t udp_event_handle;
} task_params_t;

// Declare an external instance of task_params_t
extern task_params_t global_task_params;

// ONLY FOR SENSOR DATA
typedef struct SensorPacket {
    uint8_t type;  // 1 = Sensor data, 0 = Connection request
    uint32_t player_id;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} sensor_payload_t;

// Declare an external instance of sensor_payload_t
extern sensor_payload_t global_sensor_packet;

// ONLY FOR CONNECTION REQUEST
typedef struct ConnectionPacket {
    uint8_t type;  // 0 = Connection request
    uint8_t mac_addr[6]; // MAC address of the ESP32
}ConnectionPacket_t;

// Declare an external instance of ConnectionPacket
extern ConnectionPacket_t global_connection_packet;