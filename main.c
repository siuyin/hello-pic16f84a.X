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


// Global variables used by interrupt service routine and main.
const unsigned int led_a_flash_period_ticks = 90;
volatile unsigned int led_a_flash_task_ctr;
const unsigned char led_a_speed_toggle_button_check_period_ticks = 2;
volatile unsigned char led_a_speed_toggle_button_check_ctr;

const unsigned int led_b_flash_period_ticks = 90;
volatile unsigned int led_b_flash_task_ctr;

const unsigned char tmr0_reload_val = 248;
volatile unsigned char tick; // system timer tick

enum speed_toggle_state {
    stON, stOFF
};
enum speed_toggle_state speed_toggle;

void __interrupt() interrupt_service_routine(void) {
    if (T0IE && T0IF) { // timer 0 interrupt enable and interrupt flag
        T0IF = 0; // clear TMR0 interrupt flag

        if (led_a_flash_task_ctr > 0) led_a_flash_task_ctr--;
        if (led_a_speed_toggle_button_check_ctr > 0) led_a_speed_toggle_button_check_ctr--;
        if (led_b_flash_task_ctr > 0) led_b_flash_task_ctr--;

        tick++;

        TMR0 = tmr0_reload_val; // reload TMR0 for next tick
    }
}

// Local function declarations for main.
void setup_port_b(void);
void setup_TMR0_for_interrupts(void);
void flash_LED_a_task(void);
void flash_LED_b_task(void);
void led_a_speed_toggle_button_check_task(void);
void wait_for_next_tick(unsigned char *);

void main(void) {
    setup_port_b();
    led_a_flash_task_ctr = led_a_flash_period_ticks;
    led_a_speed_toggle_button_check_ctr = led_a_speed_toggle_button_check_period_ticks;
    led_b_flash_task_ctr = led_b_flash_period_ticks;
    speed_toggle = stOFF;
    setup_TMR0_for_interrupts();
    ei(); // global interrupt enable

    unsigned char current_tick = 0;
    while (1) {
        led_a_speed_toggle_button_check_task();
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
    TRISB4 = 1; // RB4 is an input
}

void setup_TMR0_for_interrupts(void) {
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
}

#define BTN RB4

enum button_push_state {
    bpPushed, bpReleased, bpMaybeReleased
};
enum button_push_state button_state;
void toggle_speed(void);

void led_a_speed_toggle_button_check_task(void) {
    if (led_a_speed_toggle_button_check_ctr != 0) {
        return;
    }
    switch (button_state) {
        case bpReleased:
            if (BTN == 0) {
                button_state = bpPushed;
                toggle_speed();

            }
            break;
        case bpPushed:
            if (BTN == 0) {
                button_state = bpPushed;
            } else {
                button_state = bpMaybeReleased;
            }
            break;
        case bpMaybeReleased:
            if (BTN == 0) {
                button_state = bpPushed;
            } else {
                button_state = bpReleased;
            }
            break;
    }
}

void toggle_speed(void) {
    if (speed_toggle == stOFF) {
        speed_toggle = stON;
    } else {
        speed_toggle = stOFF;
    }
}

void toggle_LED_a(void);

void flash_LED_a_task(void) {
    if (led_a_flash_task_ctr != 0) {
        return;
    }

    if (speed_toggle == stON) {
        led_a_flash_task_ctr = led_a_flash_period_ticks >> 3; // make it 8 time faster
    } else {
        led_a_flash_task_ctr = led_a_flash_period_ticks;
    }
    toggle_LED_a();
}

void toggle_LED_a(void) {
    RB3 = ~RB3;
}

void toggle_LED_b(void);

void flash_LED_b_task(void) {
    if (led_b_flash_task_ctr != 0) {
        return;
    }
    led_b_flash_task_ctr = led_b_flash_period_ticks;
    toggle_LED_b();
}

void toggle_LED_b(void) {
    RB2 = ~RB2;
}

void wait_for_next_tick(unsigned char * current_tick) {
    while (*current_tick == tick) {
    }
    *current_tick = tick;
}