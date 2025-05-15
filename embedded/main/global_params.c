#include "global_params.h"
#include <stdio.h>
#include "esp_sntp.h"
#include "time.h"

task_params_t global_task_params;

packet_t global_packet;


void init_time(void) {
    // Sätt tidszon tidigt
    setenv("TZ", "Europe/Stockholm", 1);
    tzset();

    // Konfigurera SNTP
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    // Vänta tills vi har fått ett rimligt datum (>2020)
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0, retry_count = 15;
    while (timeinfo.tm_year < (2020 - 1900) && ++retry < retry_count) {
        printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
        vTaskDelay(pdMS_TO_TICKS(1000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year < (2020 - 1900)) {
        //printf("WARNING: NTP-sync misslyckades efter %d sekunder\n", retry_count);
    } else {
        // printf("NTP-synk klar: %04d-%02d-%02d %02d:%02d:%02d\n",
        //        timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
        //        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
    printf("Efter SNTP: %04d-%02d-%02d %02d:%02d:%02d (sync-status=%d)",
    timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday,
    timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
    sntp_get_sync_status());
}