idf_component_register(SRCS "gesture-classifier.c"
                       INCLUDE_DIRS "."
                       REQUIRES freertos log driver esp_driver_i2c esp_driver_gpio)
