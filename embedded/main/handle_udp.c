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

void run_udp_task(void* params)
{
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
    ESP_LOGI("UDP", "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", connection_packet.mac_addr[0],
        connection_packet.mac_addr[1], connection_packet.mac_addr[2], connection_packet.mac_addr[3],
        connection_packet.mac_addr[4], connection_packet.mac_addr[5]);
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
            // set_led_red();
            break;
        case 1:
            printf("color is GREEN\n");
            // set_led_green();
            break;
        case 2:
            printf("color is BLUE\n");
            // set_led_blue();
            break;
        default:
            printf("color is UNKNOWN\n");
            break;
        }    }


    //premade sensorpacket with mac address
    packet_t sensor_packet;
    memcpy(sensor_packet.mac_addr, connection_packet.mac_addr, sizeof(connection_packet.mac_addr));

    while (1) {
        // Receive only the sensor data portion into our pre-configured packet
        xQueueReceive(task_params->sensor_data_queue, &sensor_packet, sizeof(sensor_packet));
        // Mock data
        //  global_sensor_packet.player_id = 2;
        //  global_sensor_packet.gyro_x = 4.23;
        //  global_sensor_packet.gyro_y = 5.23;
        //  global_sensor_packet.gyro_z = 6.23;
        // printf("Sending sensor data: Joystick_x=%.2f, Joystick_y=%.2f\n", sensor_packet.sensor.joy_x,
        //      sensor_packet.sensor.joy_y);

        int err = sendto(sock, &sensor_packet, sizeof(sensor_packet), 0,
            (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Wait for 50ms before sending the next packet
        // No delay needed since we're already waiting on the queue
        // Sending again as soon as new sensor data is available
    }

    if (sock != -1) {
        ESP_LOGE("UDP", "Shutting down socket...");
        close(sock);
    }

    vTaskDelete(NULL);
}

// TODO: Edgecase: Internet dör / tappar connection. Vad händer då?