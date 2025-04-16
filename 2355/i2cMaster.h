#include "intrinsics.h"
#include <msp430.h>
#include <stdbool.h>

i2c_init(){
    UCB0CTLW0 |= UCSWRST;       // UCSWRST=1 for eUSCI_B0 in SW reset

	UCB0CTLW0 |= UCSSEL_3;      // Choose BRCLK=SMCLK=1MHz
	UCB0BRW = 10;               // Divide BRCLK by 10 for SLC=100kHz

	UCB0CTLW0 |= UCMODE_3;      // Put into I2C mode
	UCB0CTLW0 |= UCMST;         // Put into master mode
	UCB0CTLW0 |= UCTR;          // Put into Tx mode

	UCB0CTLW1 |= UCASTP_2;       // Auto STOP when UCB0TBCNT reached

	P1SEL1 &= ~BIT3;            // P1.3 = SCL
	P1SEL0 |= BIT3;

	P1SEL1 &= ~BIT2;            // P1.2 = SDA
	P1SEL0 |= BIT2;

    PM5CTL0 &= ~LOCKLPM5;       // Disable LPM

	UCB0CTLW0 &= ~UCSWRST;      // UCSWRST=1 for eUSCI_B0 in SW reset

	UCB0IE |= UCTXIE0;          // Enable I2C Tx0 IRQ
	__enable_interrupt();       // Enable Maskable IRQs
}