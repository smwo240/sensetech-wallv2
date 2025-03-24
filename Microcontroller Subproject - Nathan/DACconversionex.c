#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Pin definitions
#define DAC_PIN 17  // GPIO17 for PWM output

// Function to set PWM duty cycle (0-100%)
void set_pwm_duty_cycle(uint16_t duty_cycle) {
    pwm_set_gpio_level(DAC_PIN, duty_cycle * (65535 / 100)); // Scale to 16-bit PWM
}

// Function to initialize PWM for DAC simulation
void init_pwm_dac() {
    gpio_set_function(DAC_PIN, GPIO_FUNC_PWM);  // Set GPIO as PWM
    uint slice_num = pwm_gpio_to_slice_num(DAC_PIN); // Get PWM slice

    pwm_set_wrap(slice_num, 65535);  // 16-bit resolution
    pwm_set_clkdiv(slice_num, 1.0);  // Set clock divider
    pwm_set_enabled(slice_num, true); // Enable PWM
}

// Function to generate a sine wave
void generate_sine_wave(int steps = 100, int amplitude = 32767) {
    while (true) {
        for (int i = 0; i < steps; i++) {
            // Calculate sine wave value, scale to 16-bit PWM range (0-65535)
            uint16_t value = (uint16_t)((sin(i * 2.0 * M_PI / steps) + 1) * amplitude);
            
            // Set PWM duty cycle
            pwm_set_gpio_level(DAC_PIN, value);
            
            // Delay between steps
            sleep_ms(10);
        }
    }
}

int main() {
    // Initialize stdio for debugging
    stdio_init_all();

    // Initialize PWM for DAC simulation
    init_pwm_dac();

    // Generate sine wave
    generate_sine_wave();

    return 0;
}
