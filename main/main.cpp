#include "esp_idf_version.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "bsp/esp_bsp_devkit.h"

static const char *TAG = "imu";

class Main {

void update(void *pvParameters) {
}

};  // Main

extern "C" void app_main()
{
  Main main = Main();
  const esp_timer_create_args_t timer_cfg = {
      .callback = main.update,
      .arg = NULL, // arg to pass to callback
      .name = "Update timer",
      .skip_unhandled_events = true,
  };
  esp_timer_handle_t timer = NULL;
  esp_timer_create(&timer_cfg, &timer);
  esp_timer_start_periodic(timer, 5000); // 5ms
}
