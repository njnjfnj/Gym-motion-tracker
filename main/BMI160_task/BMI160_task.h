#ifndef BMI160_task
#define BMI160_task

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h"
#include <bmi160.h>

#define MY_SCL 4
#define MY_SDA 3
#define MY_INT1_GPIO 1

extern TaskHandle_t xTaskHandlePtr;

typedef struct {
    float accX, accY, accZ;
    float gyroX, gyroY, gyroZ;
} sensor_frame_t;

//void IRAM_ATTR isr_new_data(void* arg);
void bmi160_task(void *pvParameters);
void bmi160_task_start();


#endif