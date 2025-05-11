#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "global_params.h"
#include <stdio.h>

#define WIFI_RECONNECT_MAX_ATTEMPT 50

void init_wifi(task_params_t* params);