#include "handle_udp.h"
#include "global_params.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <endian.h>
#include <inttypes.h>

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

#define ACK_PACKET_LEN 10
static int64_t time_offset_ms = 0;

#define UDP_PORT CONFIG_SERVER_PORT
#define UDP_SERVER_IP CONFIG_SERVER_IP

/**
 * @brief Runs the UDP communication task for sensor data transmission and time synchronization.
 *
 * Establishes a UDP connection to a configured server, sends a connection request, and waits for an acknowledgment containing a server timestamp. Calculates the time offset between the server and local device. Continuously retrieves sensor data from a FreeRTOS queue, synchronizes its timestamp with the server, and sends it to the server via UDP. Handles send errors with retries and cleans up resources on failure or task termination.
 */
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

    //char rx_buffer[128]; // Buffer för mottagen data
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
     // Vänta på ACK, PARSA ut server-tiden och beräkna offset
    uint8_t ack_buf[ACK_PACKET_LEN];
    int len = recv(sock, ack_buf, ACK_PACKET_LEN, 0);
    if (len == ACK_PACKET_LEN && ack_buf[0] == 1) {
        uint8_t player_index = ack_buf[1];
        uint64_t server_ms = be64toh(*(uint64_t*)(ack_buf + 2));
        struct timeval tv_;
        gettimeofday(&tv_, NULL);
        uint64_t local_ms = tv_.tv_sec*1000ULL + tv_.tv_usec/1000ULL;
        time_offset_ms = (int64_t)server_ms - (int64_t)local_ms;
        // ESP_LOGI("TIME", "Cristian offset = %lld ms, player=%d",
        //          (long long)time_offset_ms, player_index);
        // lys upp LED baserat på index som du hade tidigare…
    } else {
        // ESP_LOGE("UDP", "Felaktigt ACK-paket (%d bytes)", len);
        vTaskDelete(NULL);
        return;
    }
    // Vänta på ACK från servern
    // err = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    // if (err < 0) {
    //     ESP_LOGE("UDP", "Error occurred during receiving: errno %d", errno);
    //     vTaskDelete(NULL);
    //     return;
    // } else {
    //     // byte 0 är ack
    //     // byte 1 är färgindex 0 = RED, 1 = GREEN, 2 = BLUE
    //     rx_buffer[err] = '\0';
    //     switch (rx_buffer[1]) {
    //     case 0:
    //         printf("color is RED\n");
    //         // set_led_red();
    //         break;
    //     case 1:
    //         printf("color is GREEN\n");
    //         // set_led_green();
    //         break;
    //     case 2:
    //         printf("color is BLUE\n");
    //         // set_led_blue();
    //         break;
    //     default:
    //         printf("color is UNKNOWN\n");
    //         break;
    //     }
    // }

    // premade sensorpacket with mac address
    packet_t sensor_packet;
    //memcpy(sensor_packet.mac_addr, connection_packet.mac_addr, sizeof(connection_packet.mac_addr));

    while (1) {
        // ESP_LOGI("HEAP", "Free heap: %lu bytes", esp_get_free_heap_size());
        // UBaseType_t high_water_mark = uxTaskGetStackHighWaterMark(NULL);

        // Clear the sensor_packet buffer before receiving new data
        memset(&sensor_packet.sensor, 0, sizeof(sensor_packet.sensor));
        xQueueReceive(task_params->sensor_data_queue, &sensor_packet, portMAX_DELAY);
        memcpy(sensor_packet.mac_addr, connection_packet.mac_addr, sizeof(connection_packet.mac_addr));

        // Get the current time
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64_t local_ms = tv.tv_sec*1000ULL + tv.tv_usec/1000ULL;
        uint64_t synced_ms = local_ms + time_offset_ms;
        //printf("LOCAL TIME: tv_sec=%lld, tv_usec=%ld → %llu ms\n", (long long)tv.tv_sec, (long)tv.tv_usec, synced_ms);
        
        
        // Set the timestamp in the packet
        //printf("ESP LOCAL TIME: tv_sec=%lld, tv_usec=%ld → %llu ms\n", (long long)tv.tv_sec, (long)tv.tv_usec, ms);
        sensor_packet.timestamp = htobe64(synced_ms);
        //printf("UDP: Sending packet with timestamp: %" PRIu64 " ms\n", ms);
        //printf("UDP: Sending packet with timestamp: %" PRIu64 " ms\n", ms);
        // Mock data
        //  global_sensor_packet.player_id = 2;
        //  global_sensor_packet.gyro_x = 4.23;
        //  global_sensor_packet.gyro_y = 5.23;
        //  global_sensor_packet.gyro_z = 6.23;
        // printf("Sending sensor data: Joystick_x=%.2f, Joystick_y=%.2f\n", sensor_packet.sensor.joy_x,
        //       sensor_packet.sensor.joy_y);

        int err = sendto(sock, &sensor_packet, sizeof(sensor_packet), 0,
            (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
            vTaskDelay(pdMS_TO_TICKS(100));
            err = sendto(sock, &sensor_packet, sizeof(sensor_packet), 0,
                (struct sockaddr*)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
                vTaskDelay(pdMS_TO_TICKS(500));
                err = sendto(sock, &sensor_packet, sizeof(sensor_packet), 0,
                    (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                if (err < 0) {
                    ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
                    break;
                }else {
                    ESP_LOGI("UDP", "Sensor data resent successfully");
                }
            }else {
                ESP_LOGI("UDP", "Sensor data resent successfully");
            }
        }
    }

    if (sock != -1) {
        ESP_LOGE("UDP", "Shutting down socket...");
        close(sock);
    }

    vTaskDelete(NULL);
}

// TODO: Edgecase: Internet dör / tappar connection. Vad händer då?