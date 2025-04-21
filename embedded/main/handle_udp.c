#include "handle_udp.h"
#include "global_params.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/socket.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "handle_led.h"

// 1. Skicka connection request till servern
// 2. få någon typ av ID tuillbaka
// 3. Skicka sensordata till servern
// 4. På en timer, skicka även ACK till servern att vi fortfarande är uppkopplade (behövs för att veta om vi
// disconnectat eller inte)

#define UDP_PORT CONFIG_SERVER_PORT
#define UDP_SERVER_IP CONFIG_SERVER_IP

void run_udp_task(void* params) {
    ESP_LOGI("UDP", "Starting UDP task...");
    task_params_t* task_params = (task_params_t*)params;

    ESP_LOGI("UDP", "Waiting for WiFi to connect...");
    // Vänta tills WiFi är anslutet
    if (params == NULL) {
        ESP_LOGE("UDP", "Task parameters are NULL");
        vTaskDelete(NULL);
        return;
    }
    xEventGroupWaitBits(task_params->event_handle, BIT0 | BIT1, pdFALSE, pdTRUE, portMAX_DELAY);

    char rx_buffer[128]; // Buffer för mottagen data
    char host_ip[] = UDP_SERVER_IP;
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_UDP;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(atoi(UDP_PORT));

    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE("UDP", "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI("UDP", "Socket created, sending to %s:%s", host_ip, UDP_PORT);

    packet_t connection_packet;
    connection_packet.type = TYPE_CONNECTION_REQUEST;
    ESP_ERROR_CHECK(esp_read_mac(connection_packet.mac_addr, ESP_MAC_WIFI_STA));

    int err = sendto(sock, &connection_packet, sizeof(connection_packet), 0, (struct sockaddr*)&dest_addr,
                     sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    // Vänta på ACK från servern
    err = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    if (err < 0) {
        ESP_LOGE("UDP", "Error occurred during receiving: errno %d", errno);
        vTaskDelete(NULL);
        return;
    } else {
        // byte 0 är ack
        // byte 1 är färgindex 0 = RED, 1 = GREEN, 2 = BLUE
        rx_buffer[err] = '\0';
        switch (rx_buffer[1]) {
        case 0:
            printf("color is RED\n");
            set_led_red();
            break;
        case 1:
            printf("color is GREEN\n");
            set_led_green();
            break;
        case 2:
            printf("color is BLUE\n");
            set_led_blue();
            break;
        default:
            printf("color is UNKNOWN\n");
            break;
        }
    }

    while (1) {
        packet_t global_sensor_packet;
        xQueueReceive(task_params->sensor_data_queue, &global_sensor_packet, portMAX_DELAY);

        int err = sendto(sock, &global_sensor_packet, sizeof(global_sensor_packet), 0,
                         (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (sock != -1) {
        ESP_LOGE("UDP", "Shutting down socket...");
        close(sock);
    }

    vTaskDelete(NULL);
}

// TODO: Edgecase: Internet dör / tappar connection. Vad händer då?