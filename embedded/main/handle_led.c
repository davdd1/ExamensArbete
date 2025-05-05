#include "handle_led.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3
#define LED_STRIP_GPIO GPIO_NUM_33
#define LED_STRIP_POWER_GPIO GPIO_NUM_21
#endif

#ifdef CONFIG_IDF_TARGET_ESP32C6
#define LED_STRIP_GPIO GPIO_NUM_8
#endif

#define LED_STRIP_LED_AMOUNT 1
#define LED_STRIP_RMT_RES_HZ 10000000 // 10MHz resolution

static led_strip_handle_t led_strip;
static TaskHandle_t rainbow_task_handle = NULL;

static void led_rainbow_task(void* pvParameters)
{
    uint16_t hue = 0;
    while (true) {
        ESP_ERROR_CHECK(led_strip_set_pixel_hsv(
            led_strip,
            0,
            (uint32_t)(hue * 360 / 255),
            255,
            255));
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        hue = (hue + 1) % 256;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// TODO: FIX THIS, ITS NOT WORKING, can't find the handle to the task
static void stop_rainbow_task(void)
{   
    printf("Stopping rainbow task\n");
    if (rainbow_task_handle != NULL) {
        vTaskDelete(rainbow_task_handle);
        rainbow_task_handle = NULL;
        printf("Rainbow task stopped\n");
    }else{
        printf("Rainbow task was not running or could't find handle\n"); // ALWAYS HERE
    }
}

void init_led()
{
#ifdef CONFIG_IDF_TARGET_ESP32S3
    // Set NEOPIXEL power pin high for RGB LED
    gpio_reset_pin(LED_STRIP_POWER_GPIO);
    gpio_set_direction(LED_STRIP_POWER_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_STRIP_POWER_GPIO, 1);
#endif

    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO,
        .max_leds = LED_STRIP_LED_AMOUNT,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
        .mem_block_symbols = 64,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    set_led_off(); // Turn off the LED strip initially
}

void set_led_rainbow()
{
    printf("Starting rainbow task\n");
    // If rainbow is already running, do nothing
    if (rainbow_task_handle != NULL) {
        return;
    }

    xTaskCreate(led_rainbow_task, "led_rainbow_task", 2048, NULL, 2, &rainbow_task_handle);
}

void set_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    printf("Setting LED color: R=%d, G=%d, B=%d\n", r, g, b);
    //stop_rainbow_task(); // Stop rainbow effect before setting a static color
    //set_led_off();
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, r, g, b));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

void set_led_off()
{
    //stop_rainbow_task();
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 0, 0, 0));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

void set_led_red() { set_led_color(70, 0, 0); }

void set_led_green() { set_led_color(0, 70, 0); }

void set_led_blue() { set_led_color(0, 0, 70); }