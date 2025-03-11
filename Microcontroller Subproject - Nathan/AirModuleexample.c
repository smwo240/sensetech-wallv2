#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// Pin definitions
#define BUTTON_PIN 15  // GPIO15 for button input
#define FAN_PIN 16     // GPIO16 for fan control (PWM)

// Function to set PWM duty cycle (0-100%)
void set_fan_speed(uint16_t duty_cycle) {
    pwm_set_gpio_level(FAN_PIN, duty_cycle * (65535 / 100)); // Scale to 16-bit PWM
}

// Function to initialize the fan PWM
void init_fan_pwm() {
    gpio_set_function(FAN_PIN, GPIO_FUNC_PWM);  // Set GPIO as PWM
    uint slice_num = pwm_gpio_to_slice_num(FAN_PIN); // Get PWM slice

    pwm_set_wrap(slice_num, 65535);  // 16-bit resolution
    pwm_set_clkdiv(slice_num, 1.0);  // Set clock divider
    pwm_set_enabled(slice_num, true); // Enable PWM
}

// Function to initialize button input
void init_button() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Enable internal pull-up
}

int main() {
    // Initialize I/O
    stdio_init_all();
    init_fan_pwm();
    init_button();

    while (true) {
        // Check if the button is pressed (active low)
        if (gpio_get(BUTTON_PIN) == 0) {
            printf("Button pressed! Turning fan ON\n");
            set_fan_speed(50);  // Set fan to 50% duty cycle

            sleep_ms(20000);  // Keep the fan on for 20 seconds
            
            printf("Turning fan OFF\n");
            set_fan_speed(0);  // Turn fan off
        }

        // Small delay for button debounce
        sleep_ms(100);
    }

    return 0;
}
