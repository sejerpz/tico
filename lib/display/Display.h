/*
 *
 * Copyright (C) 2022 Andrea Del Signore
 *
 * This file is part of TiCO.
 * 
 * TiCO is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 *
 * TiCo is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Foobar. 
 * If not, see <https://www.gnu.org/licenses/>. 
 * 
 * Author:
 *    Andrea Del Signore <sejerpz@gmail.com>
 * 
 */

#include <Arduino.h>
#include <TM1637Display.h>

#ifndef DISPLAY_LIBRARY
#define DISPLAY_LIBRARY


//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D

#define D_SEG_OFF 0b0
#define D_SEG_A 0b01110111
#define D_SEG_B 0b01111100
#define D_SEG_C 0b00111001
#define D_SEG_c 0b01011000
#define D_SEG_D 0b01011110
#define D_SEG_E 0b01111001
#define D_SEG_F 0b01110001
#define D_SEG_G 0b01101111
#define D_SEG_H 0b01110110
#define D_SEG_I 0b00110000
#define D_SEG_i 0b00010000
#define D_SEG_J 0b00011110
#define D_SEG_L 0b00111000
#define D_SEG_M 0b00110111
#define D_SEG_N 0b01010100
#define D_SEG_O 0b00111111
#define D_SEG_o 0b01011100
#define D_SEG_P 0b01110011
#define D_SEG_Q 0b01100111
#define D_SEG_R 0b01010000
#define D_SEG_S 0b01101101
#define D_SEG_T 0b01111000
#define D_SEG_U 0b00111110
#define D_SEG_V 0b00111110 //0b00011100
#define D_SEG_Y 0b01110010
#define D_SEG_Z 0b01011011

#define D_SEG_0 0b00111111
#define D_SEG_1 0b00000110
#define D_SEG_2 0b01011011
#define D_SEG_3 0b01001111
#define D_SEG_4 0b01100110
#define D_SEG_5 0b01101101
#define D_SEG_6 0b01111101
#define D_SEG_7 0b00000111
#define D_SEG_8 0b01111111
#define D_SEG_9 0b01100111

#define D_SEG_MINUS 0b01000000
#define D_SEG_SLASH 0b01010010

#define D_SEG_POINT 0b10000000

#define REPEAT_INFINITE 0xFF

class Display {

public:
    Display(uint8_t pinClk, uint8_t pinDIO, uint8_t brightness = 0x02) : tm1637(pinClk, pinDIO) { 
        tm1637.setBrightness(brightness);
    }

    void loop(unsigned long currentMillis);
    uint8_t encodeDigit(uint8_t digit) { return tm1637.encodeDigit(digit); }
    void showMessage(uint8_t *message, uint8_t len, void (*_displayFinished)() = NULL, uint8_t repeat = 1);
    void setBrightness(uint8_t brightness) { tm1637.setBrightness(brightness); }
private:
    TM1637Display tm1637;
    unsigned char displayIndex = 0;
    unsigned long startMillis = 0;
    unsigned char msgStartIndex = 0;

    uint8_t *msg = NULL;
    uint8_t msgLen = 0;
    uint8_t count = 0;
    uint8_t msgPadLen;
    void (*displayFinished) ();
};

#endif