#include "handle_udp.h"      
#include "global_params.h"    

#include <stdio.h>           
#include <stdlib.h>          
#include <string.h>           

#include <arpa/inet.h>        
#include <sys/socket.h>      
#include <netinet/in.h>       
#include <sys/param.h>        

#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"      
#include "freertos/event_groups.h"  

#include "esp_log.h"         
#include "esp_err.h"         
#include "esp_netif.h"       
#include "lwip/sockets.h"   
#include "lwip/netdb.h"      
#include "lwip/err.h"        


#define UDP_PORT CONFIG_SERVER_PORT
#define UDP_SERVER_IP CONFIG_SERVER_IP

void run_udp_task(void* params) {
    ESP_LOGI("UDP", "Starting UDP task...");
    task_params_t *task_params = (task_params_t*) params;

    ESP_LOGI("UDP", "Waiting for WiFi to connect...");
    // Vänta tills WiFi är anslutet
    if(params == NULL) {
        ESP_LOGE("UDP", "Task parameters are NULL");
        vTaskDelete(NULL);
        return;
    }
    xEventGroupWaitBits(task_params->event_handle, BIT0 | BIT1 , pdFALSE, pdTRUE, portMAX_DELAY);

    char rx_buffer[128];  // Buffer för mottagen data
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

    while (1) {
        sensor_payload_t global_sensor_packet;
        xQueueReceive(task_params->sensor_data_queue, &global_sensor_packet, portMAX_DELAY);
        

        int err = sendto(sock, &global_sensor_packet, sizeof(global_sensor_packet), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
            break;
        }

        ESP_LOGI("UDP", "Message sent");

        vTaskDelay(pdMS_TO_TICKS(100)); // Vänta 1 sekund innan nästa skickning
    }

    if (sock != -1) {
        ESP_LOGE("UDP", "Shutting down socket...");
        close(sock);
    }
    
    vTaskDelete(NULL);
}
