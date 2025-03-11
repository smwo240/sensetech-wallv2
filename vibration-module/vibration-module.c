#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

#define INPUT_POT_ADC 26
#define OUTPUT_LED_GPIO 25
#define OUTPUT_MOTORCTRL_GPIO 16

int main()
{
    // Slide potentiometer connected to VCC, GND, and GPIO26
    // ADC on GPIO26 / ADC0
    adc_init();
    adc_gpio_init(INPUT_POT_ADC);
    adc_select_input(0);

    // Output LED connected to GPIO17 and GND
    // PWM enabled to dim and brighten along with ADC input
    gpio_set_function(OUTPUT_LED_GPIO, GPIO_FUNC_PWM);
    uint led_slice_num = pwm_gpio_to_slice_num(OUTPUT_LED_GPIO);

    // Output to motor circuit connected to GPIO17, VCC, GND
    // PWM enabled to control speed of motor - goes into high speed transistor so control VCC input to motor
    gpio_set_function(OUTPUT_MOTORCTRL_GPIO, GPIO_FUNC_PWM);
    uint motor_slice_num = pwm_gpio_to_slice_num(OUTPUT_MOTORCTRL_GPIO);

    // set clock divider to 4, then frequency is 125/4 = 31.25 MHz
    // Minimize this to reduce power consumption. [WIP]
    pwm_set_clkdiv(led_slice_num, 4);
    pwm_set_clkdiv(motor_slice_num, 4);

    // Set period number of cycles for ... Hz output
    // period = Clk/cycle\\.
    // [WIP]
    int period = 1250;
    pwm_set_wrap(led_slice_num, period);
    pwm_set_wrap(motor_slice_num, period);

    // WIP - need to determine proper clock divider

    // Default duty cycle of all PWM outputs to 0
    pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0);
    pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0);

    // Enable PWM running
    pwm_set_enabled(led_slice_num, true);
    pwm_set_enabled(motor_slice_num, true);

    uint16_t result;

    while (1) 
    {
        // 12-bit value from 0-3.3V -> 0-4096
        result = adc_read();

        // set motor and output led pwm output depending on POT position
        if ((result/4096.0) >= .75) {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, period);
        }    
        else if ((result/4096.0) >= .40) {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0.75*period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0.75*period);
        }
        else if ((result/4096.0) >= .20) {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0.5*period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0.5*period);
        }
        else {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0.0*period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0.0*period);
        }
    }

}
