#include "handle_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "global_params.h"

static const char* TAG = "wifi";
static int reconnect_counter = 0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    task_params_t* param = (task_params_t*)arg;
    switch (event_id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WiFi started");
        esp_wifi_connect();
        break;

    case WIFI_EVENT_STA_CONNECTED:
        xEventGroupSetBits(param->event_handle, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Connected to wifi, set bit0");
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "Disconnected from AP, reconnecting...");

        if (reconnect_counter < WIFI_RECONNECT_MAX_ATTEMPT) {
            reconnect_counter++;
            esp_wifi_connect();
        }
        break;

    default:
        break;
    }
}

static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    task_params_t* param = (task_params_t*)arg;
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
        init_time();
        xEventGroupSetBits(param->event_handle, IP_RECEIVED_BIT);
        ESP_LOGI(TAG, "GOT IP, set bit1");
        break;

    default:
        break;
    }
}

void init_wifi(task_params_t* param)
{
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, param, NULL));
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, param, NULL));

    wifi_config_t wifi_config = { 0 };

    const char* ssid = CONFIG_WIFI_SSID;
    const char* password = CONFIG_WIFI_PASSWORD;

    strlcpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strlcpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi started");
}