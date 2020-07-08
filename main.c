/*
 * File:   main.c
 * Author: siuyin
 *
 * Created on 7 July, 2020, 9:32 PM
 */

// README: The pull-up resistor on MCLR is absolutely required.

// CONFIG
#pragma config FOSC = EXTRC     // Oscillator Selection bits (RC oscillator)
#pragma config WDTE = OFF       // Watchdog Timer (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (Power-up Timer is disabled)
#pragma config CP = OFF         // Code Protection bit (Code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 740000   // 6.8k and 100pF

void setup_port_b(void);
void setup_TMR0_for_interrupts(void);
void toggle_LEDs(void);

void main(void) {
    setup_port_b();
    setup_TMR0_for_interrupts();

    while (1) {
        toggle_LEDs();

        if (RB4 == 0) { // Speed up if button is pushed.
            __delay_ms(150);
            continue;
        }
        __delay_ms(1000);
    }
    return;
}

void setup_port_b(void) {
    // Set port b to output, except for RB4 which is a button input.
    PORTB = 0;
    TRISB = 0;
    TRISB4 = 1;
}

void setup_TMR0_for_interrupts(void) {
    PSA = 1;    // 0: Assign prescaler to TMR0
    PS2 = 0;
    PS1 = 0;
    PS0 = 0;
    TMR0 = 128; // setup for 256 counts before triggering interrrupt.
    T0CS = 1; // TMR0 counter select is set to start counting.
    T0IE = 1; // enable TMR0 interrupt
    ei(); // global interrupt enable
}

void toggle_LEDs(void) {
    // Toggle RB2
    RB2 = ~RB2;
}

void __interrupt() interrupt_service_routine(void) {
    if (T0IE && T0IF) {
        RB3 = ~RB3;
        TMR0 = 128;
        T0IF = 0; // clear TMR0 interrupt flag
        T0IE = 1; // re-enable TMR0 interrupt
    }

}