#pragma once

#include <esp_wifi.h>
#include <esp_mac.h>
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mpu6050.h"

// Macros for board MAC addresses
#define MAC_A {0xa4, 0xcb, 0x8f, 0xd9, 0x2e, 0xb8}
#define MAC_B {0xa4, 0xcb, 0x8f, 0xd9, 0x2b, 0x0c}
#define MAC_C {0xa4, 0xcb, 0x8f, 0xd9, 0x34, 0x50}

void send_cb(const wifi_tx_info_t *tx_info, const esp_now_send_status_t status);
void recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, const int len);
void wifi_espnow_startup();
void echo_mac_wifi();
void espnow_add_peer();
