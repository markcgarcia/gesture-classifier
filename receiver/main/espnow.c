#include <esp_wifi.h>
#include <esp_mac.h>
#include <string.h>
#include "esp_now.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "espnow.h"
#include "mpu6050.h"

#include "peripherals.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Callback function for sending (Tx only)
void send_cb(const wifi_tx_info_t *tx_info,          // new: transmission info
             const esp_now_send_status_t status)     // Status (success/fail)
{
    if (tx_info->des_addr == NULL) {
        printf("Error: null destination MAC address");
        return;
    }
    printf("Sent data to:");
    printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
        tx_info->des_addr[0], tx_info->des_addr[1], tx_info->des_addr[2], 
        tx_info->des_addr[3], tx_info->des_addr[4], tx_info->des_addr[5]);
    if (status == ESP_NOW_SEND_FAIL) {
        printf("Status: FAILED\n");
    }
}

// Callback function for receiving (Rx only)
// Receive callback function
void recv_cb(const esp_now_recv_info_t *recv_info,       // Receive struct (has send/dest MAC addr.)
                    const uint8_t *data,                 // Pointer to sent data
                    const int len)                       // Length of sent data
{
    // Error check for received packet
    if (recv_info == NULL || data == NULL || len <= 0) {
        printf("Error: invalid parameters");
        return;
    }
    
    DataSample sample;

    // Push incoming data into the queue
    memcpy(&sample, data, sizeof(DataSample));
    xQueueSendFromISR(incoming_queue, &sample, NULL);
}

// Setup for WiFi and ESPNOW 
void wifi_espnow_startup() {
    // Initialize WiFi (channel 1)
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

    // Initialize ESPNOW
    ESP_ERROR_CHECK(esp_now_init());
}

// Read MAC address and WiFi channel (debugging/verification)
void echo_mac_wifi() {
    uint8_t mac[6];
    uint8_t primary = 0;
    wifi_second_chan_t secondary = 0;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    esp_wifi_get_channel(&primary, &secondary);
    printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
        mac[0], mac[1], mac[2], mac[3], 
        mac[4], mac[5]);
    printf("WIFI: Connected AP Channel:\n Primary: %d\n Secondary: %d\n", 
        primary, secondary);
}

// Add a peer for ESPNOW
void espnow_add_peer() {
    // Add ESPNOW peer information
    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    // Clear out memory at this part of the heap
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    char source[] = MAC_B;
    memcpy((char*)peer->peer_addr, source, sizeof(source));
    peer->channel = 0;
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    free(peer);
}