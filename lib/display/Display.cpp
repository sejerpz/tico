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

#include "Display.h"

#define BRIGHT_TYPICAL  0x1

void Display::loop(unsigned long currentMillis) {
  if (count > 0) {
    if (startMillis == 0 || msgPadLen <= 4 || (currentMillis - startMillis) > 400) {
      uint8_t ch;

      if (((msgStartIndex + displayIndex) % msgPadLen) < msgLen)
        ch = *(msg + ((msgStartIndex + displayIndex) % msgPadLen));
      else
        ch = D_SEG_OFF;

      tm1637.setSegments(&ch, 1, displayIndex);
      displayIndex++;
        
      if (displayIndex % 4 == 0)
      {
        if (count < REPEAT_INFINITE && (msgStartIndex + displayIndex) % msgPadLen == 0) {
          count--;
          if (count == 0 && displayFinished != NULL) {
            displayFinished();
          }
        }
        startMillis = currentMillis;
        if (msgPadLen > 4) {
          msgStartIndex++;
        }
        displayIndex = 0;
      }
    }
  }
}

void Display::showMessage(uint8_t *message, uint8_t len, void (*_displayFinished)(), uint8_t repeat) {
  msg = message;
  msgLen = len;
  displayFinished = _displayFinished;
  if (msgLen > 4) {
    msgPadLen = msgLen + 4;
  } else {
    msgPadLen = 4;
  }
  count = repeat;
  displayIndex = 0;
  msgStartIndex = 0;
}
