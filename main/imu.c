#include <math.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "mpu6050.h"
#include "bsp/esp_bsp_devkit.h"
#include "esp_timer.h"

#include "imu.h"

#define RAD_TO_DEG                  57.27272727f

static const char *TAG = "imu";

static complimentary_angle_t complimentary_angle;
static mpu6050_handle_t mpu6050_dev;
static mpu6050_acce_value_t acce;
static mpu6050_gyro_value_t gyro;
static mpu6050_gyro_value_t gyro_biases;
float displacement;

static void change_axes() {
    // Account for IMU being mounted vertically, making new z-axis point up
    float tmp = acce.acce_y;
    acce.acce_y = -acce.acce_z;
    acce.acce_z = tmp;
    tmp = gyro.gyro_y;
    gyro.gyro_y = -gyro.gyro_z;
    gyro.gyro_z = tmp;
}

static void apply_gyro_biases() {
    gyro.gyro_x += gyro_biases.gyro_x;
    gyro.gyro_y += gyro_biases.gyro_y;
    gyro.gyro_z += gyro_biases.gyro_z;
}

// void mpu6050_read(void *pvParameters)
// {
//     static int cnt;
//     mpu6050_get_acce(mpu6050_dev, &acce);
//     mpu6050_get_gyro(mpu6050_dev, &gyro);
//     change_axes();
//     mpu6050_complimentory_filter(mpu6050_dev, &acce, &gyro, &complimentary_angle);
//     if (cnt++ % 50 == 0) {
//       ESP_LOGI(TAG, "acce_x:%.2f, acce_y:%.2f, acce_z:%.2f", acce.acce_x, acce.acce_y, acce.acce_z);
//       ESP_LOGI(TAG, "gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
//       ESP_LOGI(TAG, "roll: %.2f, pitch: %.2f\n", complimentary_angle.roll, complimentary_angle.pitch);
//     }
// }

static void get_avg_acce_and_gyro(int ms,
				  mpu6050_acce_value_t *avg_acce,
				  mpu6050_gyro_value_t *avg_gyro) {
    int cnt;
    for (cnt = 0; cnt < ms/10; cnt++) {
	mpu6050_get_acce(mpu6050_dev, &acce);
	mpu6050_get_gyro(mpu6050_dev, &gyro);
	change_axes();
	avg_acce->acce_x += acce.acce_x;
	avg_acce->acce_y += acce.acce_y;
	avg_acce->acce_z += acce.acce_z;
	avg_gyro->gyro_x += gyro.gyro_x;
	avg_gyro->gyro_y += gyro.gyro_y;
	avg_gyro->gyro_z += gyro.gyro_z;
	vTaskDelay(pdMS_TO_TICKS(10));
    }
    avg_acce->acce_x /= cnt;
    avg_acce->acce_y /= cnt;
    avg_acce->acce_z /= cnt;
    avg_gyro->gyro_x /= cnt;
    avg_gyro->gyro_y /= cnt;
    avg_gyro->gyro_z /= cnt;
}

// static float initialize() {
//     mpu6050_acce_value_t avg_acce;
//     mpu6050_gyro_value_t avg_gyro;
//     get_avg_acce_and_gyro(2000, &avg_acce, &avg_gyro);
// }

void imu_init()
{
    mpu6050_dev = mpu6050_create(BSP_I2C_NUM, MPU6050_I2C_ADDRESS);
    mpu6050_config(mpu6050_dev, ACCE_FS_4G, GYRO_FS_500DPS);
    mpu6050_wake_up(mpu6050_dev);
    mpu6050_acce_value_t avg_acce;
    mpu6050_gyro_value_t avg_gyro;
    get_avg_acce_and_gyro(10000, &avg_acce, &avg_gyro);
    displacement = atan2(-acce.acce_x, acce.acce_z) * RAD_TO_DEG;
    gyro_biases.gyro_x = -avg_gyro.gyro_x;
    gyro_biases.gyro_y = -avg_gyro.gyro_y;
    gyro_biases.gyro_z = -avg_gyro.gyro_z;
    // ESP_LOGI(TAG, "dispacement:%.3f, x bias: %.3f, y bias: %.3f, z bias: %.3f",
	   //   displacement, gyro_biases.gyro_x, gyro_biases.gyro_y, gyro_biases.gyro_z);
}

float imu_update_displacement() {
    static int64_t prev_time_us = -1;
    int64_t time_us = esp_timer_get_time();
    if (prev_time_us < 0) {
	prev_time_us = time_us;
	return -1;
    }
    float delta_sec = (time_us - prev_time_us) / 1000000.0;
    prev_time_us = time_us;

    mpu6050_get_gyro(mpu6050_dev, &gyro);
    change_axes();
    apply_gyro_biases();
    displacement += gyro.gyro_y * delta_sec;
    return displacement;
}
