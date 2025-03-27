#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
//#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

#define INPUT_POT_ADC 26
#define OUTPUT_LED_GPIO 25
#define OUTPUT_MOTORCTRL_GPIO 16
#define MODE_SEL_GPIO 19

#define LED1_GPIO 1
#define LED2_GPIO 2
#define LED3_GPIO 3

int main()
{
    int state, period; 
    uint16_t result;

    // mode select connected to GP19 and GND - closed circuit connects to gnd, open is vcc
    gpio_init(MODE_SEL_GPIO);
    gpio_set_dir(MODE_SEL_GPIO, GPIO_IN);
    gpio_pull_up(MODE_SEL_GPIO); // open - 1, closed - 0
    
    // PWM GPIO
    gpio_set_function(LED1_GPIO, GPIO_FUNC_PWM);
    gpio_set_function(LED2_GPIO, GPIO_FUNC_PWM);
    gpio_set_function(LED3_GPIO, GPIO_FUNC_PWM);
    // Output to motor circuit connected to GPIO16, VCC, GND
    // PWM enabled to control speed of motor - goes into high speed transistor so control VCC input to motor
    gpio_set_function(OUTPUT_MOTORCTRL_GPIO, GPIO_FUNC_PWM);

    uint slice0_num = pwm_gpio_to_slice_num(0); // slice 0
    uint slice1_num = pwm_gpio_to_slice_num(2); // slice 1

    /* LED1: GPIO1, PWM Channel 0B
       LED2: GPIO2, PWM Channel 1A
       LED3: GPIO3, PWM Channel 1B
       MOTOR: GPIO16, PWM Channel 0A
    */

    gpio_init(OUTPUT_LED_GPIO);
    gpio_set_dir(OUTPUT_LED_GPIO, GPIO_OUT);

    // Slide potentiometer connected to VCC, GND, and GPIO26
    // ADC on GPIO26 / ADC0
    adc_init();
    adc_gpio_init(INPUT_POT_ADC);
    adc_select_input(0);


    // set clock divider to 4, then frequency is 125/4 = 31.25 MHz
    // Minimize this to reduce power consumption.
    pwm_set_clkdiv(slice0_num, 4);
    pwm_set_clkdiv(slice1_num, 4);

    // Set period number of cycles for ... Hz output
    // period = Clk/cycle\\.
    // [WIP]
    period = 1250;
    pwm_set_wrap(slice0_num, period);
    pwm_set_wrap(slice1_num, period);

    // Enable PWM running
    //pwm_set_enabled(led1_slice_num, true);
    //pwm_set_enabled(led2_slice_num, true);
    pwm_set_enabled(slice0_num, true);
    pwm_set_enabled(slice1_num, true);

    uint16_t vals_period[6] = {0.4*period,0.6*period,0.9*period,period,0.9*period,0.6*period};
    float led_levels[3][6] = {
        {0, 0.10, 1, 0.15, 0, 0},
        {0, 0, 0.10, 1, 0.15, 0},
        {0, 0, 0, 0.10, 1, 0.15} 
    };

    state = 0;

    while (1) 
    {
        // LED cycle
        pwm_set_gpio_level(LED1_GPIO,led_levels[0][state]*period);
        pwm_set_gpio_level(LED2_GPIO,led_levels[1][state]*period);
        pwm_set_gpio_level(LED3_GPIO,led_levels[2][state]*period);
        // Motor Cycle
        pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO,vals_period[state]);

        //gpio_put(OUTPUT_LED_GPIO, state % 2); // debug - shows program is running

        sleep_ms(400);

        state = (state + 1) % 6;

    }
    return 0;
}
