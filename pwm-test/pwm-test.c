#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "mp3.h"

// enum to use BTN numbers as indices
typedef enum {
    BTN0 = 0,
    BTN1,
    BTN2,
    BTN3,
    BTN4,
    BTN5,
    BTN6,
    BTN7,
    BUTTON_COUNT // 8
} BUTTONS;

const uint BTN_PINS[BUTTON_COUNT] = {34, 32, 31, 29, 27, 26, 25, 24};
const uint LED_PINS[BUTTON_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7};

#define MODE_SEL 9
#define BOARD_LED 25

// Game Logic Variables
bool btn_active[BUTTON_COUNT];  // active when pressed - reset when pattern hits position
bool btn_react[BUTTON_COUNT];   // for collision blinking - not used right now
uint state;         // position of LED pattern
bool clockwise;  // direction pattern should be moving

// Interrupt routine for when a button is pressed on the faceplate.
// When button pressed, turn it on and set to active
// TODO - add collision check in here to avoid timing issues in main loop
//      - adding this will also require modification to main loop im sure there will be bugs with that that can be addressed once observed.
void gpio_callback(uint gpio, uint32_t events) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (gpio == BTN_PINS[i] && !btn_react[i]) {
            if (state == i) {
                // collision_handling(i)
            }
            else {
                gpio_put(LED_PINS[i], true);
                btn_active[i] = true;
                mp3_play_sound(C5); // activation sound
            }
            break;
        }
    }
    return;
}

// Initialize GPIO and interrupts
void gpio_init() {
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        // Initialize Button
        gpio_init(BTN_PINS[i]);
        gpio_set_dir(BTN_PINS[i], GPIO_IN);
        gpio_pull_up(BTN_PINS[i]);
        gpio_set_irq_enabled_with_callback(BTN_PINS[i], GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
        // Initialize LED
        gpio_init(LED_PINS[i]);
        gpio_set_dir(BTN_PINS[i], GPIO_OUT);
        gpio_put(LED_PINS[i], false);
    }

    gpio_init(MODE_SEL);
    gpio_set_dir(MODE_SEL, GPIO_IN);
    gpio_pull_up(MODE_SEL);

    gpio_init(BOARD_LED);
    gpio_set_dir(BOARD_LED, GPIO_OUT);
    gpio_put(BOARD_LED, false);
}

void collision_handling(uint stateS) {
    clockwise = !clockwise;
    btn_active[state] = false; 
    btn_react[state] = true;
    mp3_play_sound(BUBBLEPOP);
    for (int i = 1; i < 7; i++) {
        gpio_put(LED_PINS[state], i % 2 == 0);
        sleep_ms(150);
    }
    btn_react[state] = false;
    gpio_put(LED_PINS[state], true);
}


int main() {

    // Initialize variables
    for (int i = 0; i < BUTTON_COUNT; i++) {
        btn_active[i] = false;
    }
    state = 0;
    clockwise = true;

    stdio_init_all();
    gpio_init();

    while (1) {
        if (!btn_active[state])                                         // previous position
        gpio_put(LED_PINS[state], false);
    
        if (clockwise)                                                  // cw -> increment position
            state = (state + 1) % BUTTON_COUNT;                         // ccw -> decrement position
        else
            state = (state - 1 + BUTTON_COUNT) % BUTTON_COUNT;

        gpio_put(LED_PINS[state], true);                                // light up new position
            
        if (btn_active[state])                                          // new position 
            collision(state);                                           // check for collision with active buttons

        sleep_ms(400);

        gpio_put(BOARD_LED, state % 2 == 0);
    }
}