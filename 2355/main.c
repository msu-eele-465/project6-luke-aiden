
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
#define MAX_WINDOW 32
int pattern = 10;
volatile int prev_pattern = 10;
int keypad_input;
uint8_t patternVal;
unsigned int ADC_Value;
double temp;
int toggle;
int mode = '1';
int window;
int time_in_3;
int time_in_2;
int time_in_1;
int ave;




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
            __delay_cycles(20000);
            int test = _read_keypad_char();
            if (test >= 0 && test < 10) {
                window = test;
            } else {
                switch (test) {
                    case 'A':
                        mode = 1;
                        break;
                    case 'B':
                        mode = 2;
                        break;
                    case 'C':
                        mode = 3;
                        break;
                    case '*':
                        mode = 4;
                        break;
                }
            }
            if(test == 'D'){
                locked = true;
            }
            toggle = 0;
            UCB0TBCNT = 0x01;               // Set byte count
            UCB0I2CSA = 0x0068;             // Set slave address
            UCB0CTLW0 |= UCTXSTT;           // Start condition
            __delay_cycles(10000);
        	UCB0TBCNT = 0x09;           // Send 9 bytes of data
            toggle = 1;
            UCB0I2CSA = 0x0070;
            UCB0CTLW0 |= UCTXSTT;
            __delay_cycles(10000);
        }
    }
}


int moving_avg(int new_val, int window_size) {
    static int buffer[MAX_WINDOW] = {0};
    static int sum = 0;
    static int index = 0;
    static int count = 0;
    static int prev_window = 0;

    if (window_size != prev_window) {
        sum = 0;
        index = 0;
        count = 0;
        int i;
        for (i = 0; i < MAX_WINDOW; i++) buffer[i] = 0;
        prev_window = window_size;
    }

    sum -= buffer[index];
    buffer[index] = new_val;
    sum += new_val;
    index = (index + 1) % window_size;

    if (count < window_size)
        count++;

    return sum / count;
}



void i2c_tx(void){
    if(toggle){
        ave = moving_avg(ADC_Value, window);
        convert_to_temp(ave);
        UCB0TXBUF = thousands;              //Thousands, hundreds, tens, and ones need to be ascii
        __delay_cycles(1000);
        UCB0TXBUF = hundreds;
        __delay_cycles(1000);
        UCB0TXBUF = tens;
        __delay_cycles(1000);
        UCB0TXBUF = ones;
        __delay_cycles(1000);
        UCB0TXBUF = mode;                   //mode needs to be int
        __delay_cycles(1000);
        UCB0TXBUF = (char)window;                 //window and time_in need to be ascii
        __delay_cycles(1000);
        UCB0TXBUF = time_in_3;
        __delay_cycles(1000);
        UCB0TXBUF = time_in_2;
        __delay_cycles(1000);
        UCB0TXBUF = time_in_1;
    }else{
        UCB0TXBUF = '1';
        //UCB0TXBUF = patternVal;           //logic from Luke for sending paterns to LED 
    }
}



#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    P6OUT ^= BIT6;
    ADCCTL0 |= ADCENC | ADCSC;
    if (locked)
    {
        lock_count++;
    }
}

#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_CRR0(void)
{
    status_led_timer_ccr0(locked);
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