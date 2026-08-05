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

    // Setup GPIO pullups and onboard LED
    gpio_config(&io_config);
    led_strip_handle_t rgb = configure_led();

    // Setup queue, interrupt service routine, etc.
    switch_queue = xQueueCreate(1, sizeof(uint32_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_6, switch_isr, NULL);
    uint32_t button_counter;

    // Initialize I2C comms with MPU6050, wake up MPU6050
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    i2c_master_init(&bus_handle, &dev_handle);
    imu_write_reg(dev_handle, 0x6B, 0x00);

    // Initialize the classifier
    run_classifier_init();
    signal_t signal;                // wrapper
    ei_impulse_result_t result;     // inference output
    EI_IMPULSE_ERROR res;       
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;   // length of buffer
    signal.get_data = &get_signal_data;                         // register calback function

    // Check for switch and run classifier accordingly
    while (1) {
        if (xQueueReceive(switch_queue, &button_counter, portMAX_DELAY)) {
            printf("Button press: %ld\n", button_counter);

            // Collect gesture data and run classifier
            led_countdown(rgb);
            fill_input_buffer(dev_handle, input_buf);
            res = run_classifier(&signal, &result, false);
            
            // Print out classifier results
            float bestScore = 0;
            int choice = 0;
            printf("Returned: %d\n", res);
            printf("Gesture predictions:\n");
            for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
                printf(" %s ", ei_classifier_inferencing_categories[i]);
                printf("%.2f\n", result.classification[i].value);
                if (result.classification[i].value > bestScore) {
                    bestScore = result.classification[i].value;
                    choice = i;
                }
            }
            // Update LED with most-likely gesture
            led_choice(rgb, choice);
        }
    }
}


