#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Pin Definitions
#define PWM_PIN 16  // GPIO16 for PWM output

// Function to set the PWM duty cycle (0-65535)
void set_pwm_duty_cycle(uint16_t duty_cycle) {
    pwm_set_gpio_level(PWM_PIN, duty_cycle); // Set PWM duty cycle
}

// Function to initialize PWM
void init_pwm() {
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);  // Set GPIO as PWM
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Get PWM slice number

    pwm_set_wrap(slice_num, 65535);  // Set 16-bit resolution (0-65535)
    pwm_set_clkdiv(slice_num, 1.0);  // Set clock divider (adjustable for speed)
    pwm_set_enabled(slice_num, true); // Enable PWM
}

// Function to control brightness smoothly
void pwm_brightness_control() {
    while (true) {
        // Gradually increase brightness
        for (uint16_t duty = 0; duty <= 65535; duty += 1000) {
            set_pwm_duty_cycle(duty);
            sleep_ms(10);  // Small delay for smooth transition
        }

        // Gradually decrease brightness
        for (uint16_t duty = 65535; duty > 0; duty -= 1000) {
            set_pwm_duty_cycle(duty);
            sleep_ms(10);
        }
    }
}

int main() {
    // Initialize stdio for debugging
    stdio_init_all();

    // Initialize PWM on GPIO16
    init_pwm();

    // Run brightness control
    pwm_brightness_control();

    return 0;
}

