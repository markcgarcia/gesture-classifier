idf_component_register(SRCS "peripherals.c" "mpu6050.c" "espnow.c" "transmitter.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES freertos log driver esp_driver_i2c esp_driver_gpio esp_timer esp_wifi esp_eth nvs_flash edge-impulse esp_driver_gpio)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -DEI_PORTING_ESPRESSIF=1
    -Wno-missing-field-initializers
)