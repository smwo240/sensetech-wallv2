/*
 * File:            vibration-module.c
 * Description:     The main code used to run the functionality of the vibration module as specified.
 * Parameters:      GPIO definitions - the GP# used for the pin where the device is connected to or controlled by the board.
 *                  R# - values used to define the start and end positions of the ranges where the input will change the output.
 *                  MAX_BTWN - the LEDs progressively increase in brightness along with the position of the pushbutton. This determines the MAX brightness
 *                             that the LED can be before going to a 100% brightness when the R# threshhold is passed.
 *                  NUM_STATES - the number of states in the pulse mode. Higher # = smoother transitions between states. 
 *                               If this is modified the arrays holding corresponding values need to be modified as well.
 * Functionality:   Button presses activate the GPIO outputs that control either the fan or bubble motor devices.
 *                  Use the parameters to control time set to ON and to control switch debounce.
 */

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

// POT Range Values
#define R1 0.18
#define R2 0.37
#define R3 0.70

// Push Mode Transition Brightness Maximum
#define MAX_BTWN 0.3

// Pulse Mode Number of States
#define NUM_STATES 12

// normalize values using linear transform
// all values in this project use 0 as min out and 0.3 as max out
// fmax() used to make sure the first section of the equation is greater than 0.1 before passing into the rest of the equation - light flickers at low voltage otherwise
float norm (float x, float min_in, float max_in) {
    // ((x - min_in) / (max_in - min_in)) * (max_out - min_out) + min_out;
    return (fmax((x - min_in - 0.1), 0.0) / (max_in - min_in)) * (MAX_BTWN);
}

int main()
{
    int state, period;
    int push_state;
    int idle_count;
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

    // set clock divider to 4, then frequency is 15.625 MHz (125 MHz / 2^(4-1))
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

    // Constant values used for pulse mode
    uint16_t vals_period[NUM_STATES] = {
        0.4000*period, 0.4292*period, 0.4875*period,
        0.5750*period, 0.6625*period, 0.7208*period,
        0.7500*period, 0.7208*period, 0.6625*period,
        0.5750*period, 0.4875*period, 0.4292*period
    };
    float led_levels[3][NUM_STATES] = {{0.05, 0.10, 0.15, 1.00, 1.00, 0.15, 0.10, 0.05, 0.00, 0.00, 0.00, 0.00},
                                       {0.00, 0.00, 0.05, 0.10, 0.15, 1.00, 1.00, 0.15, 0.10, 0.05, 0.00, 0.00},
                                       {0.00, 0.00, 0.00, 0.00, 0.05, 0.10, 0.15, 1.00, 1.00, 0.15, 0.10, 0.05} 
    };
    state = 0;
    push_state = 0;
    idle_count = 100; // default to idling

    while (1) 
    {
        result = adc_read(); // 12-bit value from 0-3.3V -> 0-4096 int
        printf("%d", result); // debug adc result

        // Use state of mode select to determine behavior
        if (gpio_get(MODE_SEL_GPIO)) {
            // PUSH MODE - VB STRENGTH DETERMINED BY POT POSITION

            // STATE 3: FULL MOTOR OUTPUT, ALL LEDS FULL OUTPUT
            if ((result/4096.0) >= R3) {    
                // GPIO
                pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO, period); // Motor Duty Cycle
                pwm_set_gpio_level(LED1_GPIO, period); // full
                pwm_set_gpio_level(LED2_GPIO, period); // full
                pwm_set_gpio_level(LED3_GPIO, period); // full
                // AUDIO - detect change in vibration level and play sound
                if (push_state != 3) mp3_play_sound(C5);
                push_state = 3; // set vibration level state to 3
                idle_count = 0; // set to NOT IDLE
            }
            // STATE 2: 0.75 DUTY CYCLE MOTOR, LED2LED3 FULL, LED1 VARIABLE
            else if ((result/4096.0) >= R2) {
                pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO, 0.75*period); // Motor Duty Cycle
                // MIN_IN: R2, MAX_IN: R3, MIN_OUT: 0, MAX_OUT: MAX_BTWN
                pwm_set_gpio_level(LED1_GPIO, norm((result/4096.0),R2,R3)*period); // linear transform onto .37 to .70
                pwm_set_gpio_level(LED2_GPIO, period); // full
                pwm_set_gpio_level(LED3_GPIO, period); // full
                // AUDIO - detect change in vibration level and play sound
                if (push_state != 2) mp3_play_sound(D5);
                push_state = 2; // set vibration level state to 2
                idle_count = 0; // set to NOT IDLE
            }
            // STATE 1: 0.5 DUTY CYCLE MOTOR, LED3 FULL, LED2 VARIABLE, LED1 OFF
            else if ((result/4096.0) >= R1) {
                pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO, 0.50*period); // Motor Duty Cycle
                pwm_set_gpio_level(LED1_GPIO, 0.0); // off
                // MIN_IN: R1, MAX_IN: R2, MIN_OUT: 0, MAX_OUT: MAX_BTWN
                pwm_set_gpio_level(LED2_GPIO, norm((result/4096.0),R1,R2)*period); // linear transform onto .18 to .37
                pwm_set_gpio_level(LED3_GPIO, period); // full
                // AUDIO - detect change in vibration level and play sound
                if (push_state != 1) mp3_play_sound(E5);
                push_state = 1; // set vibration level state
                idle_count = 0; // set to NOT IDLE
            }
            // STATE 0: MOTOR OFF, LED3 VARIABLE, LED2LED1 OFF
            else {
                pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO, 0.0); // Motor off
                pwm_set_gpio_level(LED1_GPIO, 0); // off
                pwm_set_gpio_level(LED2_GPIO, 0.0); // off
                // MIN_IN: R0, MAX_IN: R1, MIN_OUT: 0, MAX_OUT: MAX_BTWN
                pwm_set_gpio_level(LED3_GPIO, norm((result/4096.0),0,R1)*period);
                // AUDIO - detect change in vibration level and play sound
                if (push_state != 0) mp3_play_sound(F5);
                push_state = 0; // set vibration level state
                idle_count++; // start counting # states where idle
            }

            // IDLE CHECK - turn LEDs on to show power state when idle
            if (idle_count >= 100) {
                pwm_set_gpio_level(LED3_GPIO, period); // full
                pwm_set_gpio_level(LED2_GPIO, period); // full
                pwm_set_gpio_level(LED1_GPIO, period); // full
            }

            sleep_ms(100); // push mode - 100 ms cycle
        }
        else {
            // PULSE MODE - VB STRENGTH FOLLOWS PATTERN
            // LED cycle
            pwm_set_gpio_level(LED1_GPIO,led_levels[0][state]*period);
            pwm_set_gpio_level(LED2_GPIO,led_levels[1][state]*period);
            pwm_set_gpio_level(LED3_GPIO,led_levels[2][state]*period);
            // Motor Cycle
            pwm_set_gpio_level(OUTPUT_MOTORCTRL_GPIO,vals_period[state]);

            // sleep_ms(100); // Was previously 400 ms for a 6 state cycle - decreased to 200 ms for 12 state cycle
        
            // Sleep time dependent on button position
            if ((result/4096.0) >= R3)
                sleep_ms(150);
            else if ((result/4096.0) >= R2)
                sleep_ms(200);
            else if ((result/4096.0) >= R1)
                sleep_ms(250);
            else
                sleep_ms(300);

            // audio dependent on loop state
            // notes go from C5-F5 then back down F5-C5
            if (state == 0)
                mp3_play_sound(C5);
            else if (state == 2) 
                mp3_play_sound(D5);
            else if (state == 4)
                mp3_play_sound(E5);
            else if (state == 5)
                mp3_play_sound(F5);
            else if (state == 8)
                mp3_play_sound(E5);
            else if (state == 10)
                mp3_play_sound(D5);

            state = (state + 1) % NUM_STATES; // increment state

            }
    }
    
    return 0;
}
