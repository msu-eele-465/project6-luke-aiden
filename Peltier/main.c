
#include <msp430.h>
#include <intrinsics.h>
#include <driverlib.h>
#include <math.h>

unsigned char data = 0x00;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // stop watchdog timer
    UCB0CTLW0 = UCSWRST;                      // Software reset enabled

    // 4.2 cool
    P4DIR |= BIT2;
    P4OUT &= ~BIT2;

    // 4.3 heat
    P4DIR |= BIT3;
    P4OUT &= ~BIT3;

    PM5CTL0 &= ~LOCKLPM5;                     // Disable LPM




    while(1){
        if("correct mode") {

        
        switch("keypad press") {
            // Heat
            case 'A' : P4OUT |= BIT3;
                       P4OUT &= ~BIT2;
                       data = 0x01;
                       UCB0CTLW0 |= UCTXSTT;
                       break;

            // Cool
            case 'B' : P4OUT |= BIT2;
                       P4OUT &= ~BIT3;
                       data = 0x02;
                       UCB0CTLW0 |= UCTXSTT;
                       break;

            // Match Ambient
            case 'C' : data = 0x03;
                       UCB0CTLW0 |= UCTXSTT;
                       /*
                       if hotter --> cool
                       if cooler --> heat
                       */
                       break;

            // Exit
            case 'D' : data = 0x00;
                       UCB0CTLW0 |= UCTXSTT;
                       P4OUT &= ~BIT2;
                       P4OUT &= ~BIT3;
                       //lock keypad (somehow)
                       break; 

            default: break;

        }
    }
    }
}




/*
int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    while(1)
    {
        // Toggle P1.0 output
        GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );

        // Delay
        for(i=10000; i>0; i--);
    }
}
*/