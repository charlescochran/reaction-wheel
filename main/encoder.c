#include <math.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "bsp/esp_bsp_devkit.h"
#include "esp_err.h"
#include "esp_timer.h"

#include "encoder.h"

#define AS5600_I2C_ADDRESS     0x36

static const char *TAG = "encoder";

static esp_err_t as5600_read_register(uint8_t reg, uint8_t *data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (AS5600_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (AS5600_I2C_ADDRESS << 1) | I2C_MASTER_READ, true));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data, I2C_MASTER_NACK));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    esp_err_t ret = i2c_master_cmd_begin(BSP_I2C_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

static float encoder_get_angle() {
    uint8_t high_byte = 0, low_byte = 0;
    ESP_ERROR_CHECK(as5600_read_register(0x0C, &high_byte));
    ESP_ERROR_CHECK(as5600_read_register(0x0D, &low_byte));
    uint16_t angle = ((high_byte << 8) | low_byte) & 0x0FFF; // Mask for 12 bits
    return angle * 360.0 / 4096.0;
}

float encoder_update_angular_vel() {
    static int64_t prev_time_us = -1;
    int64_t time_us = esp_timer_get_time();
    if (prev_time_us < 0) {
	prev_time_us = time_us;
	return -1.0;
    }
    float delta_sec = (time_us - prev_time_us) / 1000000.0;
    prev_time_us = time_us;

    static float prev_angle;//, prev_delta_angle;
    float angle = encoder_get_angle();
    float delta_angle = angle - prev_angle;
 //    int sign;
 //    if (fabs(prev_delta_angle) > 30.0) {
	// sign = copysign(1, prev_delta_angle);
 //    } else {
 //    }
    while (abs(delta_angle > 360.0)) {
	delta_angle -= copysign(360.0, delta_angle);
    }
	//
 //    prev_delta_angle = delta_angle;
    prev_angle = angle;
    return delta_angle / delta_sec;
}
