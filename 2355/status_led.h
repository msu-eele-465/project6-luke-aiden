/*

Logan Graham microcontroller application project 3.

This header file establishes everything needed to run the status RGB LED.

Pins: P2.0 <-> R, P2.1 <-> G, P2.2 <-> B, GND <-> annode
Resistors R-100, G-510, B-1k

Constants:

    - _BITS0TO2: constant with bits 0-2 as 1s and the rest as 0s
    - _PERIOD_MUL: the compare period on the timer is a multiple of 255, this is that multiple

Structures:

    - RGB: members are the RGB Values

Functions:

    - init_status_led_timer: this sets all the values in timer B3 used for the status RGB LED. The
                             input is a pointer to the initial LED color
    - set_status_rgb: this changes the CCR1-3 values to an input pointer RGB value
    - status_led_timer_ccr0: toggles R G and B LEDs on
    - _status_led_timer_ccr1: toggles R LED off
    - _status_led_timer_ccr2: toggles G LED off
    - _status_led_timer_ccr3: toggles G LED off
    - TB3_ISR_call: takes timer interrupt register as input and decides which ccrn was triggered.
                    THen calls ther corresponding _status_led_timer_ccrn


*/

#ifndef _STATUS_LED_H
#define _STATUS_LED_H

#include <msp430.h>


// Constants defitions

#define _BITS0TO2 7
#define _BITS4TO6 112
#define _PERIOD_MUL 4

// Structures definition
struct RGB
{
    int red;
    int green;
    int blue;
};

//typedef struct rgb RGB

// RGB value definitions

struct RGB locked_rgb = {196, 62, 29};
struct RGB unlocking_rgb = {196, 146, 29};
struct RGB unlocked_rgb = {29, 162, 196};

// Function defitions

void init_status_led_timer(struct RGB* init_rgb)
{
    // Set RGB pins as outputs
    P2DIR |= _BITS0TO2;                 // set P2.0-3 to outputs
    P2OUT &= ~_BITS0TO2;                // clear outputs P2.0-3
    
    // Set up timer
    TB3CTL |= TBCLR;                    // clear timers & dividers
    TB3CTL |= TBSSEL__SMCLK;            // source = SMCLK
    TB3CTL |= MC__UP;                   // set count up
    TB3CTL |= ID__1;                    // set divider 1 to 1
    TB3CTL |= CNTL_0;                   // 16-bit clock length

    // set compares 0-3
    TB3CCR0 = _PERIOD_MUL*255;          // set period to approx 1 ms
    TB3CCTL0 |= CCIE;                   // enable compare interrupt
    TB3CCTL0 &= ~CCIFG;                 // clear interrupt flag

    TB3CCR1 = _PERIOD_MUL*(init_rgb->red);// set CCR1 to red duty cycle
    TB3CCTL1 |= CCIE;                   // enable compaxxsre interrupt
    TB3CCTL1 &= ~CCIFG;                 // clear interrupt flag

    TB3CCR2 = _PERIOD_MUL*(init_rgb->green);// set CCR2 to green duty cycle
    TB3CCTL2 |= CCIE;                   // enable compare interrupt
    TB3CCTL2 &= ~CCIFG;                 // clear interrupt flag

    TB3CCR3 = _PERIOD_MUL*(init_rgb->blue);// set CCR3 to blue duty cycle
    TB3CCTL3 |= CCIE;                   // enable compare interrupt
    TB3CCTL3 &= ~CCIFG;                 // clear interrupt flag
    return;
}


void set_status_rgb(struct RGB* color)
{
    TB3CCR1 = _PERIOD_MUL*(color->red); // set CCR1 to red duty cycle
    TB3CCR2 = _PERIOD_MUL*(color->green);// set CCR2 to green duty cycle
    TB3CCR3 = _PERIOD_MUL*(color->blue);// set CCR3 to blue duty cycle
}

void status_led_timer_ccr0(bool locked)
{
    // set all colors high
    P2OUT |= _BITS0TO2;
    if (!locked)
    {
        P4OUT |= _BITS4TO6;
    }
    TB3CCTL0 &= ~CCIFG;                 // clear interrupt flag
    return;
}

void _status_led_timer_ccr1(void)
{
    // set red color to low
    P2OUT &= ~BIT0;
    TB3CCTL1 &= ~CCIFG;                 // clear interrupt flag
    return;
}

void _status_led_timer_ccr2(void)
{
    // set red color to low
    P2OUT &= ~BIT1;
    TB3CCTL2 &= ~CCIFG;                 // clear interrupt flag
    return;
}

void _status_led_timer_ccr3(void)
{
    // set red color to low
    P2OUT &= ~BIT2;
    TB3CCTL3 &= ~CCIFG;                 // clear interrupt flag
    return;
}

void TB3_ISR_call(int IV)
{
    switch (IV) 
    {
        case 2:
            _status_led_timer_ccr1();
            return;

        case 4:
            _status_led_timer_ccr2();
            return;

        case 6:
            _status_led_timer_ccr3();
            return;
    }
}


#endif