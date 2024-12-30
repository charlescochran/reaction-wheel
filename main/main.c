#include <math.h>

#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "bsp/esp_bsp_devkit.h"

#include "imu.h"
#include "encoder.h"
#include "motor.h"

static const char *TAG = "main";
// static const float P = 1;
// static const float I = 1;
// static const float D = 1;

void update(void *pvParameters) {
 //    static int64_t prev_time_us = -1;
 //    int64_t time_us = esp_timer_get_time();
 //    if (prev_time_us < 0) {
	// prev_time_us = time_us;
	// return;
 //    }
 //    int64_t delta_us = time_us - prev_time_us;
 //    prev_time_us = time_us;
	//
 //    float disp = imu_update_displacement();
 //    static int64_t prev_encoder_update_time;
 //    if (time_us - prev_encoder_update_time > 100 * 1000) {
	// prev_encoder_update_time = time_us;
	// float angular_vel = encoder_update_angular_vel();
	// static int cnt;
	// if (cnt++ % 5 == 0) {
	//     ESP_LOGI(TAG, "Encoder angular vel: %.3f", angular_vel);
	//     ESP_LOGI(TAG, "Imu displacement: %.3f", disp);
	// }
 //    }

 //    static float prev_error, integral;
 //    float error = imu_get_displacement();
 //    integral += error * delta_us;
 //    float target_alpha = P * error + I * integral + D * (error - prev_error) / delta_us;
}

void app_main(void)
{
 //    bsp_i2c_init();
 //    imu_init();
 //    const esp_timer_create_args_t timer_cfg = {
	// .callback = update,
	// .arg = NULL, // arg to pass to callback
	// .name = "Update timer",
	// .skip_unhandled_events = true,
 //    };
 //    esp_timer_handle_t timer = NULL;
 //    esp_timer_create(&timer_cfg, &timer);
 //    esp_timer_start_periodic(timer, 5000); // 5ms
    motor_init();
}
