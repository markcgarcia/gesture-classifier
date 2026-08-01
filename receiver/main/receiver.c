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
    /* Initialize I2C with the MPU6050 */
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    i2c_master_init(&bus_handle, &dev_handle);

    /* Initialize WiFi and ESPNOW */
    wifi_espnow_startup();
    ESP_ERROR_CHECK(esp_now_register_recv_cb(recv_cb));

    /* Demonstrate writing by waking up MPU6050 */
    imu_write_reg(dev_handle, 0x6B, 0x00);

    /* Initialize struct for measurements */
    DataSample sample;
    sample.num = 1;

    /* Read data from sensor */
    while (1) {
        // Self (Board A) must still receive its own measurements every 10ms
        if (imu_read_burst(dev_handle, ACCEL_XOUT_REG_ADDR, 14, &sample) != ESP_OK) {
            printf("Error! Did not return ESP_OK!\n");
        }

        // Convert sample to floating-point, and stamp with MAC address.
        FloatSample fs = convert_sample(&sample);
        uint8_t temp[] = MAC_A;
        memcpy(&fs.mac, temp, 6);

        
        print_packet(&fs);
        sample.num += 1;
        
        vTaskDelay(10 / portTICK_PERIOD_MS); 
    }
}