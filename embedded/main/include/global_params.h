#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include <stdio.h>

#define SENSOR_TASK_STACK_SIZE 4096
#define UDP_TASK_STACK_SIZE 4096

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
} packet_type_e;

// Unified packet structure
typedef struct Packet {
    packet_type_e type; // First field is the packet type
    uint8_t mac_addr[6];
    uint8_t _padding1[2]; // 2 bytes padding → 4-byte alignment
    struct { // Sensor data
        // Gyroskop
        float gyro_x, gyro_y, gyro_z;
        
        // Accelerometer
        float accel_x, accel_y, accel_z;
        
        // Joystick
        float joy_x, joy_y;
        uint8_t joy_is_pressed; // 0 = not pressed, 1 = pressed
        uint8_t _padding2[3]; // 3 bytes padding at the end → 4-byte alignment
    } sensor;
} packet_t; // Total 48 bytes

// Declare an external instance of the unified packet
extern packet_t global_packet;