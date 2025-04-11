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

bool is_pressed(uint gpio) {
    return gpio_get(gpio) == 0;
}

void run_device(uint output_gpio, uint led_gpio) {
    if (output_gpio == BUBBLE_OUTPUT)
        gpio_put(output_gpio, 0);  // Turn device ON - for bubble motor ON is 0 because the transistor used.
    else 
        gpio_put(output_gpio, 1);
    gpio_put(led_gpio, 1);     // Turn LED ON
    sleep_ms(ACTIVE_DURATION_MS);

    if (output_gpio == BUBBLE_OUTPUT)
        gpio_put(output_gpio, 1);  // Turn device ON - for bubble motor ON is 0 because the transistor used.
    else 
        gpio_put(output_gpio, 0);

        gpio_put(led_gpio, 0);     // Turn LED OFF  
}

int main() {
    stdio_init_all();

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
            sleep_ms(DEBOUNCE_MS);
            if (is_pressed(FAN_BUTTON)) {
                run_device(FAN_OUTPUT, FAN_LED);
            }
        }

        // Check bubble blower button
        if (is_pressed(BUBBLE_BUTTON)) {
            sleep_ms(DEBOUNCE_MS);
            if (is_pressed(BUBBLE_BUTTON)) {
                run_device(BUBBLE_OUTPUT, BUBBLE_LED);
            }
        }

        sleep_ms(100); // Light delay to reduce CPU usage
    }

    return 0;
}
