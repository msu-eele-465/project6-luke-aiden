
#include <msp430.h>
#include <stdbool.h>
#include "status_led.h"
#include "keypad.h"
#include "heartbeat.h"
#include "i2cMaster.h"
#include "pattern.h"
#include "temp.h"
#include "math.h"
#include "stdio.h"

// globals
#define _BITS4TO6 112
int pattern = 10;
volatile int prev_pattern = 10;
int keypad_input;
uint8_t patternVal;
unsigned int ADC_Value;
double temp;
int toggle;
int mode;





int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    HeartBeat_init();
    init_status_led_timer(&locked_rgb);
    i2c_init();
    init_temp();

    while(1)
    {
        if (locked)
        {
            P4OUT &= ~_BITS4TO6;
            clear_led_bar();
            TB0CCR0 = 32768;
            locked = check_unlock();
            TB3CCTL4 &= ~CCIE;
            TB3CCTL5 &= ~CCIE;
            TB3CCTL6 &= ~CCIE;
        }
        else 
        {
            set_status_rgb(&unlocked_rgb);
            __delay_cycles(10000);
            char test = _read_keypad_char();
            keypad_input = input_decide();
            if (keypad_input == 10)
            {
                pattern = prev_pattern;
            }
            else 
            {
                pattern = keypad_input;
            }
            if(test == 'D'){
                locked = true;
            }
            patternVal = get_outPut();
            toggle = 0;
           	UCB0TBCNT = 0x01;           // Send 1 byte of data
            UCB0I2CSA = 0x0068;         // Slave address = 0x68
            UCB0CTLW0 |= UCTXSTT;   // Generate START condition
	        __delay_cycles(10000);
        	UCB0TBCNT = 0x05;           // Send 1 byte of data
            toggle = 1;
            UCB0I2CSA = 0x0070;
            UCB0CTLW0 |= UCTXSTT;
            __delay_cycles(10000);
        }
    }
}



void i2c_tx(void){
    if(toggle){
        convert_to_temp(ADC_Value);
        UCB0TXBUF = thousands;
        __delay_cycles(1000);
        UCB0TXBUF = hundreds;
        __delay_cycles(1000);
        UCB0TXBUF = tens;
        __delay_cycles(1000);
        UCB0TXBUF = ones;
        __delay_cycles(1000);
        UCB0TXBUF = mode;
    }else{
        UCB0TXBUF = patternVal;
    }
}



#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    P6OUT ^= BIT6;
    prev_pattern = pattern_decide(prev_pattern, pattern);
    if (locked)
    {
        lock_count++;
    }
    else 
    {
//        prev_pattern = pattern_decide(prev_pattern, pattern);
    }
}

#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_CRR0(void)
{
    status_led_timer_ccr0(locked);
    ADCCTL0 |= ADCENC | ADCSC;
}

#pragma vector = TIMER3_B1_VECTOR
__interrupt void ISR_higher_CCR(void)
{
    TB3_ISR_call(TB3IV);
}

#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    i2c_tx();
}

#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void){
    ADC_Value = ADCMEM0;
    ADCCTL0 &= ~ADCENC;
}