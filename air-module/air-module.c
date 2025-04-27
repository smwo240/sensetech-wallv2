/*
 * File:            air-module.c
 * Description:     The main code used to run the functionality of the air module as specified.
 * Parameters:      GPIO definitions - the GP# used for the pin where the device is connected to or controlled by the board.
 *                  ACTIVE_DURATION_MS - time the devices will be ON when prompted, in ms.
 *                  DEBOUNCE_MS - used to eliminate additional presses caused by switch bounce on open/close.
 * Functionality:   Button presses activate the GPIO outputs that control either the fan or bubble motor devices.
 *                  Use the parameters to control time set to ON and to control switch debounce.
 */

#include <stdio.h>
#include "pico/stdlib.h"

// GPIO definitions
#define FAN_BUTTON        16
#define FAN_LED           17
#define FAN_OUTPUT        6

#define BUBBLE_BUTTON     20
#define BUBBLE_LED        21
#define BUBBLE_OUTPUT     27

#define ACTIVE_DURATION_MS 5000
#define DEBOUNCE_MS 200
#define FAN_AUTO_OFF_MS 30000 // 30 seconds


bool is_pressed(uint gpio) {
    return gpio_get(gpio) == 0;
}

void run_device(uint output_gpio, uint led_gpio) {
    // BUBBLE
    if (output_gpio == BUBBLE_OUTPUT)
        gpio_put(output_gpio, 0);  // Turn device ON - for bubble motor ON is 0 because the transistor used.
    else 
    // FAN
        gpio_put(output_gpio, 1);

    gpio_put(led_gpio, 1);          // Turn LED ON
    sleep_ms(ACTIVE_DURATION_MS);   // stay on for active duration

    // BUBBLE
    if (output_gpio == BUBBLE_OUTPUT)
        gpio_put(output_gpio, 1);  // Turn device ON - for bubble motor OFF is 1 because the transistor used.
    else 
    // FAN
        gpio_put(output_gpio, 0);

    gpio_put(led_gpio, 0);     // Turn LED OFF  
}

int main() {
    stdio_init_all();
    bool fan_on = false;
    bool fan_blinking = false;
    absolute_time_t fan_on_time;

    // Initialize GPIOs
    #pragma region init
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);

    gpio_init(FAN_BUTTON);
    gpio_set_dir(FAN_BUTTON, GPIO_IN);
    gpio_pull_up(FAN_BUTTON);  // Active-low
    // gpio_set_irq_enabled_with_callback(FAN_BUTTON, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(FAN_LED);
    gpio_set_dir(FAN_LED, GPIO_OUT);
    gpio_put(FAN_LED, 0);

    gpio_init(FAN_OUTPUT);
    gpio_set_dir(FAN_OUTPUT, GPIO_OUT);
    gpio_put(FAN_OUTPUT, 0);

    gpio_init(BUBBLE_BUTTON);
    gpio_set_dir(BUBBLE_BUTTON, GPIO_IN);
    gpio_pull_up(BUBBLE_BUTTON);  // Active-low
    // gpio_set_irq_enabled_with_callback(BUBBLE_BUTTON, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_fall);
    // gpio_set_irq_enabled_with_callback(BUBBLE_BUTTON, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_rise);

    gpio_init(BUBBLE_LED);
    gpio_set_dir(BUBBLE_LED, GPIO_OUT);
    gpio_put(BUBBLE_LED, 0);

    gpio_init(BUBBLE_OUTPUT);
    gpio_set_dir(BUBBLE_OUTPUT, GPIO_OUT);
    gpio_put(BUBBLE_OUTPUT, 1); // 1 is off
    #pragma endregion 

    // gpio_put(BUBBLE_OUTPUT, 1);

while (true) {
    // Check fan button
    if (is_pressed(FAN_BUTTON)) {
        if (!fan_on) {
            gpio_put(FAN_OUTPUT, 1);
            gpio_put(FAN_LED, 1);
            fan_on = true;
            fan_blinking = false;
            fan_on_time = get_absolute_time();
        } else {
            gpio_put(FAN_OUTPUT, 0);
            gpio_put(FAN_LED, 0);
            fan_on = false;
            fan_blinking = false;
        }
        while (is_pressed(FAN_BUTTON)) {
            sleep_ms(10);
        }
        sleep_ms(DEBOUNCE_MS);
    }

    // Check bubble blower button
    if (is_pressed(BUBBLE_BUTTON)) {
        run_device(BUBBLE_OUTPUT, BUBBLE_LED);
        while (is_pressed(BUBBLE_BUTTON)) {
            sleep_ms(10);
        }
        sleep_ms(DEBOUNCE_MS);
    }

    // Fan auto-off and blinking
    if (fan_on) {
        int64_t fan_runtime_us = absolute_time_diff_us(fan_on_time, get_absolute_time());

        // Start blinking after 25s
        if (fan_runtime_us > (FAN_AUTO_OFF_MS - 5000) * 1000 && fan_runtime_us < FAN_AUTO_OFF_MS * 1000) {
            fan_blinking = true;
        }

        // Auto turn off after 30s
        if (fan_runtime_us > FAN_AUTO_OFF_MS * 1000) {
            gpio_put(FAN_OUTPUT, 0);
            gpio_put(FAN_LED, 0);
            fan_on = false;
            fan_blinking = false;
        }
    }

    // Handle LED blinking
    if (fan_blinking) {
        static bool led_state = false;
        static absolute_time_t last_blink_time = {0};

        if (absolute_time_diff_us(last_blink_time, get_absolute_time()) > 500 * 1000) { // 500ms
            led_state = !led_state;
            gpio_put(FAN_LED, led_state);
            last_blink_time = get_absolute_time();
        }
    }

    sleep_ms(100); // light CPU sleep
}



    return 0;
}
