#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mpu6050.h"
#include "espnow.h"
#include "driver/i2c_master.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

// Used for button and LED countdown
#include <esp_timer.h>
#include "driver/gpio.h"
#include "led_strip.h"
#include "peripherals.h"


extern "C" void app_main(void) {
    // Initialize I2C with the MPU6050 
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    i2c_master_init(&bus_handle, &dev_handle);

    // Initialize ESPNOW for this board, and add Rx board (B)
    // as a peer for communication.
    wifi_espnow_startup();
    espnow_add_peer();

    // Register callback function
    esp_now_register_send_cb(send_cb);

    // Demonstrate writing by waking up MPU6050 
    imu_write_reg(dev_handle, 0x6B, 0x00);

    // Initialize struct for measurements 
    DataSample tx_sample;
    tx_sample.num = 1;

    while (1) {
        // Read IMU sensor
        if (imu_read_burst(dev_handle, ACCEL_XOUT_REG_ADDR, 14, &tx_sample) != ESP_OK) {
            printf("Error! Did not return ESP_OK!\n");
        }
        
        // Send packets every 10ms
        esp_now_send(NULL, (uint8_t*)&tx_sample, sizeof(DataSample)); 
        vTaskDelay(10 / portTICK_PERIOD_MS);
        tx_sample.num++;
    }
}