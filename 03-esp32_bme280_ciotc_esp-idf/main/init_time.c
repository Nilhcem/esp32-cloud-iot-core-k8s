#include <time.h>

#include "esp_log.h"
#include "lwip/apps/sntp.h"
#include "freertos/task.h"

static const char *TAG = "SNTP";

void init_sntp()
{
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "time.google.com");
  sntp_init();
}

void init_time()
{
  init_sntp();
  // wait for time to be set
  time_t now = 0;
  struct tm timeinfo = {0};
  while (timeinfo.tm_year < (2016 - 1900)) {
    ESP_LOGI(TAG, "Waiting for system time to be set...");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    time(&now);
    localtime_r(&now, &timeinfo);
  }
  ESP_LOGI(TAG, "Time is set...");
}

unsigned get_timestamp()
{
  return (unsigned)time(NULL);
}
