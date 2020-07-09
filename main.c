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

#define _XTAL_FREQ 740000   // 6.8k and 100pF resitor is 10% and capacitor is 20% tolerance
// I swapped chips and the timing is mostly the same.
// Accuracy is limited by RC values and not chip manufacturing tolerance.

const unsigned int led_a_flash_period_ticks = 360;
volatile unsigned int led_a_flash_task_ctr;

const unsigned int led_b_flash_period_ticks = 360;
volatile unsigned int led_b_flash_task_ctr;

const unsigned char tmr0_reload_val = 248;
volatile unsigned char tick;

void __interrupt() interrupt_service_routine(void) {
    if (T0IE && T0IF) {
        if (led_a_flash_task_ctr > 0) led_a_flash_task_ctr--;
        if (led_b_flash_task_ctr > 0) led_b_flash_task_ctr--;
        tick++;

        TMR0 = tmr0_reload_val;
        T0IF = 0; // clear TMR0 interrupt flag
        T0IE = 1; // re-enable TMR0 interrupt
    }
}

void setup_port_b(void);
void setup_TMR0_for_interrupts(void);
void flash_LED_a_task(void);
void flash_LED_b_task(void);
void wait_for_next_tick(unsigned char *);

void main(void) {
    unsigned char current_tick = 0;

    setup_port_b();
    setup_TMR0_for_interrupts();

    while (1) {
        flash_LED_a_task();
        flash_LED_b_task();

        wait_for_next_tick(&current_tick);
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
    led_a_flash_task_ctr = led_a_flash_period_ticks;
    led_b_flash_task_ctr = led_b_flash_period_ticks;
    tick = 0;

    // example:
    // Fosc = 1/(2RC)
    // Tcyc = 4/Fosc
    // TMR0_t = prescaler * Tcyc
    // If R=6.8k and C=100pF, and prescaler = 256, each TMR0_t = 1.39 ms
    PSA = 0; // 0: Assign prescaler to TMR0
    PS2 = 1; // 001: prescaler set to 2
    PS1 = 1;
    PS0 = 1;

    TMR0 = tmr0_reload_val; // setup for 256 - x counts before triggering interrupt.
    T0CS = 0; // 0: TMR0 counter source is internal clock

    T0IE = 1; // enable TMR0 interrupt
    ei(); // global interrupt enable
}

void toggle_LED_a(void);

void flash_LED_a_task(void) {
    if (led_a_flash_task_ctr == 0) {
        led_a_flash_task_ctr = led_a_flash_period_ticks;
        toggle_LED_a();
    }
}

void toggle_LED_a(void) {
    RB3 = ~RB3;
}

void toggle_LED_b(void);

void flash_LED_b_task(void) {
    if (led_b_flash_task_ctr == 0) {
        led_b_flash_task_ctr = led_b_flash_period_ticks;
        toggle_LED_b();
    }
}

void toggle_LED_b(void) {
    RB2 = ~RB2;
}

void wait_for_next_tick(unsigned char * current_tick) {
    while (*current_tick == tick) {
    }
    *current_tick = tick;
}