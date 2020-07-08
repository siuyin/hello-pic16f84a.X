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

void main(void) {
    PORTB = 0;
    TRISB = 0;
    TRISB4 = 1;
    RB3 = 1;
    while (1) {
        RB2 = ~RB2;
        RB3 = ~RB3;
        if (RB4 == 0) {
            __delay_ms(150);
            continue;
        }
        __delay_ms(1000);
    }
    return;
}
