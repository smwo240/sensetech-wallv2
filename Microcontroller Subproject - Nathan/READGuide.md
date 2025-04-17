COMMONLY USED CODE:
# This readme file should be used to document standard uses of functions across all of the individual modules and provide guidance for future work to be done in a standardized manner. 

# MicroPython notes (DISCONTINUED, only affects following two functions, USING C/C++)
# Make sure to have Micro-Pico extenstion installed on VS Code: https://marketplace.visualstudio.com/items?itemName=paulober.pico-w-go
# Updated final test code is placed in a seperate folder in our repo.

"import time": every single program will need to import a clock, regardless of function. 
"from machine import ADC/Pin, PWM": If ADC, import ADC from machine. If using PWM, import Pin, PWM. 

Use GP26 uniformly for ADC input:
if "import ADC"  is included, then can use defined "adc" variable.
"adc = ADC(26)"

Use GP16/17 uniformly for PWM input:
"pwm_pin = PWM(Pin(16/17))"


*There is no built-in DAC features of the Raspberry Pi Pico. Must use PWM to mimick effect of DAC.
Example of this can be seen below:

    from machine import Pin, PWM

    import time
    import math

# Initialize PWM on GPIO16
    dac_pin = PWM(Pin(16))  # Use pin GP17 for simulated DAC
    dac_pin.freq(1000)      # Set PWM frequency to 1 kHz

# Function to generate a sine wave

    def generate_sine_wave(steps=100, amplitude=32767):
    """
    Generates a sine wave using PWM to simulate DAC.
    
    :param steps: Number of steps for the sine wave (resolution).
    :param amplitude: Amplitude of the sine wave (max duty cycle value).
    """
    while True:
        for i in range(steps):
            # Calculate sine wave value, scale to PWM range (0-65535)
            value = int((math.sin(i / steps * 2 * math.pi) + 1) * amplitude)
            dac_pin.duty_u16(value)  # Set duty cycle
            time.sleep(0.01)  # Delay between steps

# NOTE: moved on from MicroPython and Thonny IDE, using C/C++ and Visual Studio for flexibility

# ADC example in C (used across modules):
Ex. uses standard GPIO for ADC input (GPIO26), can be seen in all example below. 

    // Initialize the ADC hardware
    adc_init();
    
    // Select ADC input 0 (GPIO26)
    adc_gpio_init(26);
    adc_select_input(0);

    while (true) {
        // Read raw ADC value (12-bit, 0-4095)
        uint16_t adc_value = adc_read();
        
        // Convert raw ADC value to voltage (assuming 3.3V reference)
        float voltage = (adc_value * 3.3f) / 4095.0f;
        
        // Print the raw ADC value and calculated voltage
        printf("ADC Value: %d, Voltage: %.2f V\n", adc_value, voltage);
        
        // Delay for readability (1 second)
        sleep_ms(1000);

    
Example: Here is actual function used in air module that takes inspiration from above ADC code. The function implements ADC through the use of a slide potentiomenter:
  
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

Example 2: Here is actual function used in vibration module that takes inspiration from above ADC code. The function implements ADC through the use of a slide potentiomenter also:

    stdio_init_all(); // serial comm for debug

    // Slide potentiometer connected to VCC, GND, and GPIO26
    // ADC on GPIO26 / ADC0
    adc_init();
    adc_gpio_init(INPUT_POT_ADC);
    adc_select_input(0);

This vibration module example further uses PWM to dim and brighten LEDs in conjunction with the ADC input. 


# GPIO usage across modules
GPIO designation across modules should be standardized, and this should serve as some documentation for that. 

The Light module assigns GPIO pins directly with buttons within the code:

    void gpio_callback(uint gpio, uint32_t events) {
    // Interrupt routines for when a button is pressed on the face plate.
    printf("Interrupt triggered");
    if (gpio == BTN1_PIN) {
    if (!btn_active[0]) {
        btn_active[0] = true;
        mp3_play_sound(C5); // play activation sound (?)
    }
    gpio_put(BTN1_LED_PIN, true);
    } 
    else if (gpio == BTN2_PIN) {
    if (!btn_active[1]) {
        btn_active[1] = true;
        mp3_play_sound(D5); // play activation sound
    }
    gpio_put(BTN2_LED_PIN, true);
    }
    else if (gpio == BTN3_PIN) {
    if (!btn_active[2]) {
        btn_active[2] = true;
        mp3_play_sound(E5); // play activation sound
    }
    gpio_put(BTN3_LED_PIN, true);
    }
    else if (gpio == BTN4_PIN) {
    if (!btn_active[3]) {
        btn_active[3] = true;
        mp3_play_sound(F5); // play activation sound
    }
    gpio_put(BTN4_LED_PIN, true);
    }
    else if (gpio == BTN5_PIN) {
    if (!btn_active[4]) {
        btn_active[4] = true;
        mp3_play_sound(G5); // play activation sound
    }
    gpio_put(BTN5_LED_PIN, true);
    }
    else if (gpio == BTN6_PIN) {
    if (!btn_active[5]) {
        btn_active[5] = true;
        mp3_play_sound(A5); // play activation sound
    }
    gpio_put(BTN6_LED_PIN, true);
    }
    else if (gpio == BTN7_PIN) {
    if (!btn_active[6]) {
        btn_active[6] = true;
        mp3_play_sound(B5); // play activation sound
    }
    gpio_put(BTN7_LED_PIN, true);
    }
    else if (gpio == BTN8_PIN) {
    if (!btn_active[7]) {
        btn_active[7] = true;
        mp3_play_sound(C6); // play activation sound
    }
    gpio_put(BTN8_LED_PIN, true);
    }
    else {
        // error, shouldn't be possible with hardware
        // debug - light up all lights for 2 s

    }
    }

This is possible due to all the GPIO pin options on the Raspberry Pi Pico, but if you have more than 9 GPIO connections, this will not be feasible. 

The Audio module assigns "buttons" to GPIO in a similar way:

    void gpio_callback(uint gpio, uint32_t events) {
    // Interrupt routine for when a button is pressed on the face plate.
    if (gpio == BTN1_PIN) {
        mp3_play_sound(C5);
        gpio_put(BTN1_LED_PIN, true);
        btn_active[0] = true;
        btn_time[0] = time_us_64();  // record timestamp
    } else if (gpio == BTN2_PIN) {
        mp3_play_sound(D5);
        gpio_put(BTN2_LED_PIN, true);
        btn_active[1] = true;
        btn_time[1] = time_us_64();  // record timestamp
    } else if (gpio == BTN3_PIN) {
        mp3_play_sound(E5);
        gpio_put(BTN3_LED_PIN, true);
        btn_active[2] = true;
        btn_time[2] = time_us_64();  // record timestamp
    } else if (gpio == BTN4_PIN) {
        mp3_play_sound(F5);
        gpio_put(BTN4_LED_PIN, true);
        btn_active[3] = true;
        btn_time[3] = time_us_64();  // record timestamp
    } else if (gpio == BTN5_PIN) {
        mp3_play_sound(G5);
        gpio_put(BTN5_LED_PIN, true);
        btn_active[4] = true;
        btn_time[4] = time_us_64();  // record timestamp
    } else if (gpio == BTN6_PIN) {
        mp3_play_sound(A5);
        gpio_put(BTN6_LED_PIN, true);
        btn_active[5] = true;
        btn_time[5] = time_us_64();  // record timestamp
    } else if (gpio == BTN7_PIN) {
        mp3_play_sound(B5);
        gpio_put(BTN7_LED_PIN, true);
        btn_active[6] = true;
        btn_time[6] = time_us_64();  // record timestamp
    } else if (gpio == BTN8_PIN) {
        mp3_play_sound(C6);
        gpio_put(BTN8_LED_PIN, true);
        btn_active[7] = true;
        btn_time[7] = time_us_64();  // record timestamp
    } else {
        // error, shouldn't be possible with hardware
    }
    }

Additionally, the GPIO is tied to mp3 sound playing functionality as well while associated buttons are pressed: 

    while (true) {
        if (btn_active[0] == true && (time_us_64() - btn_time[0] >= DELAY_US)) {
            gpio_put(BTN1_LED_PIN, false);
            btn_active[0] = false;
        }
        if (btn_active[1] == true && (time_us_64() - btn_time[1] >= DELAY_US)) {
            gpio_put(BTN2_LED_PIN, false);
            btn_active[1] = false;
        }
        if (btn_active[2] == true && (time_us_64() - btn_time[2] >= DELAY_US)) {
            gpio_put(BTN3_LED_PIN, false);
            btn_active[2] = false;
        }
        if (btn_active[3] == true && (time_us_64() - btn_time[3] >= DELAY_US)) {
            gpio_put(BTN4_LED_PIN, false);
            btn_active[3] = false;
        }
        if (btn_active[4] == true && (time_us_64() - btn_time[4] >= DELAY_US)) {
            gpio_put(BTN5_LED_PIN, false);
            btn_active[4] = false;
        }
        if (btn_active[5] == true && (time_us_64() - btn_time[5] >= DELAY_US)) {
            gpio_put(BTN6_LED_PIN, false);
            btn_active[5] = false;
        }
        if (btn_active[6] == true && (time_us_64() - btn_time[6] >= DELAY_US)) {
            gpio_put(BTN7_LED_PIN, false);
            btn_active[6] = false;
        }
        if (btn_active[7] == true && (time_us_64() - btn_time[7] >= DELAY_US)) {
            gpio_put(BTN8_LED_PIN, false);
            btn_active[7] = false;
        }

        sleep_ms(40);
    }
}

The Air module uses GPIO initialization for the LED color and the bubble blower pins:

     // Initialize LEDs
    gpio_init(LED_RED_PIN);
    gpio_init(LED_YELLOW_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_YELLOW_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    // Initialize bubble blower pin
    gpio_init(BUBBLE_BLOWER_PIN);
    gpio_set_dir(BUBBLE_BLOWER_PIN, GPIO_OUT);
    gpio_put(BUBBLE_BLOWER_PIN, 0); // Ensure blower is off at start

    // Initialize button
    gpio_init(BUBBLE_BUTTON_PIN);
    gpio_set_dir(BUBBLE_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUBBLE_BUTTON_PIN); // Pull-up resistor for stable input
    

Please reference the Raspberry Pi Pico datasheet to understand the necessary GPIO connections on the microcontroller. 

# PWM usage
PWM is used to simulate DAC and other functions on the microcontroller. USE GPIO16 for PWM functions so that the use case is standard across modules.

Example implementation:

    #define PWM_PIN 16  // GPIO16 for PWM output

    // Function to set the PWM duty cycle (0-65535)
    void set_pwm_duty_cycle(uint16_t duty_cycle) {
    pwm_set_gpio_level(PWM_PIN, duty_cycle); // Set PWM duty cycle
    }

    // Function to initialize PWM
    void init_pwm() {
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);  // Set GPIO as PWM
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Get PWM slice number

    pwm_set_wrap(slice_num, 65535);  // Set 16-bit resolution (0-65535)
    pwm_set_clkdiv(slice_num, 1.0);  // Set clock divider (adjustable for speed)
    pwm_set_enabled(slice_num, true); // Enable PWM
    }

    // Function to control brightness smoothly
    void pwm_brightness_control() {
    while (true) {
        // Gradually increase brightness
        for (uint16_t duty = 0; duty <= 65535; duty += 1000) {
            set_pwm_duty_cycle(duty);
            sleep_ms(10);  // Small delay for smooth transition
        }

        // Gradually decrease brightness
        for (uint16_t duty = 65535; duty > 0; duty -= 1000) {
            set_pwm_duty_cycle(duty);
            sleep_ms(10);
        }
    }
    }

I derived a PWM test that can be plugged and played across modules below:

    /**
    * PWM test
    * Output PWM signals on pins 0 and 1
     */

    #include "pico/stdlib.h"
    #include "hardware/pwm.h"
    #include "hardware/adc.h"
    #include "hardware/clocks.h"
    #include <stdio.h>

    int main() {
    // Turn default LED on to show power is on
    /*const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    */

    stdio_init_all();

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
    char str[50];

       while (1) { 
      // 12-bit conversion, VREF = 3.3 V  
      // 12-bit value -> val out of 4096
       result = adc_read(); 
       // adj_result = 
      // Set PWM output duty cycle
       pwm_set_chan_level(slice_num, PWM_CHAN_B, period*multiplier/4096);
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

        sprintf(str,"%lf",multiplier);
        printf(str);

       sleep_ms(100);

        }; // idle loop infinite

    }

# NEED TO ADD SPECIFIC MODULE USE CASES OF PWM 
    

WILL UPDATE WITH MORE SHARED CODE WHEN I COME ACROSS IT, MESSAGE ME WITH FAQ.  

# Relavent code I directly wrote or assisted in writing:
vibration-module.c
pwm-test.c
light-module.c
audio-module.c
newairmodule.c
PWMEx.c
DACconversionsex.c
LightModuleEx.c
Airmoduleexample.c

additionally, every file in the Microcontroller Subproject - Nathan folder has been fully authored by Nathan Sanders. 

