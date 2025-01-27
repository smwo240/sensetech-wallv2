/**
* PWM test
* Output PWM signals on pins 0 and 1
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

int main() {
    // Turn default LED on to show power is on
    /*const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    */

    // ADC on GPIO 26 / ADC 0
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // GPIO 17 - testing digital I/O
    gpio_init(17);
    gpio_set_dir(17, GPIO_OUT);
    gpio_put(17, 0);

    // Tell GPIO 25 and 16 they are allocated to the PWM
    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(16, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 25, GPIO 16
    uint slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);
    uint slice_num2 = pwm_gpio_to_slice_num(16);
    // set clock divider to 1, then frequency is 125 MHz
    pwm_set_clkdiv(slice_num, 1);
    pwm_set_clkdiv(slice_num2, 1);

    // Set period number of cycles for 100 kHz output
    // f = 125 MHz
    // 
    // period= 125e6/100e3 = 1250
    int period=1250;
    pwm_set_wrap(slice_num, period);
    pwm_set_wrap(slice_num2, period);
    // Set channel A / B output high for a number of cycles
    // PIN 25 is on channel B because its an odd number - see
    // pico forum post
    pwm_set_chan_level(slice_num, PWM_CHAN_B, period*0.1);
    pwm_set_chan_level(slice_num2, PWM_CHAN_A, period*0.1);
    // Set the PWM running
    pwm_set_enabled(slice_num, true);
    pwm_set_enabled(slice_num2, true);
    // Note we could also use pwm_set_gpio_level(gpio, x) which looks up the
    // correct slice and channel for a given GPIO.
    double multiplier = 0.1;
    uint16_t result;
    uint16_t adj_result;

       while (1) { 
      // 12-bit conversion, VREF = 3.3 V  
      // 12-bit value -> val out of 4096
       result = adc_read(); 
       // adj_result = 
      // Set PWM output duty cycle
       pwm_set_chan_level(slice_num, PWM_CHAN_B, period*result/4096);
       pwm_set_chan_level(slice_num2, PWM_CHAN_A, period*result/4096);

        // set motor on/off depending on switch state
        if ((result/4096.0) >= .75)
            gpio_put(17,1);
        else if ((result/4096.0) >= .50)
            gpio_put(17,1);
        else if ((result/4096.0) >= .25)
            gpio_put(17,0);
        else
            gpio_put(17,0);
        // used for cycling through pwm duty cycles
       multiplier += 0.1;

       if (multiplier >= 0.9)
        multiplier = 0.1;

       sleep_ms(100);

        }; // idle loop

}