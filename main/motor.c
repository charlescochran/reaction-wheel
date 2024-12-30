#include "bsp/esp_bsp_devkit.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "hal/ledc_types.h"
#include "iot_servo.h"

#include "motor.h"

static const int SERVO_GPIO = 32;
static const char *TAG = "motor";

void motor_init() {
    servo_config_t servo_cfg = {
        .max_angle = 360,
        .min_width_us = 1000,
        .max_width_us = 2000,
        .freq = 50,
        .timer_number = LEDC_TIMER_0,
        .channels = {
            .servo_pin = {
                SERVO_GPIO,
            },
            .ch = {
                LEDC_CHANNEL_0,
            },
	},
        .channel_number = 1,
    };

    iot_servo_init(LEDC_HIGH_SPEED_MODE, &servo_cfg);
    vTaskDelay(pdMS_TO_TICKS(500));
    iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
    for (int i = 5; i > 0; i--) {
	ESP_LOGI(TAG, "turn on in %d", i);
	vTaskDelay(pdMS_TO_TICKS(1000));
    }
    for (int angle = 0; angle <= 360; angle += 10) {
	ESP_ERROR_CHECK(iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, 0, angle));
	vTaskDelay(pdMS_TO_TICKS(20));  // Delay to stabilize signal
    }
    vTaskDelay(pdMS_TO_TICKS(8000));

    // iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 180);
 //    while (1) {
	// vTaskDelay(pdMS_TO_TICKS(1000));
 //    }
 //    vTaskDelay(pdMS_TO_TICKS(2000));
 //    iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
 //    vTaskDelay(pdMS_TO_TICKS(3000));
    while (1) {
	iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 200);
	vTaskDelay(pdMS_TO_TICKS(5000));
	iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 260);
	vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
