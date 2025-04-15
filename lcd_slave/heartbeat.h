#include "intrinsics.h"
#include <msp430.h>
#include <stdbool.h>

HeartBeat_init(){
    // init heartbeat LED
    P2DIR |= BIT0;
    P2OUT &= ~BIT0;


    // Setup Timer
    TB0CTL |= TBCLR;    // clear timers & dividers
    TB0CTL |= TBSSEL__ACLK; // Source = ACLK
    TB0CTL |= MC__UP;   // Mode=UP
    TB0CCR0 = 32769;    // CCR0=32769

    // Setup Timer Compare IRQ
    TB0CCTL0 &= ~CCIFG;   // Clear CCR0 Flag
    TB0CCTL0 |= CCIE;     // Enable TB0 CCR0 Overflow IRQ

}