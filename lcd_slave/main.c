#include "intrinsics.h"
#include "msp430fr2310.h"
#include "heartbeat.h"
#include "stdint.h"
#include "lcd.h"

int temp = 0;
char thousands;
char hundreds;
char tens;
int mode;
int window;
int time_3;
int time_2;
int time_1;
char thousands_2;
char hundreds_2;
char tens_2;


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
    //write temp
    write_address(0x08);
    long_delay();

    write_char('A');

    write_char(':');

    write_char(thousands);

    write_char(hundreds);

    write_char('.');

    write_char(ten);

    write_char(0b11011111);         //Degree circle thing

    write_char('C');
}


void write_window_time(void){
    write_address(0x40);            //Address 40
    long_delay();

    write_char(window);

    clear_next();

    write_char(time_3);
    write_char(time_2);
    write_char(time_1);
    write_char('s');
}

void write_pl(void){
    //write temp
    write_address(0x48);
    long_delay();

    write_char('P');

    write_char(':');

    write_char(thousands_2);

    write_char(hundreds_2);

    write_char('.');

    write_char(ten_2);

    write_char(0b11011111);         //Degree circle thing

    write_char('C');
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
        __delay_cycles(119347);     //Number chosen for 131578 cycles = 1s - 
                                    //1206 cycles worst case senerio for mode write -
                                    //3809 cycles worst case senerio for ambient write
                                    //3407 cycles worst case senerio for time window write
                                    //3809 cycles worst case senerio for plant write
                                    //= 119347
        write_mode();
        write_am();
        write_window_time();
        write_pl();
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
        case 4: window = temp; break;
        case 5: time_3 = temp; break;
        case 6: time_2 = temp; break;
        case 7: time_1 = temp; break;
        case 8: thousands_2 = temp; break;
        case 9: hundreds_2 = temp; break;
        case 10: tens_2 = temp; break;
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
