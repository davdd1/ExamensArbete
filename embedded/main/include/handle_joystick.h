#pragma once

#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "global_params.h"

typedef struct {
    float x;    // -1.0 <-> +1.0 === Right positive
    float y;    // -1.0 <-> +1.0 === Up positive
    bool isPressed;
} joystick_state_t;

