idf_component_register(SRCS "main.c" "BMI160_task/BMI160_task.c"
                       INCLUDE_DIRS "." "./BMI160_task" 
                       REQUIRES esp_driver_i2c
)
