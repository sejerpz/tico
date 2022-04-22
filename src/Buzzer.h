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

#include <stdlib.h>

#ifndef BUZZER_H
#define BUZZER_H

// formato array suoni { nreventi, freq1 evento 1, durata (ms) evento 1, ...., freq evento N, durata (ms) evento N }
const unsigned int soundWelcome[] = { 1, 440, 150 };
const unsigned int soundStart[] = { 5, 880, 150, 0, 150, 880, 150, 0, 150, 880, 150 };
const unsigned int soundPaused[] = { 3, 960, 150, 0, 150, 880, 150 };
const unsigned int soundStartCountDown[] = { 3, 739, 100, 0, 100, 739, 150 };
const unsigned int soundCancel[] = { 1, 220, 1000 };
const unsigned int soundAck[] = { 1, 440, 150 };
const unsigned int soundCountDown[] = { 3, 440, 100, 0, 50, 440, 50 };
const unsigned int soundCountDownHigh[] = { 3, 880, 50, 0, 50, 880, 100 };
const unsigned int soundEnd[] = { 1, 880, 1000 };
const unsigned int soundReset[] = { 5, 880, 500, 739, 500, 440, 150, 0, 100, 440, 150 };
const unsigned int soundMetronome[] = { 1, 440, 100 };
const unsigned int soundHalfMinute[] = { 3, 440, 100, 0, 50, 440, 100 };
const unsigned int soundMinute[] = { 1, 440, 500 };
const unsigned int soundRelativeMinute[] = { 1, 880, 500 };
const unsigned int soundStartListening[] = { 2, 220, 150, 440, 150 };
const unsigned int soundStopListening[] = { 3, 440, 100, 220, 100, 110, 200 };

struct Buzzer {
  const unsigned int *sound = NULL;

  unsigned int soundIndex = 0;
  unsigned long soundStartMillis = 0;
};

#endif