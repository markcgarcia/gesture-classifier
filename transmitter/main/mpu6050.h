#pragma once

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#define ACCEL_XOUT_REG_ADDR     0x3B
#define ACCEL_YOUT_REG_ADDR     0x3D
#define ACCEL_ZOUT_REG_ADDR     0x3F
#define TEMP_OUT_REG_ADDR       0x41
#define GYRO_XOUT_REG_ADDR      0x43
#define GYRO_YOUT_REG_ADDR      0x45
#define GYRO_ZOUT_REG_ADDR      0x47
#define ACCEL_SENSITIVITY       16384
#define TEMP_SENSITIVITY        340
#define GYRO_SENSITIVITY        131

// Struct that contains all of our necessary data (gyro xyz, accelo xyz, temp, time)
typedef struct {
    int16_t num;    // packet number
    char mac[6];    // mac address
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t tp;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} DataSample;

esp_err_t imu_read_burst(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, size_t len, DataSample *sample);
esp_err_t imu_read_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t imu_write_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data);
void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle);
void print_packet(DataSample *sample);
