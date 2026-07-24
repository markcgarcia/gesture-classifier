/* This is a basic I2C driver that allows the ESP32 to read data
   from an MPU6050 IMU. */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"

#define ACCEL_XOUT_REG_ADDR     0x3B
#define ACCEL_YOUT_REG_ADDR     0x3D
#define ACCEL_ZOUT_REG_ADDR     0x3F
#define TEMP_OUT_REG_ADDR       0x41
#define GYRO_XOUT_REG_ADDR      0x43
#define GYRO_YOUT_REG_ADDR      0x45
#define GYRO_ZOUT_REG_ADDR      0x47

// Goal: have a struct that contains all of our necessary data (gyro xyz, accelo xyz, temp, time)
typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t tp;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} DataSample;

// Goal: do a burst read of all measurements (7 measurements x 2 bytes each)
static esp_err_t imu_read_burst(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, size_t len, DataSample *sample) {
    // Burst read: store in read buffer "arr"
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

static esp_err_t imu_read_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len) {
    return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, -1);
}

static esp_err_t imu_write_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data) {
    uint8_t arr[2] = {reg_addr, data};
    return i2c_master_transmit(dev_handle, arr, 2, 10);
}

static void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle) {
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

void app_main(void) {
    /* Initialize I2c */
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    i2c_master_init(&bus_handle, &dev_handle);

    /* Read the MPU6050 WHO_AM_I register. Should print 0x68 */
    uint8_t data;
    if (imu_read_reg(dev_handle, 0x75, &data, 1) != ESP_OK) {
        printf("Error! Did not return \"ESP_OK\"\n");
    }
    printf("WHO_AM_I: 0x%x\n", data);

    /* Demonstrate writing by waking up MPU6050 */
    imu_write_reg(dev_handle, 0x6B, 0x00);

    /* Initialize struct for measurements */
    DataSample sample;

    /* Read data from sensor */
    while (1) {
        // Read data from sensor with 10ms delay
        if (imu_read_burst(dev_handle, ACCEL_XOUT_REG_ADDR, 14, &sample) != ESP_OK) {
            printf("Error! Did not return ESP_OK!\n");
        }
        printf("WHO_AM_I: 0x%x\n", data);
        printf("Reading ========\n");
        printf("ax: %d\nay: %d\naz: %d\ntp: %d\ngx: %d\ngy: %d\ngz: %d\n",
               sample.ax, sample.ay, sample.az, sample.tp, sample.gx, sample.gy, sample.gz);
        vTaskDelay(10 / portTICK_PERIOD_MS); 
    }
}