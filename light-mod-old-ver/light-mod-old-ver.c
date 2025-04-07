#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "mp3.h"

// LED GPIO
#define BTN1_LED_PIN 0
#define BTN2_LED_PIN 1
#define BTN3_LED_PIN 2
#define BTN4_LED_PIN 3
#define BTN5_LED_PIN 4
#define BTN6_LED_PIN 5
#define BTN7_LED_PIN 6
#define BTN8_LED_PIN 7

// BUTTON GPIO
#define BTN1_PIN 28
#define BTN2_PIN 27
#define BTN3_PIN 26
#define BTN4_PIN 22
#define BTN5_PIN 21
#define BTN6_PIN 20
#define BTN7_PIN 19
#define BTN8_PIN 18

// MODE SELECT GPIO
#define MODE_SEL 9

// When a button is pressed, it lights up and stays lit until the position variable reaches the corresponding button and
// then the direction of the position changes. This should work for multiple buttons but we can set a limit on the number of buttons "active".

// global variables
// Button states for light module logic - activated from interrupt handler
// This is idependent of the light being lit up, this is the state that matches when a button has been pressed
// and the looping behavior is waiting for the position to match a btnN_on

// one off of btn# because of initial oversights  btn# = index + 1
bool btn_active[8] = {false, false, false, false, false, false, false, false};

void gpio_callback(uint gpio, uint32_t events) {
    // Interrupt routines for when a button is pressed on the face plate.
    printf("test \r\n");
    if (gpio == BTN1_PIN) {
    if (!btn_active[0]) {
        btn_active[0] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN1_LED_PIN, true);
} 
else if (gpio == BTN2_PIN) {
    if (!btn_active[1]) {
        btn_active[1] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN2_LED_PIN, true);
}
else if (gpio == BTN3_PIN) {
    if (!btn_active[2]) {
        btn_active[2] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN3_LED_PIN, true);
}
else if (gpio == BTN4_PIN) {
    if (!btn_active[3]) {
        btn_active[3] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN4_LED_PIN, true);
}
else if (gpio == BTN5_PIN) {
    if (!btn_active[4]) {
        btn_active[4] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN5_LED_PIN, true);
}
else if (gpio == BTN6_PIN) {
    if (!btn_active[5]) {
        btn_active[5] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN6_LED_PIN, true);
}
else if (gpio == BTN7_PIN) {
    if (!btn_active[6]) {
        btn_active[6] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN7_LED_PIN, true);
}
else if (gpio == BTN8_PIN) {
    if (!btn_active[7]) {
        btn_active[7] = true;
        // play activation sound
        button_press_sound();
    }
    gpio_put(BTN8_LED_PIN, true);
}
    else {
        // error, shouldn't be possible with hardware
        // debug - light up all lights for 2 s

    }
}

// increment notes as number of active buttons present
void button_press_sound() {
    // count active buttons
    uint count = 0;
    for (int i = 0; i < 8; i++) {
        if (btn_active[i] == true)
            count++;
    }
    // count should be at least 1
    switch (count) {
        case 1: mp3_play_sound(C5);
        case 2: mp3_play_sound(D5);
        case 3: mp3_play_sound(E5);
        case 4: mp3_play_sound(F5);
        case 5: mp3_play_sound(G5);
        case 6: mp3_play_sound(A5);
        case 7: mp3_play_sound(B5);
        default: // do nothing
    }
    return;
}

int main()
{
    stdio_init_all();

    mp3_initialize();
    mp3_set_volume(15);
    mp3_query_status();

    // Initialize GPIO - all buttons set up for negative logic on push (pull-up resistors and interrupts on falling edge)

    // BTN1_LED_PIN
    gpio_init(BTN1_LED_PIN);
    gpio_set_dir(BTN1_LED_PIN, GPIO_OUT);
    
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

    // BTN3_LED_PIN
    gpio_init(BTN3_LED_PIN);
    gpio_set_dir(BTN3_LED_PIN, GPIO_OUT);
    // BTN3_PIN
    gpio_init(BTN3_PIN);
    gpio_set_dir(BTN3_PIN, GPIO_IN);
    gpio_pull_up(BTN3_PIN);
    gpio_set_irq_enabled(BTN3_PIN, GPIO_IRQ_EDGE_FALL, true);

    // BTN4_LED_PIN
    gpio_init(BTN4_LED_PIN);
    gpio_set_dir(BTN4_LED_PIN, GPIO_OUT);
    // BTN4_PIN
    gpio_init(BTN4_PIN);
    gpio_set_dir(BTN4_PIN, GPIO_IN);
    gpio_pull_up(BTN4_PIN);
    gpio_set_irq_enabled(BTN4_PIN, GPIO_IRQ_EDGE_FALL, true);

    // BTN5_LED_PIN
    gpio_init(BTN5_LED_PIN);
    gpio_set_dir(BTN5_LED_PIN, GPIO_OUT);
    // BTN5_PIN
    gpio_init(BTN5_PIN);
    gpio_set_dir(BTN5_PIN, GPIO_IN);
    gpio_pull_up(BTN5_PIN);
    gpio_set_irq_enabled(BTN5_PIN, GPIO_IRQ_EDGE_FALL, true);

    // BTN6_LED_PIN
    gpio_init(BTN6_LED_PIN);
    gpio_set_dir(BTN6_LED_PIN, GPIO_OUT);
    // BTN6_PIN
    gpio_init(BTN6_PIN);
    gpio_set_dir(BTN6_PIN, GPIO_IN);
    gpio_pull_up(BTN6_PIN);
    gpio_set_irq_enabled(BTN6_PIN, GPIO_IRQ_EDGE_FALL, true);

    // BTN7_LED_PIN
    gpio_init(BTN7_LED_PIN);
    gpio_set_dir(BTN7_LED_PIN, GPIO_OUT);
    // BTN7_PIN
    gpio_init(BTN7_PIN);
    gpio_set_dir(BTN7_PIN, GPIO_IN);
    gpio_pull_up(BTN7_PIN);
    gpio_set_irq_enabled(BTN7_PIN, GPIO_IRQ_EDGE_FALL, true);

    // BTN8_LED_PIN
    gpio_init(BTN8_LED_PIN);
    gpio_set_dir(BTN8_LED_PIN, GPIO_OUT);
    // BTN8_PIN
    gpio_init(BTN8_PIN);
    gpio_set_dir(BTN8_PIN, GPIO_IN);
    gpio_pull_up(BTN8_PIN);
    gpio_set_irq_enabled(BTN8_PIN, GPIO_IRQ_EDGE_FALL, true);


    // loop to model simple behavior.
    uint32_t position = 1; // position of "light" moving from button to button in circular pattern
    bool clockwise = true;  // true = clockwise
                            // false = counterclockwise

    while (true) {
        
        if (!btn_active[position]) {
            // turn off previous position 
            gpio_put(position, false);

            // do direction swap function - future implmentation
        }
        
        if (clockwise) { 
            position = (position + 1) % 8; }   // iterate clockwise (8 % 8 = 0)
        else { 
            position = (position - 1 + 8) % 8; } // iterate counterclockwise (substitute for -1 % 8 = 7)

        // light up new position
        gpio_put(position, true);

        // ========= Check if button is active at new position =========
        if (btn_active[position]) {
            // play a sound (once configured)

            clockwise = !clockwise; // toggle direction
            btn_active[position] = false;

            mp3_play_sound(CRASH); // collision sound
            // button starts ON if collision occurs
            for (int i = 1; i < 7; i++) {
                gpio_put(position, i % 2 == 0 );
                sleep_ms(150);
            }
            gpio_put(position,true); // backup turn button back on
        }
        else {
            // do nothing, continue to next loop
        }
        
        //mp3_play_sound(C5);

        // gpio_put(BTN1_LED_PIN, true);
        // gpio_put(BTN2_LED_PIN, true);
        // gpio_put(BTN3_LED_PIN, true);
        // gpio_put(BTN4_LED_PIN, true);
        // gpio_put(BTN5_LED_PIN, true);
        // gpio_put(BTN6_LED_PIN, true);
        // gpio_put(BTN7_LED_PIN, true);
        // gpio_put(BTN8_LED_PIN, true);

        if (gpio_get(MODE_SEL))
            sleep_ms(400); // speed of the rotation pattern
        else   
            sleep_ms(300);

    }
}