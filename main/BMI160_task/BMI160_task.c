#include "BMI160_task.h"

const char *TAG = "BMI160";

TaskHandle_t xTaskHandlePtr = NULL;

static void IRAM_ATTR isr_new_data(void* arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(xTaskHandlePtr, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void bmi160_task(void *pvParameters) {
    ESP_LOGI(TAG, "CONFIGURING BMI160");
    bmi160_t bmi160_dev;
    memset(&bmi160_dev.i2c_dev, 0, sizeof(i2c_dev_t));

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = (1ULL << MY_INT1_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Установка службы прерываний и привязка функции-обработчика
    gpio_install_isr_service(0);
    gpio_isr_handler_add(MY_INT1_GPIO, isr_new_data, NULL);

    // Инициализация датчика
    ESP_ERROR_CHECK(bmi160_init(&bmi160_dev, 0x69, 0, MY_SDA, MY_SCL));
    ESP_ERROR_CHECK(bmi160_self_test(&bmi160_dev));

    bmi160_conf_t bmi160_conf = {
        .accRange = BMI160_ACC_RANGE_2G,
        .accOdr = BMI160_ACC_ODR_200HZ,
        .accAvg = BMI160_ACC_LP_AVG_2,
        .accMode = BMI160_PMU_ACC_NORMAL,
        .gyrRange = BMI160_GYR_RANGE_125DPS,
        .gyrOdr = BMI160_GYR_ODR_200HZ,
        .gyrMode = BMI160_PMU_GYR_NORMAL,
        .accUs = 0u
    };

    ESP_ERROR_CHECK(bmi160_start(&bmi160_dev, &bmi160_conf));
    ESP_ERROR_CHECK(bmi160_calibrate(&bmi160_dev));

    bmi160_int_out_conf_t intOutConf = {
        .intPin = BMI160_PIN_INT1,
        .intEnable = BMI160_INT_ENABLE,
        .intOd = BMI160_INT_PUSH_PULL,
        .intLevel = BMI160_INT_ACTIVE_HIGH // Поэтому на ESP32 мы используем POS_EDGE
    };
    ESP_ERROR_CHECK(bmi160_enable_int_new_data(&bmi160_dev, &intOutConf));

    ESP_LOGI(TAG, "BMI160 Init Done. Waiting for data...");

    bmi160_result_t result;

    while(true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        esp_err_t ret = bmi160_read_data(&bmi160_dev, &result);
        
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "ACC: X: %+.3f  Y: %+.3f  Z: %+.3f | GYRO: X: %+.3f  Y: %+.3f  Z: %+.3f", 
                     result.accX, result.accY, result.accZ, 
                     result.gyroX, result.gyroY, result.gyroZ);
        } else {
            ESP_LOGW(TAG, "Failed to read data!");
        }
    }

    ESP_ERROR_CHECK(bmi160_free(&bmi160_dev));
    vTaskDelete(NULL);
}

void bmi160_task_start() {
    xTaskCreate(bmi160_task, "bmi160_task", 4096, NULL, 5, &xTaskHandlePtr);
}