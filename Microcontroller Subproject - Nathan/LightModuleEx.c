#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Pin Definitions
#define BTN1_PIN 17      // GPIO17 (Button 1)
#define BTN1_LED_PIN 19  // GPIO19 (LED for Button 1)
#define BTN2_PIN 0       // GPIO0 (Button 2)
#define BTN2_LED_PIN 1   // GPIO1 (LED for Button 2)

// Global Variables
volatile bool btn1_on = false;
volatile bool btn2_on = false;
volatile bool clockwise = true;  // Direction of LED rotation
volatile int position = 1;        // Position of the "light" in the pattern

// Button Interrupt Handler
void button_callback(uint gpio, uint32_t events) {
    if (gpio == BTN1_PIN) {  // Button 1 pressed
        btn1_on = !btn1_on;  // Toggle state
        clockwise = true;    // Set clockwise direction
    } else if (gpio == BTN2_PIN) {  // Button 2 pressed
        btn2_on = !btn2_on;  // Toggle state
        clockwise = false;   // Set counterclockwise direction
    }
}

// Timer Callback Function for Light Rotation
bool timer_callback(repeating_timer_t *rt) {
    // Update LED states based on position
    if (position == 1) {
        gpio_put(BTN1_LED_PIN, 1);
        gpio_put(BTN2_LED_PIN, 0);
    } else if (position == 2) {
        gpio_put(BTN1_LED_PIN, 0);
        gpio_put(BTN2_LED_PIN, 1);
    }

    // Update position based on direction
    if (clockwise) {
        position++;
    } else {
        position--;
    }

    // Wrap-around position
    if (position > 2) {
        position = 1;
    } else if (position < 1) {
        position = 2;
    }

    return true; // Continue the timer
}

int main() {
    // Initialize stdio for debugging
    stdio_init_all();

    // Initialize GPIOs
    gpio_init(BTN1_PIN);
    gpio_set_dir(BTN1_PIN, GPIO_IN);
    gpio_pull_up(BTN1_PIN);

    gpio_init(BTN2_PIN);
    gpio_set_dir(BTN2_PIN, GPIO_IN);
    gpio_pull_up(BTN2_PIN);

    gpio_init(BTN1_LED_PIN);
    gpio_set_dir(BTN1_LED_PIN, GPIO_OUT);

    gpio_init(BTN2_LED_PIN);
    gpio_set_dir(BTN2_LED_PIN, GPIO_OUT);

    // Attach Interrupt Handlers
    gpio_set_irq_enabled_with_callback(BTN1_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN2_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // Initialize Timer
    repeating_timer_t light_timer;
    add_repeating_timer_ms(500, timer_callback, NULL, &light_timer);

    // Main Loop (for Debugging)
    while (true) {
        printf("BTN1_ON: %d, BTN2_ON: %d, Position: %d, Clockwise: %d\n", btn1_on, btn2_on, position, clockwise);
        sleep_ms(500);
    }

    return 0;
}
