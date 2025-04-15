//This code was used to test the functionality of the battery subsystem used within the vibration module. This code runs the motor duty cycle 
//indefinitely until the user stops the test. The purpose of this test is to ensure that the fan will run for 8+ hrs straight to meet an ER set by our team. 
//In the current iteration of the vibration module, the battery passed the necessary test to satisfy the threshold set by our ER.


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "mp3.h"

// Devices GPIO
#define INPUT_POT_ADC 26
#define OUTPUT_LED_GPIO 25
#define OUTPUT_MOTORCTRL_GPIO 16
#define MODE_SEL_GPIO 19

// GPIO Mapping for Set of LEDs
#define LED1_GPIO 1
#define LED2_GPIO 2
#define LED3_GPIO 3

int main()
{
    int state, period;
    int push_state;
    uint16_t result;

    #pragma region audio
    stdio_init_all();
    mp3_initialize();
    mp3_set_volume(16);
    mp3_query_status();
    #pragma endregion
    
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

    /* LED1: GPIO1, PWM Channel 0B
       LED2: GPIO2, PWM Channel 1A
       LED3: GPIO3, PWM Channel 1B
       MOTOR: GPIO16, PWM Channel 0A
    */
    uint slice0_num = pwm_gpio_to_slice_num(0); // slice 0: motor and led1
    uint slice1_num = pwm_gpio_to_slice_num(2); // slice 1: led2 and led3

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
    period = 1250;
    pwm_set_wrap(slice0_num, period);
    pwm_set_wrap(slice1_num, period);

    // Enable PWM running
    pwm_set_enabled(slice0_num, true);
    pwm_set_enabled(slice1_num, true);

    while (1) 
    {
        result = adc_read(); // 12-bit value from 0-3.3V -> 0-4096 int
        printf("%d", result); // debug adc result

        // Use state of mode select to determine behavior - mode 0 for 50% power test, mode 1 for 100% power test
        if (gpio_get(MODE_SEL_GPIO)) {
            pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO, period); // Motor Duty Cycle
            pwm_set_gpio_level(LED1_GPIO, period); // full
            pwm_set_gpio_level(LED2_GPIO, period); // full
            pwm_set_gpio_level(LED3_GPIO, period); // full
            
        }
        else {
            
            pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO, 0.50*period); // Motor Duty Cycle
            pwm_set_gpio_level(LED1_GPIO, 0.50*period); // half
            pwm_set_gpio_level(LED2_GPIO, 0.50*period); // half
            pwm_set_gpio_level(LED3_GPIO, 0.50*period); // half

        }

        sleep_ms(5000);
    }
    
    return 0;
}
