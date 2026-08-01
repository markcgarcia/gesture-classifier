idf_component_register(SRCS "transmitter.c" "mpu6050.c" "espnow.c" 
                       INCLUDE_DIRS "."
                       REQUIRES freertos log driver esp_driver_i2c esp_driver_gpio esp_timer esp_wifi esp_eth nvs_flash)
