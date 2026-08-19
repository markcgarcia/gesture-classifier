#pragma once 
#include <esp_timer.h>
#include "driver/gpio.h"
#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_PIN 48 // 47 for our actual board, remember to change!
#define DEBOUNCE_DELAY_US 200000ULL

extern QueueHandle_t switch_queue;
extern QueueHandle_t incoming_queue;
extern uint32_t counter;
extern volatile uint64_t last_isr_time;
extern gpio_config_t io_config;

led_strip_handle_t configure_led(void);
void switch_isr(void *arg);
void led_countdown(led_strip_handle_t rgb);
void led_choice(led_strip_handle_t rgb, int choice);

#ifdef __cplusplus
}
#endif