#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define BTN1_PIN 17     // pin 22 - GP17
#define BTN1_LED_PIN 19 // pin 25 - GP19
#define BTN2_PIN 0      // pin 1 - GP0
#define BTN2_LED_PIN 1  // pin 2 - GP1

// global variables
// Button states for light module logic - activated from interrupt handler
bool btn1_on;
bool btn2_on;
bool btn3_on;
bool btn4_on;
bool btn5_on;
bool btn6_on;
bool btn7_on;
bool btn8_on;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {
    // Interrupt routines for when a button is pressed on the face plate.
    if (gpio == BTN1_PIN) {

    } 
    else if (gpio == BTN2_PIN) {

    }
}


int main()
{
    stdio_init_all();

    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);

    // Initialize GPIO - all buttons set up for negative logic on push (pull-up resistors and interrupts on falling edge)

    // BTN1_LED_PIN
    gpio_init(BTN1_LED_PIN);
    gpio_set_dir(BTN1_LED_PIN, GPIO_OUT);
    //gpio_set_function(BTN1_LED_PIN, GPIO_FUNC_PWM); // Pwm not implemented yet
    // BTN1_PIN
    gpio_init(BTN1_PIN);
    gpio_set_dir(BTN1_PIN,GPIO_IN);
    gpio_pull_up(BTN1_PIN);
    gpio_set_irq_enabled_with_callback(BTN1_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback); // enable interrupts

    // BTN2_LED_PIN
    gpio_init(BTN2_LED_PIN);
    gpio_set_dir(BTN2_LED_PIN, GPIO_OUT);
    //BTN2_PIN
    gpio_init(BTN2_PIN);
    gpio_set_dir(BTN2_PIN, GPIO_IN);
    gpio_pull_up(BTN2_PIN);
    gpio_set_irq_enabled(BTN2_PIN, GPIO_IRQ_EDGE_FALL, true);

    // loop to model simple behavior.
    uint32_t position = 1; // position of "light" moving from button to button in circular pattern
    bool clockwise = true;  // true = clockwise
                            // false = counterclockwise

    while (true) {
        if (clockwise) { position++; }

        
    }
}
