#include "intrinsics.h"
#include "msp430fr2355.h"
#include <msp430.h>
#include <stdbool.h>
#include "math.h"

char celcius;
int thousands;
int hundreds;
int tens;
int ones;

void init_temp(){
    P1SEL1 |= BIT5;
    P1SEL0 |= BIT5;

    PM5CTL0 &= ~LOCKLPM5;

    ADCCTL0 &= ~ADCSHT;
    ADCCTL0 |= ADCSHT_2;
    ADCCTL0 |= ADCON;

    ADCCTL1 |= ADCSSEL_2;
    ADCCTL1 |= ADCSHP;

    //ADCCTL1 |= ADCSHS_2;

    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;

    ADCMCTL0 |= ADCINCH_5;

    ADCIE |= ADCIE0;
    __enable_interrupt();
}

void convert_to_temp(int ADC_Value){
    double volts = ((double)ADC_Value / 4096) * 3.3;

    //double temp = -1481.96 + sqrt(2196200 + ((1.8639 - volts)/.00000388));
    double temp = (volts - 1.8663)/(-.01169);

    int real_temp = 100*temp;
    thousands = (real_temp/1000) + 48;
    real_temp %= 1000;
    hundreds = (real_temp/100) + 48;
    real_temp %= 100;
    tens = (real_temp/10) + 48;
    ones = (real_temp%10) + 48;

}