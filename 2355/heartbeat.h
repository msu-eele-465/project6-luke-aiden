#include "intrinsics.h"
#include <msp430.h>
#include <stdbool.h>

HeartBeat_init(){
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mdoe to activate

    // init heartbeat LED
    P6DIR |= BIT6;
    P6OUT &= ~BIT6;

    TB0CTL |= TBSSEL__ACLK | ID__1 | MC__UP | TBCLR;

    TB0CCR0 = 32769;    // CCR0=32769

    // Setup Timer Compare IRQ
    TB0CCTL0 &= ~CCIFG;   // Clear CCR0 Flag
    TB0CCTL0 |= CCIE;     // Enable TB0 CCR0 Overflow IRQ

    __enable_interrupt();
}