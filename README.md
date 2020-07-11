# Basic timer driven system on PIC16F84A

## System tick
Timer TMR0 is used to generate a 11ms (approx) tick.
The oscillator source is R=6.8 kOhm and C=100 pF for
and RC time constant of 1.36 us.

The instruction cycle period is 4 times greater at 5.44 us.

With a 256x prescaler, TMR0 clocks at 256 * 5.44 us = 1.39 ms.

TMR0 generates an interrupt when it overflows from 255 to 0.
Thus if TMR0 is loaded with a value of 248, it will overflow
in 256-248 = 8 clock periods or 8 * 1.39 ms = 11.1 ms.
This interrupt is the timing source of the system tick.

# Main loop
The main loop comprises independently executing functions.
Each function runs only at its scheduled time and _must not_
block the main loop.

As the main loop may try to execute the function many times
within a system tick, the function tracks the time it last
executed.