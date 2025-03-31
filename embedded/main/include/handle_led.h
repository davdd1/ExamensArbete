#pragma once

#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void init_led(void);
void set_led_rainbow();
void set_led_color(uint8_t r, uint8_t g, uint8_t b);
void set_led_off();
void set_led_red();
void set_led_green();
void set_led_blue();

