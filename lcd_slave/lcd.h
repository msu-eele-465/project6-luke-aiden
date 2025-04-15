#include "msp430fr2310.h"

void lcd_init(void){
    P1DIR |= BIT0;          //Enable
    P1OUT &= ~BIT0;

    P1DIR |= BIT1;          //Register Select
    P1OUT &= ~BIT1;

    P1DIR |= BIT4;          //DB4
    P1OUT &= ~BIT4; 

    P1DIR |= BIT5;          //DB5
    P1OUT &= ~BIT5;

    P1DIR |= BIT6;          //DB6
    P1OUT &= ~BIT6;

    P1DIR |= BIT7;          //DB7
    P1OUT &= ~BIT7;        

    
}