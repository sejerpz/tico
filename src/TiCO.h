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

#ifndef TICO_H
#define TICO_H

const int encoderPin1 = 2;
const int encoderPin2 = 3;
const int button1Pin = 6; // start / stop
const int button2Pin = 4; // encoder button
const int buttonFocusPin = 7; // focus
const int buttonAuxPin = 5; // aux switch
const int buzzerPin = 12;
const int displayClkPin = 8; //pins definitions for TM1637 and can be changed to other ports
const int displayDIOPin = 9;
const int rele1Pin = 13;

#define PARAM_TIMER_DEVEL_MIN 1000
#define PARAM_TIMER_DEVEL_DEFAULT 120000
#define PARAM_TIMER_DEVEL_MAX 999000
#define PARAM_TIMER_ENLARGER_MIN 100
#define PARAM_TIMER_ENLARGER_DEFAULT 30000
#define PARAM_TIMER_ENLARGER_MAX 999000
#define PARAM_TIMER_ENLARGER_MODE_MIN 0
#define PARAM_TIMER_ENLARGER_MODE_DEFAULT (PrintMode::Time)
#define PARAM_TIMER_ENLARGER_MODE_MAX 1
#define PARAM_TIMER_ENLARGER_FSTOP_MIN 1
#define PARAM_TIMER_ENLARGER_FSTOP_DEFAULT 3
#define PARAM_TIMER_ENLARGER_FSTOP_MAX 10
#define PARAM_TIMER_TESTSTRIP_MIN 1000
#define PARAM_TIMER_TESTSTRIP_DEFAULT 2000
#define PARAM_TIMER_TESTSTRIP_MAX 99000
#define PARAM_TIMER_STOP_MIN 0
#define PARAM_TIMER_STOP_DEFAULT 10000
#define PARAM_TIMER_STOP_MAX 60000
#define PARAM_TIMER_FIX_MIN 0
#define PARAM_TIMER_FIX1_DEFAULT 120000
#define PARAM_TIMER_FIX2_DEFAULT 0
#define PARAM_TIMER_FIX_MAX 999000
#define PARAM_TIMER_COUNTDOWN_MIN 0
#define PARAM_TIMER_COUNTDOWN_DEFAULT 10000
#define PARAM_TIMER_COUNTDOWN_MANUAL 31000
#define PARAM_TIMER_COUNTDOWN_MAX 31000

#define PARAM_TESTSTRIP_STRIPCOUNT_MIN 0
#define PARAM_TESTSTRIP_STRIPCOUNT_DEFAULT 0
#define PARAM_TESTSTRIP_STRIPCOUNT_MAX 20
#define PARAM_TESTSTRIP_COUNTDOWN_MIN 0
#define PARAM_TESTSTRIP_COUNTDOWN_DEFAULT 3000
#define PARAM_TESTSTRIP_COUNTDOWN_MANUAL 11000
#define PARAM_TESTSTRIP_COUNTDOWN_MAX 11000
#define PARAM_TESTSTRIP_FSTOP_MIN 0
#define PARAM_TESTSTRIP_FSTOP_DEFAULT 3
#define PARAM_TESTSTRIP_FSTOP_MAX 10
#define PARAM_TESTSTRIP_PREEXPOS_MIN 0
#define PARAM_TESTSTRIP_PREEXPOS_DEFAULT 0
#define PARAM_TESTSTRIP_PREEXPOS_MAX 60000
#define PARAM_TESTSTRIP_TIMECALC_MIN 0
#define PARAM_TESTSTRIP_TIMECALC_DEFAULT 0
#define PARAM_TESTSTRIP_TIMECALC_SINGLE 0
#define PARAM_TESTSTRIP_TIMECALC_DELTA 1
#define PARAM_TESTSTRIP_TIMECALC_MAX 1

#define PARAM_BRIGHTNESS_MIN 1
#define PARAM_BRIGHTNESS_DEFAULT 1
#define PARAM_BRIGHTNESS_MAX 7

#define PARAM_TIMER_FACT_MIN 1.0f
#define PARAM_TIMER_FACT_DEFAULT 8.0f
#define PARAM_TIMER_FACT_MAX 25.0f
#define PARAM_BPM_MIN 1
#define PARAM_BPM_DEFAULT 1
#define PARAM_BPM_MAX 4


#define PARAM_VOICE_MIN 0
#define PARAM_VOICE_DEFAULT (VoiceControlMode::Disabled)
#define PARAM_VOICE_MAX 3

#define PARAM_AFTER_PRINT_DEFAULT (TimerModes::EnlargerTimer)
#define PARAM_AFTER_DEVEL_DEFAULT (TimerModes::Devel)

// setting parameters
#define PARAM_TESTSTRIP_STRIPCOUNT 0
#define PARAM_TESTSTRIP_MODE 1
#define PARAM_TESTSTRIP_CDOWN 2
#define PARAM_AFTER_PRINT 3
#define PARAM_DEVEL_MODE 4
#define PARAM_TSTOP 5
#define PARAM_TFIX1 6
#define PARAM_TFIX2 7
#define PARAM_AFTER_DEVEL 8
#define PARAM_CDOWN 9
#define PARAM_SOUND 10
#define PARAM_BRIGHTNESS 11
#define PARAM_STATISTICS 12
#define PARAM_RESET 13
#define PARAM_COUNT 14

// quick edit parameters
#define PARAM_TESTSTRIP_PRETIME 100
#define PARAM_TESTSTRIP_FSTOP 101
#define PARAM_TIMER_ENLARGER_MODE 102
#define PARAM_TIMER_ENLARGER_FSTOP 103

#define TIMER_MODE_DEFAULT (TimerModes::TestStrips)
#define TIMER_DEVEL_MODE_DEFAULT (DevelMode::Factorial)

#define TICO_MAGIC_SIGNATURE 0x50

#define FIX_THRESHOLD 30000         
#define ENCODER_ACCEL_THRESHOLD 16
#define ENCODER_MAX_TIMER_STEP 5000
#define LONG_CLICK_TIME 3000
#define ENCODER_TICKS 4 // Tick for each encoder step
#define EE_ADDR_TIMER_SETTINGS 0x00
#define MAX_TIMER_MODES 3 // l'utima modalita' (FactorialCalculator) non e' selezionabile

enum TimerModes {
  TestStrips,
  EnlargerTimer,
  Devel,
  Metronome,
  FactorialCalculator
};



enum TimerStates {
  Stopped,
  RunningDown,
  RunningUp,
  Settings,
  FocusOn
};

enum PrintSteps {
  Printing,
  PrintingCountDown,
  TestStripCountDown,
  Development,
  StopCountDown,
  Stop,
  Fix1CountDown,
  Fix1,
  Fix2CountDown,
  Fix2,
  Counter
};

enum SoundMode {
  Quiet,
  StartStop,
  All
};

enum VoiceControlMode {
  Disabled,
  StartTraining,
  Enabled,
};

enum PrintMode {
  Time, // encoder: increment / decrement time in seconds or fractions
  FStop   // encoder: increment / decrement time in f-stop fractions
};

enum DevelMode {
  Linear,
  Factorial
};

const int SoundModeCount = SoundMode::All + 1;

#endif