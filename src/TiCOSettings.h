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
#include "TiCO.h"

#ifndef TICOSETTINGS_H
#define TICOSETTINGS_H

struct TicoSettings {
  // Current selected timer mode
  TimerModes timerMode = TIMER_MODE_DEFAULT;
  // Configured devel mode: Linear | Factorial
  DevelMode develMode = TIMER_DEVEL_MODE_DEFAULT;
  // Print time in milliseconds
  unsigned long printTime = PARAM_TIMER_ENLARGER_DEFAULT;
  // Print base time for FStop mode in milliseconds
  int printFStopBaseTime = PARAM_TIMER_ENLARGER_DEFAULT;
  // Print encoder mode: linear / f-stop
  PrintMode printMode = PARAM_TIMER_ENLARGER_MODE_DEFAULT;
   // Print f-stop interval settings (fraction of stop)
  int printFStop = PARAM_TIMER_ENLARGER_FSTOP_DEFAULT;
  // Devel time in milliseconds
  unsigned long develTime = PARAM_TIMER_DEVEL_DEFAULT;
  // Devel factorial multiplicator value
  float develFactor = PARAM_TIMER_FACT_DEFAULT;
  // Stop time in milliseconds
  unsigned long stopTime = PARAM_TIMER_STOP_DEFAULT;
  // Fix1 time in milliseconds
  unsigned long fix1Time = PARAM_TIMER_FIX1_DEFAULT;
  // Fix2 time in milliseconds
  unsigned long fix2Time = PARAM_TIMER_FIX2_DEFAULT;
  // Interstep countdown time in milliseconds
  unsigned long countDownTime = PARAM_TIMER_COUNTDOWN_DEFAULT;
  
  // Teststrip number, 0 means manual
  int testStripStripCount = PARAM_TESTSTRIP_STRIPCOUNT_DEFAULT;
  // Teststrip base time if in f-stop mode or teststrip time in linear mode (milliseconds)
  int testStripBaseTime = PARAM_TIMER_TESTSTRIP_DEFAULT;
  // Teststrip pre exposition time (milliseconds)
  int testStripPreExpos = PARAM_TESTSTRIP_PREEXPOS_DEFAULT;
  // Teststrip countdown time in milliseconds
  unsigned long testStripCountDownTime = PARAM_TESTSTRIP_COUNTDOWN_DEFAULT;
  // Teststrip f-stop interval settings (fraction of stop)
  int testStripFStop = PARAM_TESTSTRIP_FSTOP_DEFAULT;
  // Teststrip f-stop time mode single exposition or delta from the previous value
  uint8_t testStripTimeCalc = PARAM_TESTSTRIP_TIMECALC_DEFAULT;

  // Workflow: wich step activate after printing (EnlargerTimer = Stop)
  TimerModes afterPrintGoTo = PARAM_AFTER_PRINT_DEFAULT;
  // Workflow: wich step activate after devel (Devel = Stop)
  TimerModes afterDevelGoTo = PARAM_AFTER_DEVEL_DEFAULT;

  // Metronome beat per seconds (1...4)
  uint8_t bps = PARAM_BPM_DEFAULT;

  // Display brightness
  byte brightness = PARAM_BRIGHTNESS_DEFAULT;

  // Sound configuration
  SoundMode sounds = SoundMode::All;

  // Voice control module setting
  VoiceControlMode voiceControl = PARAM_VOICE_DEFAULT;

  // Statistics
  int onCount = 0;
  unsigned long onTime = 0;

  // Magic value used for settings validation
  uint8_t magic = TICO_MAGIC_SIGNATURE;
};
#endif