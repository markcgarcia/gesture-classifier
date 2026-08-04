/* This is a basic I2C driver that allows the ESP32 to read data
   from an MPU6050 IMU. */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mpu6050.h"
#include "espnow.h"
#include "driver/i2c_master.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

// 200 samples x 6 measurements = 1200 floats
// ordered from ax, ay, ax, gx, gy, gz, ax2, ay2...
static float input_buf[1200];

// This callback function copies our data into out_ptr.
// out_ptr is where we need to store our data.
// offset is where we start reading data from.
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        *(out_ptr + i) = *(input_buf + offset + i);
    }
    return 0;
}

extern "C" void app_main(void) {
    /* Initialize I2C with the MPU6050 */
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    i2c_master_init(&bus_handle, &dev_handle);

    /* Demonstrate writing by waking up MPU6050 */
    imu_write_reg(dev_handle, 0x6B, 0x00);

    // /* Initialize struct for measurements */
    // DataSample sample;
    // sample.num = 1;

    /* Initialize the classifier */
    run_classifier_init();
    signal_t signal;                // wrapper
    ei_impulse_result_t result;     // inference output
    EI_IMPULSE_ERROR res;       
    
    /* Prompt user to do a gesture */
    printf("Collecting data in 1 second.\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    fill_input_buffer(dev_handle, input_buf);

    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;   // length of buffer
    signal.get_data = &get_signal_data;                         // register calback function

    res = run_classifier(&signal, &result, false);
    printf("Returned: %d\n", res);

    printf("Gesture predictions:\n");
    for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        printf(" %s ", ei_classifier_inferencing_categories[i]);
        printf("%.2f\n", result.classification[i].value);
    }

    /* Keep task alive */
    while (1) {
        vTaskDelay(10 / portTICK_PERIOD_MS); 
    }
}


