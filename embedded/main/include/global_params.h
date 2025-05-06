#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include <stdio.h>

#define WIFI_CONNECTED_BIT BIT0
#define IP_RECEIVED_BIT BIT1

typedef struct task_params {
    EventGroupHandle_t event_handle;
    QueueHandle_t sensor_data_queue;
    EventGroupHandle_t udp_event_handle;
} task_params_t;

// Declare an external instance of task_params_t
extern task_params_t global_task_params;

// Packet types
typedef enum packet_type { 
    TYPE_CONNECTION_REQUEST = 0, 
    TYPE_SENSOR_DATA = 1 
} packet_type_t;

// Unified packet structure
typedef struct Packet {
    packet_type_t type; // First field is the packet type
    union {
        struct { // Connection request data
            uint8_t mac_addr[6];
        };
        struct { // Sensor data
            uint32_t player_id;
            float gyro_x;
            float gyro_y;
            float gyro_z;
        };
    };
} packet_t;

// Declare an external instance of the unified packet
extern packet_t global_packet;