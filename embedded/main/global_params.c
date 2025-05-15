#include "global_params.h"
#include <stdio.h>
#include "esp_sntp.h"
#include "time.h"

task_params_t global_task_params;

packet_t global_packet;

void init_sntp(void){
    // Sätt upp rätt tidszon (exempel CET/CEST)
    setenv("TZ", "Europe/Stockholm", 1);
    tzset();
    
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    // Vänta på sync (max ~10 sekunder)
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void init_time(void) {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    
    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    
    while (timeinfo.tm_year < (2020 - 1900) && ++retry < 15) {
        printf("Waiting for system time to be set... (%d/15)\n", retry);
        vTaskDelay(pdMS_TO_TICKS(1000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}