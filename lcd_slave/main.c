#include "intrinsics.h"
#include "msp430fr2310.h"
#include "heartbeat.h"
#include "stdint.h"
#include "lcd.h"

int temp = 0;
char thousands;
char hundreds;
char tens;
char ones;


//---------------LCD Functions-------------------------
void pulse_enable(){
    P1OUT |= BIT0;           // E = 1
    __delay_cycles(50);      // Hold E high for a short time
    P1OUT &= ~BIT0;          // E = 0
    //__delay_cycles(50);      
}


void write_nibble(uint8_t data){
    if(data & BIT0){
        P1OUT |= BIT4;
    }else{
        P1OUT &= ~BIT4;
    }

    if(data & BIT1){
        P1OUT |= BIT5;
    }else{
        P1OUT &= ~BIT5;
    }

    if(data & BIT2){
        P1OUT |= BIT6;
    }else{
        P1OUT &= ~BIT6;
    }

    if(data & BIT3){
        P1OUT |= BIT7;
    }else{
        P1OUT &= ~BIT7;
    }    
}

void lcd_start(void){
    __delay_cycles(30000);
    P1OUT &= ~BIT1;             //Set RS Low
    write_nibble(0b00000010);
    pulse_enable();
    write_nibble(0b00000010);
    pulse_enable();
    write_nibble(0b00001100);
    pulse_enable();
    __delay_cycles(10);
    write_nibble(0b00000000);
    pulse_enable();
    write_nibble(0b00001111);
    pulse_enable();
    __delay_cycles(10);
    write_nibble(0b00000000);
    pulse_enable();
    write_nibble(0b00000001);
    pulse_enable();
    __delay_cycles(2000);
    write_nibble(0b00000000);
    pulse_enable();
    write_nibble(0b00000110);
    pulse_enable();
    __delay_cycles(10);
}

void write_mode(void){
    write_nibble(0b00001000);       //Address 00
    P1OUT &= ~BIT1;
    pulse_enable();
    write_nibble(0b00000000);
    pulse_enable();

    write_nibble(0b00000100);       //Character M
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00001101);
    pulse_enable();

    write_nibble(0b00000100);       //Character A
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00000001);
    pulse_enable();

    write_nibble(0b00000101);       //Character T
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00000100);
    pulse_enable();

    write_nibble(0b00000100);       //Character C
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00000011);
    pulse_enable();

    write_nibble(0b00000100);       //Character H
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00001000);
    pulse_enable();
}


//----------------------END LCD Functions




int main(void)
{
    
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    UCB0CTLW0 = UCSWRST;                      // Software reset enabled
    UCB0CTLW0 |= UCMODE_3;           // I2C mode, sync mode
    UCB0CTLW0 &= ~UCMST;
    UCB0I2COA0 = 0x0070 | UCOAEN;         // Own Address and enable
    UCB0CTLW0 &= ~UCTR;             //Put the slave into recieve mode

    //-- Configure eUSCI_B0
    UCB0CTLW0 |= UCSSEL__SMCLK; // Choose BRCLK=SMCLK=1MHz
    UCB0BRW = 10;               // Divide BRCLK by 10 for SCL = 100kHz


    UCB0TBCNT = 0x04;           // recieve 1 byte of data
    UCB0CTLW1 &= ~UCASTP_0;      // Auto STOP when UCB0TBCNT reached
    UCB0CTLW1 &= ~UCASTP_1;

    //-- Configure Ports
    P1SEL1 &= ~BIT3;            // P1.3 = SCL
    P1SEL0 |= BIT3;

    P2DIR |= BIT0;              //initialize heartbeat LED pin
    P2OUT &= ~BIT0;

    P1SEL1 &= ~BIT2;            // P1.2 = SDA
    P1SEL0 |= BIT2;



    //-- Take eUSCI_B0 out of SW reset
    UCB0CTLW0 &= ~UCSWRST;      // UCSWRST=1 for eUSCI_B0 in SW reset
    

    //-- Enable Interrupts

    HeartBeat_init();
    lcd_init();

    PM5CTL0 &= ~LOCKLPM5;       // Disable LPM

    lcd_start();
    
    UCB0IE |= UCRXIE0;          // Enable I2C Rx0 IRQ
    __enable_interrupt();       // Enable Maskable IRQs


    while(1) {
        write_mode();
    }
    
    return 0;
}

void record_temp(void){
    static int count;

    if(count == 0){
        thousands = (char)temp;
        count += 1;
        return;
    }else if(count == 1){
        hundreds = (char)temp;
        count += 1;
        return;
    }else if(count == 2){
        tens = (char)temp;
        count += 1;
        return;
    }else if(count == 3){
        ones = (char)temp;
        count = 0;
        return;
    }else{
        return;
    }

}

//----------------- Interrupt Service Routines ----------------------------------------
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_ISR(void){

    temp = UCB0RXBUF;
    record_temp();
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    P2OUT ^= BIT0;
}