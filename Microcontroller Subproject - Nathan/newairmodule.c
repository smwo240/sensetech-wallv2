#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#define FAN_PWM_PIN 16     // PWM pin for fan
#define LED_RED_PIN 17     // Red LED pin
#define LED_YELLOW_PIN 18  // Yellow LED pin
#define LED_GREEN_PIN 19   // Green LED pin
#define POT_ADC_PIN 26     // ADC channel 0 (GPIO 26)
#define AUTO_SHUTOFF_MS 30000 // Auto shutoff after 30s

volatile uint32_t last_change_time = 0; // Timer for auto shutoff

// Function to initialize PWM
void init_pwm(uint pin, uint slice) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    pwm_set_wrap(slice, 65535); // 16-bit resolution
    pwm_set_chan_level(slice, PWM_CHAN_A, 0);
    pwm_set_enabled(slice, true);
}

// Function to initialize ADC
void init_adc() {
    adc_init();
    adc_gpio_init(POT_ADC_PIN);
    adc_select_input(0); // Select ADC channel 0 (GPIO 26)
}

// Function to set LED color
void set_led_color(bool red, bool yellow, bool green) {
    gpio_put(LED_RED_PIN, red);
    gpio_put(LED_YELLOW_PIN, yellow);
    gpio_put(LED_GREEN_PIN, green);
}

// Function to get current time in milliseconds
uint32_t millis() {
    return to_ms_since_boot(get_absolute_time());
}

int main() {
    stdio_init_all();

    // Initialize fan PWM
    uint fan_slice = pwm_gpio_to_slice_num(FAN_PWM_PIN);
    init_pwm(FAN_PWM_PIN, fan_slice);

    // Initialize LEDs
    gpio_init(LED_RED_PIN);
    gpio_init(LED_YELLOW_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_YELLOW_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    // Initialize ADC for potentiometer
    init_adc();

    uint16_t prev_speed = 0;
    
    while (1) {
        // Read ADC value (12-bit, range: 0-4095)
        uint16_t adc_value = adc_read();
        uint16_t pwm_value = (adc_value * 65535) / 4095; // Scale to 16-bit PWM range
        
        // Determine fan speed level based on ADC range
        if (adc_value < 1341) { // 1% - 33%
            pwm_set_chan_level(fan_slice, PWM_CHAN_A, pwm_value);
            set_led_color(1, 0, 0); // Red LED
        } else if (adc_value < 2731) { // 34% - 67%
            pwm_set_chan_level(fan_slice, PWM_CHAN_A, pwm_value);
            set_led_color(0, 1, 0); // Yellow LED
        } else { // 68% - 99%
            pwm_set_chan_level(fan_slice, PWM_CHAN_A, pwm_value);
            set_led_color(0, 0, 1); // Green LED
        }

        // If the input has changed, update the timer
        if (adc_value != prev_speed) {
            last_change_time = millis();
            prev_speed = adc_value;
        }

        // Check if auto shutoff condition is met
        if ((millis() - last_change_time) >= AUTO_SHUTOFF_MS) {
            pwm_set_chan_level(fan_slice, PWM_CHAN_A, 0); // Turn off fan
            set_led_color(0, 0, 0); // Turn off LED
        }

        sleep_ms(3000); // Delay for 3 seconds before checking again
    }

    return 0;
}
//end
