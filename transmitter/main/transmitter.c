/* For now, I'm just going to repurpose this transmitter file on this 
   branch as the button and LED testing suite. Can't really do much with
   this anyways until we get a second breadboard. */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mpu6050.h"
#include "espnow.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "led_strip.h"
#include <esp_timer.h>

#define LED_PIN 38 // 47 for our actual board, remember to change!
#define DEBOUNCE_DELAY_US 200000ULL
static QueueHandle_t switch_queue;
static uint32_t counter = 0;
static volatile uint64_t last_isr_time = 0;


led_strip_handle_t configure_led(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN, // The GPIO that connected to the LED strip's data line
        .max_leds = 1,      // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,        // LED strip model
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color order of the strip: GRB

    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10000000 // 10mil
    };
    
    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    return led_strip;
}

gpio_config_t io_config = {
    .intr_type = GPIO_INTR_NEGEDGE,
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = (1 << GPIO_NUM_6),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE
};

static void IRAM_ATTR switch_isr(void *arg) {
    uint64_t now = esp_timer_get_time(); // Get current time in microseconds
    // Check if debounce period has passed, then process the button press
    if (now - last_isr_time > DEBOUNCE_DELAY_US) {
        counter++;
        uint32_t cnt = counter;
        BaseType_t higher_priority_task_woken = pdFALSE;
        xQueueSendFromISR(switch_queue, &cnt, &higher_priority_task_woken); // Send counter to queue from ISR
        last_isr_time = now;
        if (higher_priority_task_woken) {
            portYIELD_FROM_ISR();
        }
    }
}

static void led_countdown(led_strip_handle_t rgb) {
    // Perform a 3, 2, 1, hold for 5sec.

    // 3
    led_strip_set_pixel(rgb, 0, 255, 0, 0);
    led_strip_refresh(rgb);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    led_strip_clear(rgb);
    led_strip_refresh(rgb);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // 2
    led_strip_set_pixel(rgb, 0, 255, 0, 0);
    led_strip_refresh(rgb);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    led_strip_clear(rgb);
    led_strip_refresh(rgb);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // 1
    led_strip_set_pixel(rgb, 0, 255, 0, 0);
    led_strip_refresh(rgb);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    led_strip_clear(rgb);
    led_strip_refresh(rgb);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // Hold
    led_strip_set_pixel(rgb, 0, 255, 0, 0);
    led_strip_refresh(rgb);
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    led_strip_clear(rgb);
    led_strip_refresh(rgb);
    vTaskDelay(200 / portTICK_PERIOD_MS);

}

void app_main(void) {

    // Setup GPIO pullups and onboard LED
    gpio_config(&io_config);
    led_strip_handle_t rgb = configure_led();

    // Setup queue, interrupt service routine, etc.
    switch_queue = xQueueCreate(1, sizeof(uint32_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_6, switch_isr, NULL);
    uint32_t button_counter;

    while (1) {
        if (xQueueReceive(switch_queue, &button_counter, portMAX_DELAY)) {
            printf("Button press: %ld\n", button_counter);
            led_countdown(rgb);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}