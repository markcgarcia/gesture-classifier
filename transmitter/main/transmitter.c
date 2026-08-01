/* This is a basic I2C driver that allows the ESP32 to read data
   from an MPU6050 IMU. */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mpu6050.h"
#include "espnow.h"
#include "driver/i2c_master.h"

void app_main(void) {
    /* Initialize I2c */
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    i2c_master_init(&bus_handle, &dev_handle);

    /* Initialize WiFi and ESPNOW */
    wifi_espnow_startup();
    ESP_ERROR_CHECK(esp_now_register_send_cb(send_cb));

    /* Add peer (board A) */
    espnow_add_peer();

    /* Demonstrate writing by waking up MPU6050 */
    imu_write_reg(dev_handle, 0x6B, 0x00);

    /* Initialize struct for measurements */
    DataSample sample;
    sample.num = 1;

    /* Read data from sensor */
    while (1) {
        // Read data from sensor with 10ms delay
        if (imu_read_burst(dev_handle, ACCEL_XOUT_REG_ADDR, 14, &sample) != ESP_OK) {
            printf("Error! Did not return ESP_OK!\n");
        }
        sample.num += 1;

        // Send data from this board to the receiving board
        ESP_ERROR_CHECK(esp_now_send(NULL, (uint8_t*)&sample, 22));
        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}