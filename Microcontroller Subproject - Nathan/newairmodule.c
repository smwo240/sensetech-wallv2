#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#define FAN_PWM_PIN 16         // PWM pin for fan
#define LED_RED_PIN 17         // Red LED pin
#define LED_YELLOW_PIN 18      // Yellow LED pin
#define LED_GREEN_PIN 19       // Green LED pin
#define POT_ADC_PIN 26         // ADC channel 0 (GPIO 26)
#define AUTO_SHUTOFF_MS 30000  // Auto shutoff after 30s

#define BUBBLE_BLOWER_PIN 21   // GPIO pin for bubble blower control
#define BUBBLE_BUTTON_PIN 20   // GPIO pin for button input

volatile uint32_t last_change_time = 0; // Timer for auto shutoff
volatile bool fan_running = false;
volatile bool bubble_blower_running = false;

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

// Function to handle the bubble blower
void handle_bubble_blower() {
    // If the button is pressed
    if (gpio_get(BUBBLE_BUTTON_PIN) == 0) {
        // If fan is running, turn it off
        if (fan_running) {
            pwm_set_chan_level(pwm_gpio_to_slice_num(FAN_PWM_PIN), PWM_CHAN_A, 0);
            set_led_color(0, 0, 0);  // Turn off LED
            fan_running = false;
        }

        // Activate bubble blower
        gpio_put(BUBBLE_BLOWER_PIN, 1);
        bubble_blower_running = true;
        last_change_time = millis();

        printf("Bubble blower activated\n");
    }
}

// Function to update fan speed based on potentiometer
void update_fan_speed(uint fan_slice) {
    // Read ADC value (12-bit, range: 0-4095)
    uint16_t adc_value = adc_read();
    uint16_t pwm_value = (adc_value * 65535) / 4095; // Scale to 16-bit PWM range
    
    if (adc_value > 50) { // Ensure fan only turns on when potentiometer is moved
        // If bubble blower is running, turn it off
        if (bubble_blower_running) {
            gpio_put(BUBBLE_BLOWER_PIN, 0);
            bubble_blower_running = false;
        }

        // Set fan speed
        pwm_set_chan_level(fan_slice, PWM_CHAN_A, pwm_value);
        fan_running = true;
        last_change_time = millis();

        // Set LED color based on speed
        if (adc_value < 1341) { // 1% - 33%
            set_led_color(1, 0, 0); // Red LED
        } else if (adc_value < 2731) { // 34% - 67%
            set_led_color(0, 1, 0); // Yellow LED
        } else { // 68% - 99%
            set_led_color(0, 0, 1); // Green LED
        }
    } else {
        fan_running = false;
    }
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

    // Initialize bubble blower pin
    gpio_init(BUBBLE_BLOWER_PIN);
    gpio_set_dir(BUBBLE_BLOWER_PIN, GPIO_OUT);
    gpio_put(BUBBLE_BLOWER_PIN, 0); // Ensure blower is off at start

    // Initialize button
    gpio_init(BUBBLE_BUTTON_PIN);
    gpio_set_dir(BUBBLE_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUBBLE_BUTTON_PIN); // Pull-up resistor for stable input

    uint16_t prev_speed = 0;

    while (1) {
        // Handle bubble blower input
        handle_bubble_blower();

        // Update fan speed
        update_fan_speed(fan_slice);

        // Auto shutoff logic
        if ((millis() - last_change_time) >= AUTO_SHUTOFF_MS) {
            if (fan_running) {
                pwm_set_chan_level(fan_slice, PWM_CHAN_A, 0); // Turn off fan
                set_led_color(0, 0, 0); // Turn off LED
                fan_running = false;
            }
            if (bubble_blower_running) {
                gpio_put(BUBBLE_BLOWER_PIN, 0); // Turn off bubble blower
                bubble_blower_running = false;
            }
        }

        sleep_ms(100); // Small delay to reduce CPU usage
    }

    return 0;
}
