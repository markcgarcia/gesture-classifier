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

// Classifier specifics
static float input_buf[2400];

static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        *(out_ptr + i) = *(input_buf + offset + i);
    }
    return 0;
}

// Collects 200 10ms samples across six axes. This function takes in float *buf, which
// should point to the input buffer that we want to fill.
void fill_input_buffer(i2c_master_dev_handle_t dev_handle, float *buf, QueueHandle_t incoming_queue) {
    
    // Flush queue
    vQueueDelete(incoming_queue);
    incoming_queue = xQueueCreate(1024, sizeof(DataSample));
    printf("Now collecting!\n");
    
    // Initialize struct for measurements 
    DataSample rx_sample, tx_sample;
    FloatSample frx, ftx;
    rx_sample.num = 1;
    int i = 0; 
    while (i < 200) {
         if (xQueueReceive(incoming_queue, &tx_sample, portMAX_DELAY)) {
            // Read self (Rx) sensor
            if (imu_read_burst(dev_handle, ACCEL_XOUT_REG_ADDR, 14, &rx_sample) != ESP_OK) {
                printf("Error! Did not return ESP_OK!\n");
            }

            // Convert both samples to float values
            frx = convert_sample(&rx_sample);
            ftx = convert_sample(&tx_sample);
            rx_sample.num++;

            // Put sample fields into buf array
            *(buf + 0  + 12*i) = frx.ax;
            *(buf + 1  + 12*i) = frx.ay;
            *(buf + 2  + 12*i) = frx.az;
            *(buf + 3  + 12*i) = frx.gx;
            *(buf + 4  + 12*i) = frx.gy;
            *(buf + 5  + 12*i) = frx.gz;
            *(buf + 6  + 12*i) = ftx.ax;
            *(buf + 7  + 12*i) = ftx.ay;
            *(buf + 8  + 12*i) = ftx.az;
            *(buf + 9  + 12*i) = ftx.gx;
            *(buf + 10 + 12*i) = ftx.gy;
            *(buf + 11 + 12*i) = ftx.gz;

            i++;
         }
    }
}


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

    /* Initialize the classifier */
    run_classifier_init();
    signal_t signal;                // wrapper
    ei_impulse_result_t result;     // inference output
    EI_IMPULSE_ERROR res;       
    
    /* Prompt user to do a gesture */
    printf("Collecting data in 1 second.\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    fill_input_buffer(dev_handle, input_buf, incoming_queue);

    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;   // length of buffer
    signal.get_data = &get_signal_data;                         // register calback function

    res = run_classifier(&signal, &result, false);
    printf("Returned: %d\n", res);

    printf("Gesture predictions:\n");
    for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        printf(" %s ", ei_classifier_inferencing_categories[i]);
        printf("%.2f\n", result.classification[i].value);
    }

    while (1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}