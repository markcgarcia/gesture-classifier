#include "mpu6050.h"
#include "driver/i2c_master.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Perform a burst read of current accel, gyro, and temp sensors.
esp_err_t imu_read_burst(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, size_t len, DataSample *sample) {
    // Store in read buffer "arr"
    uint8_t arr[14];
    esp_err_t err =  i2c_master_transmit_receive(dev_handle, &reg_addr, 1, arr, len, -1);

    // Move contents from "arr" to struct parameter "sample"
    sample->ax = (arr[0] << 8) + arr[1];
    sample->ay = (arr[2] << 8) + arr[3];
    sample->az = (arr[4] << 8) + arr[5];
    sample->tp = (arr[6] << 8) + arr[7];
    sample->gx = (arr[8] << 8) + arr[9];
    sample->gy = (arr[10] << 8) + arr[11];
    sample->gz = (arr[12] << 8) + arr[13];
    return err;
}

// Reads a single register from the MPU6050.
esp_err_t imu_read_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len) {
    return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, -1);
}

// Writes to a single register on the MPU6050.
esp_err_t imu_write_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data) {
    uint8_t arr[2] = {reg_addr, data};
    return i2c_master_transmit(dev_handle, arr, 2, 10);
}

// Startup for the MPU6050.
void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle) {
    /* Configure the bus handle */
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = GPIO_NUM_5,
        .sda_io_num = GPIO_NUM_4,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, bus_handle));

    /* Configure the dev handle */
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x68, 
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle));
}

// Prints out our six axes of measurement in a given sample. Note that our timestamp,
// temp, and MAC address are left off (for Edge Impulse ML-training purposes).
void print_packet(FloatSample *sample) {
    printf("%.7f,%.7f,%.7f,%.7f,%.7f,%.7f\n", 
            sample->ax, sample->ay, sample->az, sample->gx, sample->gy, sample->gz);
}

// Converts our sample data from raw integer to float. This is meant to be
// done on the receiving board, which is why it isn't merged with the burst
// read.
FloatSample convert_sample(DataSample *sample) {
    FloatSample f;
    f.num = sample->num;
    f.ax = (float)sample->ax/ACCEL_SENSITIVITY * 9.8066;
    f.ay = (float)sample->ay/ACCEL_SENSITIVITY * 9.8066;
    f.az = (float)sample->az/ACCEL_SENSITIVITY * 9.8066;
    f.tp = (float)sample->tp/TEMP_SENSITIVITY + 36.53;
    f.gx = (float)sample->gx/GYRO_SENSITIVITY;
    f.gy = (float)sample->gy/GYRO_SENSITIVITY;
    f.gz = (float)sample->gz/GYRO_SENSITIVITY;

    return f;
}

// Collects 200 10ms samples across six axes. This function takes in float *buf, which
// should point to the input buffer that we want to fill.
void fill_input_buffer(i2c_master_dev_handle_t dev_handle, float *buf) {
    DataSample sample;
    for (int i = 0; i < 200; i++) {
        
        if (imu_read_burst(dev_handle, ACCEL_XOUT_REG_ADDR, 14, &sample) != ESP_OK) {
            printf("Error! Did not return ESP_OK!\n");
        }
        sample.num = 1;
        // Convert sample to float
        FloatSample fs = convert_sample(&sample);

        // Put sample fields into buf array
        *(buf + 0 + 6*i) = fs.ax;
        *(buf + 1 + 6*i) = fs.ay;
        *(buf + 2 + 6*i) = fs.az;
        *(buf + 3 + 6*i) = fs.gx;
        *(buf + 4 + 6*i) = fs.gy;
        *(buf + 5 + 6*i) = fs.gz;

        sample.num += 1;

        // Repeat
        vTaskDelay(10 / portTICK_PERIOD_MS); 
    }
}