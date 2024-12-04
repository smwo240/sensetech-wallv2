COMMONLY USED CODE:

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

# Initialize PWM on GPIO17
dac_pin = PWM(Pin(17))  # Use pin GP17 for simulated DAC
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

# Run sine wave generation
generate_sine_wave()



WILL UPDATE WITH MORE SHARED CODE WHEN I COME ACROSS IT, MESSAGE ME WITH QUESTIONS.  
