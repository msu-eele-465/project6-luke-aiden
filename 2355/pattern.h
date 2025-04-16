/*

Luke Johnson microcontroller application project 3.

This file is a header that includes all declarations for using the LED Bar. All pins on Port 1 will be used to operate the LED Bar.

Pins: uC: P1.i <-> i+1 LED Bar

Constants:
    - integer counter
    - integer down 256 counter1

Functions:
    - _init_LED_bar()   to initialize port1 pins 
    - __pattern0()      not present here, hardcoded in led_pattern.c
    - __pattern1()      toggle all LEDs
    - __pattern2()      256 bit binary up counter
    - __pattern3()      256 bit binary down counter 
    - __pattern4()      in and out pattern
    - __pattern5()      rotate left pattern
    - __pattern6()      rotate 0 right pattern
    - __pattern7()      fill left

*/

#ifndef _LED_BAR_H
#define _LED_BAR_H

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

// constants
#define QUATER_SECOND 8192

// globals
int counter[] = {0, 0, 0, 0, 256, 0, 0, 0};
unsigned int period = 32768;
uint8_t outPut = 0;

inline void _decrease_speed(void) {
        // if "A" is pressed decrease cycle speed by 0.25s
        if (period < 24577)
        {
        period += 8192;
        }
        return;
}

inline void _increase_speed(void) {
        // if "B" is pressed increase cycle speed by 0.25s
        if (period > 8193)
        {
        period -= 8192;
        }
        
        return;
}

inline uint8_t get_outPut(){
    return outPut;
}

inline void clear_led_bar(void)
{
    outPut &= ~BIT0;  // Toggle LED 1
    outPut &= ~BIT1;  // Toggle LED 2
    outPut &= ~BIT2;  // Toggle LED 3
    outPut &= ~BIT3;  // Toggle LED 4
    outPut &= ~BIT4;  // Toggle LED 5
    outPut &= ~BIT5;  // Toggle LED 6
    outPut &= ~BIT6;  // Toggle LED 7
    outPut &= ~BIT7;  // Toggle LED 8
}

inline void __pattern0(void) { // Pattern 0

    outPut |= BIT0; // Set values 
    outPut &= ~BIT1;
    outPut |= BIT2;
    outPut &= ~BIT3;
    outPut |= BIT4;
    outPut &= ~BIT5;
    outPut |= BIT6;
    outPut &= ~BIT7;

    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;
}
inline void __pattern1(void) {
    outPut ^= BIT0;  // Toggle LED 1
    outPut ^= BIT1;  // Toggle LED 2
    outPut ^= BIT2;  // Toggle LED 3
    outPut ^= BIT3;  // Toggle LED 4
    outPut ^= BIT4;  // Toggle LED 5
    outPut ^= BIT5;  // Toggle LED 6
    outPut ^= BIT6;  // Toggle LED 7
    outPut ^= BIT7;  // Toggle LED 8

    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;
}

inline void __pattern2(void) {
    (counter[2])++; 
    outPut = counter[2];
    
    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;
}

inline void __pattern3(void) {
    (counter[3])++;

    switch(counter[3]) {
        case 1: outPut |= BIT3;
                outPut |= BIT4;
                break;
        case 2: outPut &= ~BIT3;
                outPut &= ~BIT4;
                outPut |= BIT2;
                outPut |= BIT5;
                break;
        case 3: outPut &= ~BIT2;
                outPut &= ~BIT5;
                outPut |= BIT1;
                outPut |= BIT6;
                break;
        case 4: outPut &= ~BIT1;
                outPut &= ~BIT6;
                outPut |= BIT0;
                outPut |= BIT7;
                break;
        case 5: outPut &= ~BIT0;
                outPut &= ~BIT7;
                outPut |= BIT1;
                outPut |= BIT6;
                break;
        case 6: outPut &= ~BIT1;
                outPut &= ~BIT6;
                outPut |= BIT2;
                outPut |= BIT5;
                break;
        case 7: outPut &= ~BIT2;
                outPut &= ~BIT5;
                outPut |= BIT3;
                outPut |= BIT4;
                counter[3] = 0;
                break;
    }

    
    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;
}

inline void __pattern4(void) {
    //int counter1 = 256; do this somewhere else
    (counter[4])--;
    outPut = counter[4];
    
    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;

}

inline void __pattern5(void) {
    (counter[5])++;
    switch(counter[5]) {
        case 1: outPut |= BIT7;
                outPut &= ~BIT0;
                break;
        case 2: outPut |= BIT6;
                outPut &= ~BIT7;
                break;
        case 3: outPut |= BIT5;
                outPut &= ~BIT6;
                break;
        case 4: outPut |= BIT4;
                outPut &= ~BIT5;
                break;
        case 5: outPut |= BIT3;
                outPut &= ~BIT4;
                break;
        case 6: outPut |= BIT2;
                outPut &= ~BIT3;
                break;
        case 7: outPut |= BIT1;
                outPut &= ~BIT2;
                break;
        case 8: outPut |= BIT0;
                outPut &= ~BIT1;
                counter[5] = 0;
                break;
    }
    
    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;
}

inline void __pattern6(void) {
    (counter[6])++;
    switch(counter[6]) {
        case 1: outPut &= ~BIT1;
                outPut |= BIT0;
                break;
        case 2: outPut &= ~BIT2;
                outPut |= BIT1;
                break;
        case 3: outPut &= ~BIT3;
                outPut |= BIT2;
                break;
        case 4: outPut &= ~BIT4;
                outPut |= BIT3;
                break;
        case 5: outPut &= ~BIT5;
                outPut |= BIT4;
                break;
        case 6: outPut &= ~BIT6;
                outPut |= BIT5;
                break;
        case 7: outPut &= ~BIT7;
                outPut |= BIT6;
                break;
        case 8: outPut &= ~BIT0;
                outPut |= BIT7;
                counter[6] = 0;
                break;
    }
    
    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;
}

inline void __pattern7(void) {
    (counter[7])++;
    switch(counter[7]) {
        case 1: outPut |= BIT6;
                break;
        case 2: outPut |= BIT5;
                break;
        case 3: outPut |= BIT4;
                break;
        case 4: outPut |= BIT3;
                break;
        case 5: outPut |= BIT2;
                break;
        case 6: outPut |= BIT1;
                break;
        case 7: outPut |= BIT0;
                break;
        case 8: outPut &= ~BIT0;
                outPut &= ~BIT1;
                outPut &= ~BIT2;
                outPut &= ~BIT3;
                outPut &= ~BIT4;
                outPut &= ~BIT5;
                outPut &= ~BIT6;
                outPut |= BIT7;
                counter[7] = 0;
    }
    
    TB0CCTL0 &= ~CCIFG; //Clear CCR0 Flag
    return;
}

inline int pattern_decide(int prev_pattern, int pattern)
{
    if (pattern == prev_pattern)
    {
        switch (pattern)
        {
            case 0:
                TB0CCR0 = period;
                __pattern0();
                break;
            case 1:
                TB0CCR0 = period;
                __pattern1();
                break;
            case 2:
                TB0CCR0 = period >> 1;
                __pattern2();
                break;
            case 3:
                TB0CCR0 = period >> 1;
                __pattern3();
                break;
            case 4:
                TB0CCR0 = period >> 2;
                __pattern4();
                break;
            case 5:
                TB0CCR0 = (int)(period*1.5);
                __pattern5();
                break;
            case 6:
                TB0CCR0 = period >> 1;
                __pattern6();
                break;
            case 7:
                TB0CCR0 = period;
                __pattern7();
                break;

        }
    }
    else 
    {
        clear_led_bar();

        switch (pattern)
        {
            case 0:
                TB0CCR0 = period;
                __pattern0();
                break;
            case 1:
                TB0CCR0 = period;
                outPut |= BIT0; // Set values 
                outPut &= ~BIT1;
                outPut |= BIT2;
                outPut &= ~BIT3;
                outPut |= BIT4;
                outPut &= ~BIT5;
                outPut |= BIT6;
                outPut &= ~BIT7;
                __pattern1();
                break;
            case 2:
               TB0CCR0 = period >> 1;
                __pattern2();
                break;
            case 3:
                TB0CCR0 = period >> 1;
                __pattern3();
                break;
            case 4:
                TB0CCR0 = period >> 2;
                __pattern4();
                break;
            case 5:
                TB0CCR0 = (int)(period*1.5);
                __pattern5();
                break;
            case 6:
                TB0CCR0 = period >> 1;
                outPut &= ~BIT0; // Set Outputs
                outPut |= BIT1;
                outPut |= BIT2;
                outPut |= BIT3;
                outPut |= BIT4;
                outPut |= BIT5;
                outPut |= BIT6;
                outPut |= BIT7;
                __pattern6();
                break;
            case 7:
                TB0CCR0 = period;
                __pattern7();
                break;

        }
    }
    return pattern;
}

#endif 