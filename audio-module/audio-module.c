/*
 * File:            audio-module.case
 * Description:     The main code used to run the functionality of the audio module as specified.
 * Parameters:      GPIO definitions - the GP# used for the pin where the device is connected to or controlled by the board.
 * Functionality:   Pressing a button on the face marks it as "active" while the output sound is playing.  
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mp3.h"

// LED PIN
#define LED_PIN 25

// BUTTON LED GPIO
#define RED_LED_PIN 18
#define GREEN_LED_PIN 19
#define BLUE_LED_PIN 20
#define MUTE_LED_PIN 21

// BUTTON GPIO
#define BTN1_PIN 2
#define BTN2_PIN 3
#define BTN3_PIN 4
#define BTN4_PIN 5
#define BTN5_PIN 6
#define BTN6_PIN 7
#define BTN7_PIN 8
#define BTN8_PIN 9
const uint BTN_PINS[8] = {BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN, BTN5_PIN, BTN6_PIN, BTN7_PIN, BTN8_PIN};

// SLIDE POTENTIOMETER
#define INPUT_POT_ADC 26

// button states, btn# = index + 1
bool btn_active[8] = {false, false, false, false, false, false, false, false};
uint64_t btn_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// mode
uint8_t mode = 0;

// volume
uint16_t adc;
uint16_t volume;

// delay in microseconds
#define DELAY_US 50000

// gpio callback
void gpio_callback(uint gpio, uint32_t events) {
    // interrupt routine for when a button is pressed on the face plate.
    if (gpio == BTN1_PIN && btn_active[0] == false) {
        switch (mode) {
            case 0: mp3_play_sound(C5); break;
            case 1: mp3_play_sound(CRASH); break;
            case 2: mp3_play_sound(DOG_BARK); break;
            default: mp3_play_sound(C5); break;
        }
        btn_active[0] = true;
        btn_time[0] = time_us_64();  // record timestamp
    } else if (gpio == BTN2_PIN && btn_active[1] == false) {
        switch (mode) {
            case 0: mp3_play_sound(D5); break;
            case 1: mp3_play_sound(HIHAT); break;
            case 2: mp3_play_sound(CAT_MEOW); break;
            default: mp3_play_sound(D5); break;
        }
        btn_active[1] = true;
        btn_time[1] = time_us_64();  // record timestamp
    } else if (gpio == BTN3_PIN && btn_active[2] == false) {
        switch (mode) {
            case 0: mp3_play_sound(E5); break;
            case 1: mp3_play_sound(KICK); break;
            case 2: mp3_play_sound(COW_MOO); break;
            default: mp3_play_sound(E5); break;
        }
        btn_active[2] = true;
        btn_time[2] = time_us_64();  // record timestamp
    } else if (gpio == BTN4_PIN && btn_active[3] == false) {
        switch (mode) {
            case 0: mp3_play_sound(F5); break;
            case 1: mp3_play_sound(CLAP); break;
            case 2: mp3_play_sound(ROOSTER_CROW); break;
            default: mp3_play_sound(F5); break;
        }
        btn_active[3] = true;
        btn_time[3] = time_us_64();  // record timestamp
    } else if (gpio == BTN5_PIN && btn_active[4] == false) {
        switch (mode) {
            case 0: mp3_play_sound(G5); break;
            case 1: mp3_play_sound(SNARE); break;
            case 2: mp3_play_sound(HORSE_NEIGH); break;
            default: mp3_play_sound(G5); break;
        }
        btn_active[4] = true;
        btn_time[4] = time_us_64();  // record timestamp
    } else if (gpio == BTN6_PIN && btn_active[5] == false) {
        switch (mode) {
            case 0: mp3_play_sound(A5); break;
            case 1: mp3_play_sound(TOM); break;
            case 2: mp3_play_sound(FART); break;
            default: mp3_play_sound(A5); break;
        }
        btn_active[5] = true;
        btn_time[5] = time_us_64();  // record timestamp
    } else if (gpio == BTN7_PIN && btn_active[6] == false) {
        mode = (mode + 1) % 3; // mode select

        switch (mode) {
            case 0: mp3_play_sound(A5); gpio_put(RED_LED_PIN, true); gpio_put(BLUE_LED_PIN, false); gpio_put(GREEN_LED_PIN, false); break;
            case 1: mp3_play_sound(SNARE); gpio_put(RED_LED_PIN, false); gpio_put(BLUE_LED_PIN, true); gpio_put(GREEN_LED_PIN, false); break;
            case 2: mp3_play_sound(CAT_MEOW); gpio_put(RED_LED_PIN, false); gpio_put(BLUE_LED_PIN, false); gpio_put(GREEN_LED_PIN, true); break;
            default: mp3_play_sound(A5); break;
        }

        btn_active[6] = true;
        btn_time[6] = time_us_64();  // record timestamp
    } else if (gpio == BTN8_PIN && btn_active[7] == false) {
        // mute(?)

        btn_active[7] = true;
        btn_time[7] = time_us_64();  // record timestamp
    } else {
        // error, shouldn't be possible with hardware
    }
}

// setup pins
void setup_pins() {
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_init(MUTE_LED_PIN);
    gpio_set_dir(MUTE_LED_PIN, GPIO_OUT);

    for (int i = 0; i < 8; i++) {
        gpio_init(BTN_PINS[i]);
        gpio_set_dir(BTN_PINS[i], GPIO_IN);
        gpio_pull_up(BTN_PINS[i]);
        if (i == 0) {
            gpio_set_irq_enabled_with_callback(BTN1_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
        }
        else gpio_set_irq_enabled(BTN_PINS[i], GPIO_IRQ_EDGE_FALL, true);
    }
}

int main()
{
    stdio_init_all();

    // LED pin setup
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, true);

    // button pin setup
    setup_pins();

    // slide potentiometer connected to VCC (1), GND (3), and GPIO26 (2)
    // ADC on GPIO26 / ADC0
    adc_init();
    adc_gpio_init(INPUT_POT_ADC);
    adc_select_input(0);

    // initialize tfplayer
    mp3_initialize();
    // mp3_set_volume(15);
    mp3_query_status();

    gpio_put(RED_LED_PIN, true);

    while (true) {
        if (btn_active[0] == true && (time_us_64() - btn_time[0] >= DELAY_US)) {
            btn_active[0] = false;
        }
        if (btn_active[1] == true && (time_us_64() - btn_time[1] >= DELAY_US)) {
            btn_active[1] = false;
        }
        if (btn_active[2] == true && (time_us_64() - btn_time[2] >= DELAY_US)) {
            btn_active[2] = false;
        }
        if (btn_active[3] == true && (time_us_64() - btn_time[3] >= DELAY_US)) {
            btn_active[3] = false;
        }
        if (btn_active[4] == true && (time_us_64() - btn_time[4] >= DELAY_US)) {
            btn_active[4] = false;
        }
        if (btn_active[5] == true && (time_us_64() - btn_time[5] >= DELAY_US)) {
            btn_active[5] = false;
        }
        if (btn_active[6] == true && (time_us_64() - btn_time[6] >= DELAY_US)) {
            btn_active[6] = false;
        }
        if (btn_active[7] == true && (time_us_64() - btn_time[7] >= DELAY_US)) {
            // mute (?)

            btn_active[7] = false;
        }

        adc = adc_read();
        uint16_t adc_to_volume = (adc / 4096.0) * 30;
        if (adc_to_volume != volume) {
            volume = adc_to_volume;
            mp3_set_volume(volume);
            
            if (volume == 0) gpio_put(MUTE_LED_PIN, true);
            else gpio_put(MUTE_LED_PIN, false);
        }

        sleep_ms(400);
    }
}
