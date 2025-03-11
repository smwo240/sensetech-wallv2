#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main() {
    // Initialize the standard I/O for serial output
    stdio_init_all();
    
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
    }

    return 0;
}
