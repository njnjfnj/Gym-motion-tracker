#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h"

#include "BMI160_task/BMI160_task.h"

//static const char *TAG = "GYM_TRACKER";

void nfc_reader_task() {
    
}

void app_main(void) 
{
    ESP_ERROR_CHECK(i2cdev_init());

    xTaskCreate(bmi160_task, "bmi160_task", 4096, NULL, 5, &xTaskHandlePtr);
}