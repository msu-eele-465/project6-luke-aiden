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
int mode;
int window;
int time;


//---------------LCD Functions-------------------------
void short_delay(void){
    __delay_cycles(200);
}

void long_delay(void){
    __delay_cycles(2000);
}

void pulse_enable(){
    P1OUT |= BIT0;           // E = 1
    __delay_cycles(1);      // Hold E high for a short time
    P1OUT &= ~BIT0;          // E = 0   
    short_delay();
}

/*
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
}*/

void write_nibble(uint8_t data){
    // Clear P1.4–P1.7
    P1OUT &= ~(BIT4 | BIT5 | BIT6 | BIT7);

    // Set only bits corresponding to nibble
    P1OUT |= (data & 0x0F) << 4;
}

void write_address(uint8_t addy){
    uint8_t addy_up = (addy & 0xF0) >> 4;
    uint8_t addy_low = addy & 0x0F;

    P1OUT &= ~BIT1;
    write_nibble(addy_up);
    pulse_enable();
    write_nibble(addy_low);
    pulse_enable();
}
/*
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
    short_delay();
}
*/

void lcd_start(void) {
    __delay_cycles(30000); // Wait >15ms after power-on

    P1OUT &= ~BIT1; // RS = 0 (command mode)

    // Reset sequence to force 8-bit mode first
    write_nibble(0x03);
    pulse_enable();
    __delay_cycles(5000);

    write_nibble(0x03);
    pulse_enable();
    __delay_cycles(200);

    write_nibble(0x03);
    pulse_enable();
    __delay_cycles(200);

    // Switch to 4-bit mode
    write_nibble(0x02);
    pulse_enable();
    __delay_cycles(100);

    // Full commands now using 2 nibbles
    write_address(0x28); // Function set: 4-bit, 2-line, 5x8 font
    write_address(0x0F); // Display ON, cursor ON, blink ON ✅
    write_address(0x01); // Clear display
    __delay_cycles(2000);
    write_address(0x06); // Entry mode set: increment, no shift
}





void write_char(uint8_t data){
    uint8_t high = (data >> 4) & 0x0F;
    uint8_t low = data & 0x0F;

    P1OUT |= BIT1;
    write_nibble(high);
    pulse_enable();
    write_nibble(low);
    pulse_enable();
}

void clear_next(void){
    write_char(' ');
}

void write_mode(void){
    write_address(0x00);
    long_delay();

    switch (mode) {
        case 1: // match
            write_char('M');
            write_char('A');
            write_char('T');
            write_char('C');
            write_char('H');
            break;

        case 2: // heat
            write_char('H');
            write_char('E');
            write_char('A');
            write_char('T');
            clear_next();
            break;

        case 3: // cool
            write_char('C');
            write_char('O');
            write_char('O');
            write_char('L');
            clear_next();
            break;

        case 4: // off
            write_char('O');
            write_char('F');
            write_char('F');
            clear_next();
            clear_next();
            break;

        case 5: // set
            write_char('S');
            write_char('E');
            write_char('T');
            clear_next();
            clear_next();
            break;
    }
}


void write_am(void){
    uint8_t thousands_up = (thousands & 0xF0) >> 4;
    uint8_t thousands_low = thousands & 0x0F;
    uint8_t hundreds_up = (hundreds & 0xF0) >> 4;
    uint8_t hundreds_low = hundreds & 0x0F;
    uint8_t tens_up = (tens & 0xF0) >> 4;
    uint8_t tens_low = tens & 0x0F;

    //write temp
    write_address(0x08);

    write_nibble(0b00000100);       //Character A
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00000001);
    pulse_enable();

    write_nibble(0b00000011);       //Character :
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00001010);
    pulse_enable();

    write_nibble(thousands_up);       //thousands place
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(thousands_low);
    pulse_enable();

    write_nibble(hundreds_up);       //hundreds place
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(hundreds_low);
    pulse_enable();

    write_nibble(0b00000010);       //Character .
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00001110);
    pulse_enable();

    write_nibble(tens_up);       //tens place
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(tens_low);
    pulse_enable();

    write_nibble(0b00001101);       //Character degree circle thing
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00001111);
    pulse_enable();

    write_nibble(0b00000100);       //Character C
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(0b00000011);
    pulse_enable();
}


void write_window_time(void){


    write_address(0x40);            //Address 40


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

    long_delay();
    
    UCB0IE |= UCRXIE0;          // Enable I2C Rx0 IRQ
    __enable_interrupt();       // Enable Maskable IRQs


    while(1) {
        mode = 2;
        __delay_cycles(130078);     //Number chosen for 131578 cycles = 1s - 
                                    //600 cycles worst case senerio for mode write -
                                    //900 cycles worst case senerio for ambient write
                                    //= 130078
        write_mode();
        //write_am();
        //write_window_time();

    }
    
    return 0;
}

void record_temp() {
    static uint8_t count = 0;
    switch (count++) {
        case 0: thousands = temp; break;
        case 1: hundreds = temp; break;
        case 2: tens = temp; break;
        case 3: mode = temp; break;
        default: count = 0; break;
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

/*
#include "intrinsics.h"
#include "msp430fr2310.h"
#include "heartbeat.h"
#include "stdint.h"
#include "lcd.h"

uint8_t temp = 0, thousands, hundreds, tens, mode;

const uint8_t MATCH[] = {0x0D, 0x01, 0x04, 0x03, 0x08};
const uint8_t HEAT[]  = {0x08, 0x05, 0x01, 0x04};
const uint8_t COOL[]  = {0x03, 0x0F, 0x0F, 0x0C};
const uint8_t OFF[]   = {0x0F, 0x06, 0x06};
const uint8_t SET[]   = {0x03, 0x05, 0x04};

void pulse_enable() {
    P1OUT |= BIT0;
    __delay_cycles(50);
    P1OUT &= ~BIT0;
}

void write_nibble(uint8_t data) {
    P1OUT = (P1OUT & ~(BIT4 | BIT5 | BIT6 | BIT7)) | ((data & 0x0F) << 4);
}

void lcd_cmd(uint8_t nib, uint8_t data) {
    write_nibble(nib);
    P1OUT &= ~BIT1;
    pulse_enable();
    write_nibble(data);
    pulse_enable();
}

void lcd_data(uint8_t nib, uint8_t data) {
    write_nibble(nib);
    P1OUT |= BIT1;
    pulse_enable();
    write_nibble(data);
    pulse_enable();
}

void lcd_start() {
    __delay_cycles(30000);
    P1OUT &= ~BIT1;
    write_nibble(0x02); pulse_enable();
    write_nibble(0x02); pulse_enable();
    write_nibble(0x0C); pulse_enable();
    __delay_cycles(10);
    write_nibble(0x00); pulse_enable();
    write_nibble(0x0F); pulse_enable();
    __delay_cycles(10);
    write_nibble(0x00); pulse_enable();
    write_nibble(0x01); pulse_enable();
    __delay_cycles(2000);
    write_nibble(0x00); pulse_enable();
    write_nibble(0x06); pulse_enable();
    __delay_cycles(10);
}

void clear_next() {
    lcd_data(0x02, 0x00);
}

void write_string(const uint8_t* str, uint8_t len) {
    lcd_cmd(0x08, 0x00);
    int i;
    for (i = 0; i < len; i++) {
        lcd_data(0x04, str[i]);
    }
}

void write_mode() {
    switch (mode) {
        case 1: write_string(MATCH, 5); break;
        case 2: write_string(HEAT, 4); clear_next(); break;
        case 3: write_string(COOL, 4); clear_next(); break;
        case 4: write_string(OFF, 3); clear_next(); clear_next(); break;
        case 5: write_string(SET, 3); clear_next(); clear_next(); break;
    }
}

void write_am() {
    uint8_t up, low;
    lcd_cmd(0x08, 0x08);
    lcd_data(0x04, 0x01); // A
    lcd_data(0x03, 0x0A); // :

    up = (thousands & 0xF0) >> 4; low = thousands & 0x0F;
    lcd_data(up, low);

    up = (hundreds & 0xF0) >> 4; low = hundreds & 0x0F;
    lcd_data(up, low);

    lcd_data(0x02, 0x0E); // .

    up = (tens & 0xF0) >> 4; low = tens & 0x0F;
    lcd_data(up, low);

    lcd_data(0x0D, 0x0F); // Degree symbol
    lcd_data(0x04, 0x03); // C
}

void record_temp() {
    static uint8_t count = 0;
    switch (count++) {
        case 0: thousands = temp; break;
        case 1: hundreds = temp; break;
        case 2: tens = temp; break;
        case 3: mode = temp; break;
        default: count = 0; break;
    }
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_ISR(void) {
    temp = UCB0RXBUF;
    record_temp();
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void) {
    P2OUT ^= BIT0;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    UCB0CTLW0 = UCSWRST | UCMODE_3;
    UCB0I2COA0 = 0x0070 | UCOAEN;
    UCB0CTLW0 &= ~UCMST & ~UCTR;
    UCB0CTLW0 |= UCSSEL__SMCLK;
    UCB0BRW = 10;
    UCB0TBCNT = 0x04;
    UCB0CTLW1 &= ~(UCASTP_0 | UCASTP_1);

    P1SEL0 |= BIT2 | BIT3;
    P1SEL1 &= ~(BIT2 | BIT3);

    P2DIR |= BIT0;
    P2OUT &= ~BIT0;

    UCB0CTLW0 &= ~UCSWRST;

    HeartBeat_init();
    lcd_init();
    PM5CTL0 &= ~LOCKLPM5;

    lcd_start();
    UCB0IE |= UCRXIE0;
    __enable_interrupt();

    while (1) {
        __delay_cycles(130078);
        write_mode();
        write_am();
    }

    return 0;
}
*/