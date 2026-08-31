#include "peripherals.h"

// Variables
QueueHandle_t switch_queue;
QueueHandle_t incoming_queue;
uint32_t counter = 0;
volatile uint64_t last_isr_time = 0;
gpio_config_t io_config = {
    .pin_bit_mask = (1 << GPIO_NUM_6),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_NEGEDGE,
};


// This function configures the onboard RGB LED on the ESP32. Note
// that our LED_PIN must match the specific's board pinout.
led_strip_handle_t configure_led(void) {
   
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN,           // The GPIO that connected to the LED strip's data line
        .max_leds = 1,                       // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,        
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, 
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10000000 // 10mil hz
    };
    
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    return led_strip;
}


// Interrupt handler: debounces the switch and increments the amount of button
// presses this cycle. Amount of button presses is tracked with xQueueSendFromISR
void IRAM_ATTR switch_isr(void *arg) {
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


// This function triggers an LED countdown before we collect data.
void led_countdown(led_strip_handle_t rgb) {
    // Perform a 2, 1, hold.
    printf("Collecting data in 1 second.\n");

    // 3
    led_strip_set_pixel(rgb, 0, 100, 0, 0);
    led_strip_refresh(rgb);
    vTaskDelay(250 / portTICK_PERIOD_MS);

    led_strip_clear(rgb);
    led_strip_refresh(rgb);
    vTaskDelay(250 / portTICK_PERIOD_MS);

    // 2
    led_strip_set_pixel(rgb, 0, 100, 0, 0);
    led_strip_refresh(rgb);
    vTaskDelay(250 / portTICK_PERIOD_MS);

    led_strip_clear(rgb);
    led_strip_refresh(rgb);
    vTaskDelay(250 / portTICK_PERIOD_MS);

    // Change to red forever until changed by the classifier.
    led_strip_set_pixel(rgb, 0, 100, 0, 0);
    led_strip_refresh(rgb);
    vTaskDelay(5000 / portMAX_DELAY);
}

void led_choice(led_strip_handle_t rgb, int choice) {
    switch (choice) {
        case 0:    // leftchop red
            led_strip_set_pixel(rgb, 0, 255, 0, 0);
            break;

        case 1:    // leftjab orange
            led_strip_set_pixel(rgb, 0, 255, 75, 0);
            break;
        
        case 2:    // leftwave yellow
            led_strip_set_pixel(rgb, 0, 255, 255, 0);
            break;

        case 3:    // rightchop green
            led_strip_set_pixel(rgb, 0, 0, 255, 0);
            break;
        case 4:    // rightjab blue
            led_strip_set_pixel(rgb, 0, 0, 0, 255);
            break;
        case 5:    // rightwave violet
            led_strip_set_pixel(rgb, 0, 238, 130, 238);
            break;

        default:    // turn LED off
            led_strip_clear(rgb);
            break;   
    }
    led_strip_refresh(rgb);
}