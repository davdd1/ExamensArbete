#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_RECONNECT_MAX_ATTEMPT 50
#define GOT_WIFI_BIT BIT0
#define GOT_IP_BIT BIT1


typedef struct task_params{
    //GOT WIFI BIT0
    //GOT IP BIT1
    EventGroupHandle_t event_handle;
} task_params_t;

void handle_wifi(task_params_t* params);