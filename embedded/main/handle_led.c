#include "handle_led.h"

#define LED_STRIP_GPIO_NUM GPIO_NUM_8
#define LED_STRIP_LED_AMOUNT 1
#define LED_STRIP_RMT_RES_HZ 10000000 // 10MHz resolution

static led_strip_handle_t led_strip;
static TaskHandle_t rainbow_task_handle = NULL;

static void led_rainbow_task(void* pvParameters) {
    uint8_t hue = 0;
    while (1) {
        uint8_t r, g, b;

        // Simple HSV to RGB conversion
        uint8_t region = hue / 43;
        uint8_t remainder = (hue - (region * 43)) * 6;

        uint8_t p = 0;
        uint8_t q = 255 - remainder;
        uint8_t t = remainder;

        switch (region) {
        case 0:
            r = 255;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = 255;
            b = p;
            break;
        case 2:
            r = p;
            g = 255;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = 255;
            break;
        case 4:
            r = t;
            g = p;
            b = 255;
            break;
        default:
            r = 255;
            g = p;
            b = q;
            break;
        }

        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, r, g, b));
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));

        // Increment hue for rainbow effect
        hue = (hue + 1) % 256;

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void stop_rainbow_task(void) {
    if (rainbow_task_handle != NULL) {
        vTaskDelete(rainbow_task_handle);
        rainbow_task_handle = NULL;
    }
}

void init_led() {
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_NUM,
        .max_leds = LED_STRIP_LED_AMOUNT,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    set_led_off(); // Turn off the LED strip initially
}

void set_led_rainbow() {
    // If rainbow is already running, do nothing
    if (rainbow_task_handle != NULL) {
        return;
    }

    xTaskCreate(led_rainbow_task, "led_rainbow_task", 2048, NULL, 2, &rainbow_task_handle);
}

void set_led_color(uint8_t r, uint8_t g, uint8_t b) {
    stop_rainbow_task(); // Stop rainbow effect before setting a static color
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, r, g, b));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

void set_led_off() {
    stop_rainbow_task();
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 0, 0, 0));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

void set_led_red() { set_led_color(40, 0, 0); }

void set_led_green() { set_led_color(0, 40, 0); }

void set_led_blue() { set_led_color(0, 0, 40); }