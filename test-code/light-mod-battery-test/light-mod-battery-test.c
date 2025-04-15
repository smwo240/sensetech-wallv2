//This test code is used to ensure that the battery subsystem works within the light module and can run for 8+ hrs as described in the ER for our team.
//Near the end of the code, there is a block to power on the lights indefinitely, which was used to test the duration of battery length. 
//In the current iteration of the air module, the test for the battery passes. 


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

int main()
{
    stdio_init_all();

    mp3_initialize();
    mp3_set_volume(15);
    mp3_query_status();

    // Initialize GPIO - all buttons set up for negative logic on push (pull-up resistors and interrupts on falling edge)

    #pragma region gpio

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

    #pragma endregion

    // Test code - holds a 1 to all GPIO buttons to ensure battery life expectations are met.
    gpio_put(BTN1_LED_PIN,1);
    gpio_put(BTN2_LED_PIN,1);
    gpio_put(BTN3_LED_PIN,1);
    gpio_put(BTN4_LED_PIN,1);
    gpio_put(BTN5_LED_PIN,1);
    gpio_put(BTN6_LED_PIN,1);
    gpio_put(BTN7_LED_PIN,1);
    gpio_put(BTN8_LED_PIN,1);

    while (true) {
        
        // blink onboard LED to ensure program execution has not halted
        gpio_put(25, 0);

        sleep_ms(5000);

        gpio_put(25, 1);

        sleep_ms(5000);

    }
}
