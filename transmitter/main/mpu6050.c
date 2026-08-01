#include "mpu6050.h"
#include "driver/i2c_master.h"
#include "esp_timer.h"

// Perform a burst read of current accel, gyro, and temp sensors.
// Also stamps packet with the local time that this packet was sent, in ms.
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

// Writes to a single register from the MPU6050
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

// Prints packet data.
void print_packet(DataSample *sample) {
    printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
            sample->mac[0], sample->mac[1], sample->mac[2], 
            sample->mac[3], sample->mac[4], sample->mac[5]);
     printf("num: %d\nax: %d\nay: %d\naz: %d\ntp: %d\ngx: %d\ngy: %d\ngz: %d\n",
            sample->num, sample->ax, sample->ay, sample->az, sample->tp, sample->gx, sample->gy, sample->gz);
    fflush(stdout);
}