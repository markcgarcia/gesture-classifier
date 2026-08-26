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

    // Create FreeRTOS queue to hold samples
    incoming_queue = xQueueCreate(1024, sizeof(DataSample));

    // Initialize ESPNOW for this board
    wifi_espnow_startup();

    // Register callback function
    esp_now_register_recv_cb(recv_cb);

    // Demonstrate writing by waking up MPU6050 
    imu_write_reg(dev_handle, 0x6B, 0x00);

    // Initialize struct for measurements 
    DataSample rx_sample, tx_sample;
    FloatSample frx, ftx;
    rx_sample.num = 1;

    while (1) {
         if (xQueueReceive(incoming_queue, &tx_sample, portMAX_DELAY)) {

            // Read self (Rx) sensor
            if (imu_read_burst(dev_handle, ACCEL_XOUT_REG_ADDR, 14, &rx_sample) != ESP_OK) {
                printf("Error! Did not return ESP_OK!\n");
            }

            // Convert both samples to float values
            frx = convert_sample(&rx_sample);
            ftx = convert_sample(&tx_sample);

            // Print out values
            // printf("rx: %d, ", frx.num);
            // print_packet(&frx);
            // printf("tx: %d, ", ftx.num);
            // print_packet(&ftx);
            print_combined(&frx, &ftx);

            rx_sample.num++;
         }
    }
}