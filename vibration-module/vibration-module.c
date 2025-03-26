#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

#define INPUT_POT_ADC 26
#define OUTPUT_LED_GPIO 25
#define OUTPUT_MOTORCTRL_GPIO 16
#define MODE_SEL_GPIO 19

#define LED1_GPIO 0
#define LED2_GPIO 1
#define LED3_GPIO 2

int main()
{
    stdio_init_all(); // serial comm for debug

    // mode select connected to GP19 and GND - closed circuit connects to gnd, open is vcc
    gpio_init(MODE_SEL_GPIO);
    gpio_set_dir(MODE_SEL_GPIO, GPIO_IN);
    gpio_pull_up(MODE_SEL_GPIO); // open - 1, closed - 0
    //gpio_set_irq_enabled(BTN6_PIN, GPIO_IRQ_EDGE_FALL, true);

    // LEDs initialization
    gpio_init(LED1_GPIO);
    gpio_set_dir(LED1_GPIO, GPIO_OUT);
    gpio_init(LED2_GPIO);
    gpio_set_dir(LED2_GPIO, GPIO_OUT);
    gpio_init(LED3_GPIO);
    gpio_set_dir(LED3_GPIO, GPIO_OUT);

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

    // WIP - need to determine proper clock divider - i think this is fine

    // Default duty cycle of all PWM outputs to 0
    pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0);
    pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0);

    // Enable PWM running
    pwm_set_enabled(led_slice_num, true);
    pwm_set_enabled(motor_slice_num, true);

    uint16_t result;

    pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0.0);
    while (1) 
    {
        // state machine redesign so that sound logic works better (detect transition from off to on / on to off)

        // 12-bit value from 0-3.3V -> 0-4096
        result = adc_read();
        printf("ADC In: %d", result);
        // pwm_set_chan_level(led_slice_num, PWM_CHAN_B, (result/4096.0)*period);
        
        // set motor and output led pwm output depending on POT position
        
        if ((result/4096.0) >= .70) {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, period);
            /* set LEDs */{
                gpio_put(LED1_GPIO,true);
                gpio_put(LED2_GPIO,true);
                gpio_put(LED3_GPIO,true);
            }
        }    
        else if ((result/4096.0) >= .37) {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0.75*period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0.75*period);
            /* set LEDs */{
                gpio_put(LED1_GPIO,false);
                gpio_put(LED2_GPIO,true);
                gpio_put(LED3_GPIO,true);
            }
        }
        else if ((result/4096.0) >= .18) {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0.5*period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0.5*period);
            /* set LEDs */{
                gpio_put(LED1_GPIO,false);
                gpio_put(LED2_GPIO,false);
                gpio_put(LED3_GPIO,true);
            }
        }
        else {
            pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 0.0*period);
            pwm_set_chan_level(motor_slice_num, PWM_CHAN_A, 0.0*period);
            /* set LEDs */{
                gpio_put(LED1_GPIO,false);
                gpio_put(LED2_GPIO,false);
                gpio_put(LED3_GPIO,false);
            }
        }

        // Set [SOME FEATURE] here using mode select button
        // ...
        if (gpio_get(MODE_SEL_GPIO)) {
            // pwm_set_chan_level(led_slice_num, PWM_CHAN_B, 1.0*period);
        }
        
        sleep_ms(250); // sleep!
    }

}
