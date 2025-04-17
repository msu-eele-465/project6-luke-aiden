/*

Logan Graham microcontroller application project 3.

This file is a header that includes all declarations for using the keypad. All pins on P3 will be used to operate the keypad.

Pins: uC: P3.i <-> i+1 membrain keypad

Constanats:
    - ROW_MASK: masks the bits 0-3 these are assosiated with the rows
    - COLUMN_MASK: masks the bits 4-7 these are assosiated with the columns

Funtions:
    - _read_keypad_columns: This funtion returns an integer whose second nibble has 1 high bit corresponding to the column read.
    - _read_keypad_rows: This function returns an integer whose first nibble has 1 high bit correspondign to the row read.
    - _read_keypad_char: This function returns a char that represents the one pressed on the keypad. the char E means no button is being pressed.
    - input_decide: this function decided what to do based on the input from the keypad.

*/

#ifndef _KEYPAD_H
#define _KEYPAD_H

#include <msp430.h>
#include <stdbool.h>
#include "keypad.h"
#include "status_led.h"


// globals

bool locked = true;
int lock_count = 0;

// Constants definitions

#define ROW_MASK 0xF
#define COLUMN_MASK 0xF0

// Function definitions

inline int _read_keypad_columns(void)
{
    int column_read = 0;

    // initialize port

    P3DIR |= ROW_MASK;                  // P3.0-3 outputs
    P3DIR &= ~COLUMN_MASK;              // P3.4-7 inputs
    P3REN |= COLUMN_MASK;               // enable resistors
    P3OUT &= ~COLUMN_MASK;              // make resistors pull down
    P3OUT |= ROW_MASK;                  // outputs set to 1

    // read port

    column_read = P3IN;

    // reset port

    P3DIR |= 0xFF;
    P3OUT &= ~0xFF;                     // resting port to all off and output
    P3REN &= ~0xFF;                     // resetting resistor byte to all zero

    column_read &= COLUMN_MASK;

    return column_read;

}

inline int _read_keypad_rows(void)
{
    int row_read = 0;

    P3DIR |= COLUMN_MASK;               // P3.4-7 outputs
    P3DIR &= ~ROW_MASK;                 // P3.0-3 inputs
    P3REN |= ROW_MASK;                  // enable resistors
    P3OUT &= ~ROW_MASK;                 // make resistors pull down
    P3OUT |= COLUMN_MASK;               // outputs set to 1

    // read port

    row_read = P3IN;
    row_read &= ROW_MASK;               // read input

    // reset port

    P3DIR |= 0xFF;
    P3OUT &= ~0xFF;                     // resting port to all off and output
    P3REN &= ~0xFF;                     // resetting resistor byte to all zero

    return row_read;
}

inline int _read_keypad_char(void)
{
    int read = (_read_keypad_columns() | _read_keypad_rows());

    switch (read) {

        case 0x11:
            return 1;
        
        case 0x21:
            return 2;

        case 0x41:
            return 3;

        case 0x81:
            return 'A';

        case 0x12:
            return 4;
        
        case 0x22:
            return 5;

        case 0x42:
            return 6;

        case 0x82:
            return 'B';

        case 0x14:
            return 7;

        case 0x24:
            return 8;

        case 0x44:
            return 9;

        case 0x84:
            return 'C';

        case 0x18:
            return '*';

        case 0x28:
            return 0;

        case 0x48:
            return '#';

        case 0x88:
            return 'D';
        
        default:
            return 'E';
    
    }
}

inline bool check_unlock(void)
{
    int code[] = {0, 1, 2, 3, 4};  
    unsigned int i = 1;

    __delay_cycles(1000);

    while(i < 5)
    {
        char keystroke = _read_keypad_char();

        if (lock_count > 50)
        {
            lock_count = 0;
            set_status_rgb(&locked_rgb);
            return true;
        }

        if (keystroke == code[i])
        {
            i++;
            set_status_rgb(&unlocking_rgb);
        }
        else if (keystroke == 'E') {}
        else if (keystroke == code[i-1])
        {
            set_status_rgb(&unlocking_rgb);
        }
        else 
        {
            set_status_rgb(&locked_rgb);
            return true;
        }
    }

    return false;

}

inline int input_decide()
{
    int output;

    char input = _read_keypad_char();

    if (input == 'A')
    {
//        _increase_speed();
        output = 10;
    }
    else if (input == 'B')
    {
//        _decrease_speed();
        output = 10;
    }
    else if (input == 'D') 
    {
        lock_count = 0;
        locked = true;
    }
    else if (input == '#'){
    }
    else if (input == '*'){
    }
    else if (input != 'E') 
    {
        output = input - '0';
    }
    else
    {
        output = 10;
    }

    return output;
}


#endif // _KEYPAD_H