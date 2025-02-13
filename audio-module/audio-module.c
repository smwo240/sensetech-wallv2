#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "mp3.h"

// LED PIN
#define LED_PIN 25

// BUTTON LED GPIO
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

// button states, btn# = index + 1
bool btn_active[8] = {false, false, false, false, false, false, false, false};
uint64_t btn_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// delay in microseconds
#define DELAY_US 400000

void gpio_callback(uint gpio, uint32_t events) {
    // Interrupt routine for when a button is pressed on the face plate.
    if (gpio == BTN1_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN1_LED_PIN, true);
        btn_active[0] = true;
        btn_time[0] = time_us_64();  // record timestamp
    } else if (gpio == BTN2_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN2_LED_PIN, true);
        btn_active[1] = true;
        btn_time[1] = time_us_64();  // record timestamp
    } else if (gpio == BTN3_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN3_LED_PIN, true);
        btn_active[2] = true;
        btn_time[2] = time_us_64();  // record timestamp
    } else if (gpio == BTN4_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN4_LED_PIN, true);
        btn_active[3] = true;
        btn_time[3] = time_us_64();  // record timestamp
    } else if (gpio == BTN5_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN5_LED_PIN, true);
        btn_active[4] = true;
        btn_time[4] = time_us_64();  // record timestamp
    } else if (gpio == BTN6_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN6_LED_PIN, true);
        btn_active[5] = true;
        btn_time[5] = time_us_64();  // record timestamp
    } else if (gpio == BTN7_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN7_LED_PIN, true);
        btn_active[6] = true;
        btn_time[6] = time_us_64();  // record timestamp
    } else if (gpio == BTN8_PIN) {
        mp3_play_sound((uint8_t)(rand() % 1));
        gpio_put(BTN8_LED_PIN, true);
        btn_active[7] = true;
        btn_time[7] = time_us_64();  // record timestamp
    } else {
        // error, shouldn't be possible with hardware
    }
}

int main()
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, true);

    // BTN1_LED_PIN
    gpio_init(BTN1_LED_PIN);
    gpio_set_dir(BTN1_LED_PIN, GPIO_OUT);

    // BTN1_PIN
    gpio_init(BTN1_PIN);
    gpio_set_dir(BTN1_PIN,GPIO_IN);
    gpio_pull_up(BTN1_PIN);
    gpio_set_irq_enabled_with_callback(BTN1_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback); // enable interrupts

    // initialize tfplayer
    mp3_initialize();
    mp3_set_volume(30);
    mp3_query_status();

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
