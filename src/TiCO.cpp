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
#include <EEPROM.h>

#include "TiCO.h"
#include "TiCOSettings.h"
#include "Watch.h"
#include "Display.h"
#include "Messages.h"
#include "Buzzer.h"

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

struct Tico {
  TicoSettings settings = TicoSettings();
  TimerStates timerState = TimerStates::Stopped;
  PrintSteps currentPrintStep = PrintSteps::Development;
  bool skipCurrentStep = false;

  TimerModes timerStartMode = TIMER_MODE_DEFAULT;
  unsigned long timerInterval = 0;
  unsigned long timerStartValue = 0;
  unsigned long timerLastChange = 0;
  unsigned long timerPausedStart = 0;

  unsigned long releOnStart = 0;

  long timerAdd = 1000;
  byte timerChangeCount = 0;
  int timerChangeDirection = 0;
  
  uint8_t currentPar = 0;
  uint8_t currentStat = 0;

  int testStripCount = 0;
  bool testStripDone = false;

  byte timerStep = 1;

  long lastencoderValue = 0;

  uint8_t displayBuff[4] = { D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF};
  Buzzer buzzer = Buzzer();

  uint8_t reset = 0;
  bool isMessageInfoVisible = false;

  public:
    void startPrint(unsigned long currentMillis) {
      settings.timerMode = TimerModes::EnlargerTimer;
      currentPrintStep = PrintSteps::Printing;
      timerInterval = settings.printTime;
      timerStartValue = currentMillis;
      timerState = TimerStates::RunningDown;
    }

    void startDevel(unsigned long currentMillis) {
      settings.timerMode = TimerModes::Devel;
      currentPrintStep = PrintSteps::Development;

      if (settings.develMode == DevelMode::Linear) {
        timerInterval = settings.develTime;
        timerStartValue = currentMillis;
        timerState = TimerStates::RunningDown;
      } else {
        timerInterval = 0;
        timerState = TimerStates::RunningUp;
      }
    }

    void stop()
    {
      timerState = TimerStates::Stopped;
      buzzer.sound = soundEnd;
    }

    void pause(unsigned long currentMillis) {
      timerPausedStart = currentMillis;
      buzzer.sound = soundPaused;
      if (settings.timerMode == TimerModes::TestStrips && currentPrintStep == PrintSteps::Printing)
        setRele(LOW, currentMillis);
    }

    void resume(unsigned long currentMillis) {
      timerStartValue = timerStartValue + (currentMillis - timerPausedStart);
      timerPausedStart = 0;
      buzzer.sound = soundAck;
      if (settings.timerMode == TimerModes::TestStrips && currentPrintStep == PrintSteps::Printing)
        setRele(HIGH, currentMillis);
    }

    bool isPaused() {
      return timerPausedStart != 0;
    }

    void cancel() {
      timerPausedStart = 0;
      timerState = TimerStates::Stopped;
      buzzer.sound = soundCancel;
    }

    // switch focus on, if action cannot be done due to timer state returns false otherwise returns true
    bool toggleFocus() {
      bool focusToggled = true;

      if (timerState == TimerStates::FocusOn) {
        timerState = TimerStates::Stopped;
        buzzer.sound = soundFocusOff;
      } else {
        if ((settings.timerMode == TimerModes::EnlargerTimer || settings.timerMode == TimerModes::Metronome || settings.timerMode == TimerModes::TestStrips)
            && timerState == TimerStates::Stopped) {
          // focus
          timerState = TimerStates::FocusOn;
        } else {
          // can't toggle focus
          focusToggled = false;
        }
      }

      return focusToggled;
    }
    
    bool setNextStepCountDown(unsigned long currentMillis, PrintSteps nextStep) {
      bool inCountDown = false;

      if ((settings.timerMode == TimerModes::TestStrips && settings.testStripCountDownTime == PARAM_TESTSTRIP_COUNTDOWN_MANUAL)
          || (settings.timerMode == TimerModes::TestStrips && nextStep == PrintSteps::PrintingCountDown && settings.countDownTime == PARAM_TIMER_COUNTDOWN_MANUAL) 
          || (settings.timerMode != TimerModes::TestStrips && settings.countDownTime == PARAM_TIMER_COUNTDOWN_MANUAL)) {
        pause(currentMillis);
      } else if (settings.timerMode == TimerModes::TestStrips) {
        if (nextStep == PrintSteps::TestStripCountDown && settings.testStripCountDownTime > 0) {
          currentPrintStep = nextStep;
          timerInterval = settings.testStripCountDownTime;
          inCountDown = true;
        } else if (nextStep == PrintSteps::PrintingCountDown && settings.countDownTime > 0) {
          currentPrintStep = nextStep;
          timerInterval = settings.countDownTime;
          inCountDown = true;
        }
      } else if (settings.countDownTime > 0) {
        currentPrintStep = nextStep;
        timerInterval = settings.countDownTime;
        inCountDown = true;
      }

      return inCountDown;
    }

    void startTestStrip(unsigned long currentMillis) {
      testStripDone = false;
      currentPrintStep = PrintSteps::Printing;

      if (settings.testStripPreExpos > 0) {
        testStripCount = -1;
        timerInterval = settings.testStripPreExpos;
      } else {
        testStripCount = 0;
        if (settings.testStripFStop > 0) {
          // fstop
          if (settings.testStripTimeCalc == PARAM_TESTSTRIP_TIMECALC_SINGLE)
            timerInterval = calculateCurrentStripTime();
          else
            timerInterval = calculateCurrentStripDeltaTime();
        } else {
          // lineare
          timerInterval = settings.testStripBaseTime;
        }
      }
      timerStartValue = currentMillis;
      timerState = TimerStates::RunningDown;
    }

    void nextTestStrip(unsigned long currentMillis) {
      currentPrintStep = PrintSteps::Printing;
      testStripCount++;
      if (settings.testStripFStop > 0) {
          // fstop
          if (settings.testStripTimeCalc == PARAM_TESTSTRIP_TIMECALC_SINGLE)
            timerInterval = calculateCurrentStripTime();
          else
            timerInterval = calculateCurrentStripDeltaTime();
      } else {
        // lineare
        timerInterval = settings.testStripBaseTime;
      }
      timerStartValue = currentMillis;
      if (settings.testStripStripCount > 0 && testStripCount == settings.testStripStripCount - 1) {
        endTestStrip(currentMillis);
      }
    }

    void endTestStrip(unsigned long currentMillis) {
      testStripDone = true;
      buzzer.sound = soundAck;
    }

    void startCounter(unsigned long currentMillis) {
      timerState = TimerStates::RunningUp;
      currentPrintStep = PrintSteps::Counter;
      timerStartValue = currentMillis;
      timerInterval = 0;
    }

    // da https://www.analogica.it/post204604.html#p204604
    // base time × 2 ^ (step * fstopStep)
    // se ci fossero problemi di spazio sostuire pow con tabella lookup precalcolata
    unsigned long calculateCurrentStripTime() {
      unsigned long time = ((((double)settings.testStripBaseTime / 1000.0) * pow(2, (1.0 / settings.testStripFStop) * testStripCount)) + 0.005) * 1000;

      return time;
    }

    unsigned long calculateCurrentStripDeltaTime() {
      // previous step time
      unsigned long prevStepTime = 0;
      
      if (testStripCount > 0)
        prevStepTime = ((((double)settings.testStripBaseTime / 1000.0) * pow(2, (1.0 / settings.testStripFStop) * (testStripCount - 1))) + 0.005) * 1000;
   
      return calculateCurrentStripTime() - prevStepTime;
    }

    // reset settings to default values
    void resetSettings() {
      settings.timerMode = TimerModes::EnlargerTimer;
      settings.printTime = PARAM_TIMER_ENLARGER_DEFAULT;
      settings.develMode = DevelMode::Factorial;
      settings.develTime = PARAM_TIMER_DEVEL_DEFAULT;
      settings.stopTime = PARAM_TIMER_STOP_DEFAULT;
      settings.fix1Time = PARAM_TIMER_FIX1_DEFAULT;
      settings.fix2Time = PARAM_TIMER_FIX2_DEFAULT;
      settings.countDownTime = PARAM_TIMER_COUNTDOWN_DEFAULT;
      settings.develFactor = PARAM_TIMER_FACT_DEFAULT;
      settings.brightness = PARAM_BRIGHTNESS_DEFAULT;
      settings.sounds = SoundMode::All;
      settings.afterPrintGoTo = PARAM_AFTER_PRINT_DEFAULT;
      settings.afterDevelGoTo = PARAM_AFTER_DEVEL_DEFAULT;
      settings.testStripBaseTime = PARAM_TIMER_TESTSTRIP_DEFAULT;
      settings.testStripStripCount = PARAM_TESTSTRIP_STRIPCOUNT_DEFAULT;
      settings.testStripCountDownTime = PARAM_TESTSTRIP_COUNTDOWN_DEFAULT;
      settings.testStripFStop = PARAM_TESTSTRIP_FSTOP_DEFAULT;
      settings.testStripTimeCalc = PARAM_TESTSTRIP_TIMECALC_DEFAULT;
      settings.testStripPreExpos = PARAM_TESTSTRIP_PREEXPOS_DEFAULT;
      settings.magic = TICO_MAGIC_SIGNATURE;
    }

    void resetStatistics() {
      settings.onCount = 0;
      settings.onTime = 0;
    }
    // validate each settings with max / min values
    void validateSettings() {
      if ((int)settings.timerMode < 0 || (int)settings.timerMode > MAX_TIMER_MODES)
        settings.timerMode = TIMER_MODE_DEFAULT;

      if ((int)settings.develMode < 0 || (int)settings.develMode > DevelMode::Factorial)
          settings.develMode = TIMER_DEVEL_MODE_DEFAULT;

      settings.printTime = settings.printTime >= PARAM_TIMER_ENLARGER_MIN && settings.printTime <= PARAM_TIMER_ENLARGER_MAX ? settings.printTime : PARAM_TIMER_ENLARGER_DEFAULT;
      settings.develTime = settings.develTime >= PARAM_TIMER_DEVEL_MIN && settings.develTime <= PARAM_TIMER_DEVEL_MAX ? settings.develTime : PARAM_TIMER_DEVEL_DEFAULT;
      settings.stopTime = settings.stopTime >= PARAM_TIMER_STOP_MIN && settings.stopTime <= PARAM_TIMER_STOP_MAX ? settings.stopTime : PARAM_TIMER_STOP_DEFAULT;
      settings.fix1Time = settings.fix1Time >= PARAM_TIMER_FIX_MIN && settings.fix1Time <= PARAM_TIMER_FIX_MAX ? settings.fix1Time : PARAM_TIMER_FIX1_DEFAULT;
      settings.fix2Time = settings.fix2Time >= PARAM_TIMER_FIX_MIN && settings.fix2Time <= PARAM_TIMER_FIX_MAX ? settings.fix2Time : PARAM_TIMER_FIX2_DEFAULT;
      settings.countDownTime = settings.countDownTime >= PARAM_TIMER_COUNTDOWN_MIN && settings.countDownTime <= PARAM_TIMER_COUNTDOWN_MAX ? settings.countDownTime : PARAM_TIMER_COUNTDOWN_DEFAULT;
      settings.develFactor = settings.develFactor >= PARAM_TIMER_FACT_MIN && settings.develFactor <= PARAM_TIMER_FACT_MAX ? settings.develFactor : PARAM_TIMER_FACT_DEFAULT;
      settings.bps = settings.bps >= PARAM_BPM_MIN && settings.bps <= PARAM_BPM_MAX ? settings.bps : PARAM_BPM_DEFAULT;
      settings.brightness = settings.brightness >= PARAM_BRIGHTNESS_MIN && settings.brightness <= PARAM_BRIGHTNESS_MAX ? settings.brightness : PARAM_BRIGHTNESS_DEFAULT;
      settings.sounds = (int)settings.sounds < SoundModeCount ? settings.sounds : SoundMode::All;
      settings.testStripBaseTime = settings.testStripBaseTime >= PARAM_TIMER_TESTSTRIP_MIN && settings.testStripBaseTime <= PARAM_TIMER_TESTSTRIP_MAX ? settings.testStripBaseTime : PARAM_TIMER_TESTSTRIP_DEFAULT;
      settings.testStripStripCount = settings.testStripStripCount >= PARAM_TESTSTRIP_STRIPCOUNT_MIN && settings.testStripStripCount <= PARAM_TESTSTRIP_STRIPCOUNT_MAX ? settings.testStripStripCount : PARAM_TESTSTRIP_STRIPCOUNT_DEFAULT;
      settings.testStripCountDownTime = settings.testStripCountDownTime >= PARAM_TESTSTRIP_COUNTDOWN_MIN && settings.testStripCountDownTime <= PARAM_TESTSTRIP_COUNTDOWN_MAX ? settings.testStripCountDownTime : PARAM_TESTSTRIP_COUNTDOWN_DEFAULT;
      settings.testStripFStop = settings.testStripFStop >= PARAM_TESTSTRIP_FSTOP_MIN && settings.testStripFStop <= PARAM_TESTSTRIP_FSTOP_MAX ? settings.testStripFStop : PARAM_TESTSTRIP_FSTOP_DEFAULT;
      settings.testStripPreExpos = settings.testStripPreExpos >= PARAM_TESTSTRIP_PREEXPOS_MIN && settings.testStripPreExpos <= PARAM_TESTSTRIP_PREEXPOS_MAX ? settings.testStripPreExpos : PARAM_TESTSTRIP_PREEXPOS_DEFAULT;
      settings.testStripTimeCalc = settings.testStripTimeCalc >= PARAM_TESTSTRIP_TIMECALC_MIN && settings.testStripTimeCalc <= PARAM_TESTSTRIP_TIMECALC_MAX ? settings.testStripTimeCalc : PARAM_TESTSTRIP_TIMECALC_DEFAULT;
      switch (settings.afterPrintGoTo)
      {
        case TimerModes::EnlargerTimer:
        case TimerModes::Devel:
          break;
        default:
          settings.afterPrintGoTo = PARAM_AFTER_PRINT_DEFAULT;
          break;
      }
      switch (settings.afterDevelGoTo)
      {
        case TimerModes::Devel:
        case TimerModes::Metronome:
          break;
        default:
          settings.afterDevelGoTo = PARAM_AFTER_DEVEL_DEFAULT;
          break;
      }
    }

    void setRele(uint8_t val, unsigned long millis)
    {
      digitalWrite(rele1Pin, val);
      if (val == HIGH && releOnStart == 0) {
        releOnStart = millis;
      } else if (val == LOW && releOnStart != 0) {
        settings.onCount++;
        settings.onTime += millis - releOnStart;
        releOnStart = 0;
      } 
    }
} tico;

void onButton1Clicked (Watch::Event *evt);
void onButton2Clicked (Watch::Event *evt);
void onButtonFocusClicked (Watch::Event *evt);
void onTimerDisplayElapsed (Watch::Event *evt);
void onTimerSoundElapsed (Watch::Event *evt);
void onTimerStatusChanged (Watch::Event *evt);
void onDisplayChanged (Watch::Event *evt);
void onSettingsChanged (Watch::Event *evt);
void onBuzzerChanged (Watch::Event *evt);
void onEncoderChanged (Watch::Event *evt);
void onCurrentParChanged (Watch::Event *evt);
void onCurrentPrintStepChanged (Watch::Event *evt);
void displayCurrentMode ();
void displayCurrentParDes ();
void displayCurrentParValue ();
void clearDisplay ();

Watch::Button button1 = Watch::Button(button1Pin, onButton1Clicked, 50,  1500, LOW);
Watch::Button button2 = Watch::Button(button2Pin, onButton2Clicked);
Watch::Button button3 = Watch::Button(buttonFocusPin, onButtonFocusClicked, 50,  1500, LOW);
Watch::Timer timerDisplay = Watch::Timer(100, onTimerDisplayElapsed, false);
Watch::Timer timerSound = Watch::Timer(50, onTimerSoundElapsed);
Watch::Memory timerStatusMemory = Watch::Memory(&tico.timerState, sizeof(TimerStates), onTimerStatusChanged);
Watch::Memory displayMemory = Watch::Memory(tico.displayBuff, 4, onDisplayChanged);
Watch::Memory settingsMemory = Watch::Memory(&tico.settings, sizeof(TicoSettings), onSettingsChanged);
Watch::Memory buzzerMemory = Watch::Memory(&tico.buzzer, sizeof(int *), onBuzzerChanged);
Watch::Memory encoderMemory = Watch::Memory((void *)&encoderValue, sizeof(long), onEncoderChanged);
Watch::Memory currentParMemory = Watch::Memory((void *)&tico.currentPar, sizeof(uint8_t), onCurrentParChanged);
Watch::Memory currentPrintStepMemory = Watch::Memory((void *)&tico.currentPrintStep, sizeof(PrintSteps), onCurrentPrintStepChanged);
Watch::Event *ctx[] = { &button1, &button2, &button3, 
                        &settingsMemory, &displayMemory, 
                        &buzzerMemory,  &timerStatusMemory, 
                        &encoderMemory, &currentParMemory, &currentPrintStepMemory,
                        &timerDisplay, &timerSound, 
                        NULL };

Display *display;

int encodeTestStripSettings(uint8_t *msg) {
  int len;

  // informazioni sulla modalita' e sugli stop
  if (tico.settings.testStripFStop == 0) {
    msg[0] = D_SEG_L;
    msg[1] = D_SEG_I;
    msg[2] = D_SEG_N | D_SEG_POINT;
    len = 3;
  } else {
    msg[0] = D_SEG_F;
    msg[1] = D_SEG_MINUS;
    msg[2] = D_SEG_S | D_SEG_POINT;
    if (tico.settings.testStripFStop == 1) {
      msg[4] = D_SEG_1;
      len = 5;
    } else {
      msg[4] = D_SEG_1;
      msg[5] = D_SEG_MINUS;
      if (tico.settings.testStripFStop == 10) {
        msg[6] = D_SEG_1;
        msg[7] = D_SEG_0;
        len = 8;
      } else {
        msg[6] = display->encodeDigit(tico.settings.testStripFStop);
        len = 7;
      }
    }
  }
  
  return len;
}

void onDisplayInfoMessageDone() {
  tico.isMessageInfoVisible = false;
  displayCurrentMode();
}

void displayTimerModeInfoMessage() {
  uint8_t *msg = NULL;
  uint8_t len = 0;

  switch (tico.settings.timerMode)
  {
    case TimerModes::EnlargerTimer:
      msg = msg_INFO_TIMERMODE_EnlargerTimer;
      len = sizeof(msg_INFO_TIMERMODE_EnlargerTimer);
      break;
    case TimerModes::Devel:
      msg = msg_INFO_TIMERMODE_Devel;
      len = sizeof(msg_INFO_TIMERMODE_Devel);
      if (tico.settings.develMode == DevelMode::Linear) {
        msg[4] = D_SEG_L;
        msg[5] = D_SEG_I;
        msg[6] = D_SEG_N | D_SEG_POINT;
      } else {
        msg[4] = D_SEG_F;
        msg[5] = D_SEG_A;
        msg[6] = D_SEG_T | D_SEG_POINT;
      }
      break;
    case TimerModes::FactorialCalculator:
      msg = msg_INFO_TIMERMODE_FactorialCalculator;
      len =  encodeTestStripSettings(msg + 5) + 5;
      break;
    case TimerModes::Metronome:
      msg = msg_INFO_TIMERMODE_Metronome;
      len = sizeof(msg_INFO_TIMERMODE_Metronome);
      break;
    case TimerModes::TestStrips:
      msg = msg_INFO_TIMERMODE_TestStrips;
      len = encodeTestStripSettings(msg + 3) + 3;
      break;
  }

  if (msg != NULL) {
    tico.isMessageInfoVisible = true;
    clearDisplay();
    display->showMessage(msg, len, onDisplayInfoMessageDone, 1);
  }
}

void displayMessage(uint8_t *msg) {
  for(unsigned int i=0;i < sizeof(tico.displayBuff); i++) {
    tico.displayBuff[i] = msg == NULL ? D_SEG_OFF : msg[i];
  }
}

void displayInteger(int number, uint8_t pointChar = 4) {
  byte digit = number / 1000;
  tico.displayBuff[0] = digit == 0 ? D_SEG_OFF : display->encodeDigit (digit);
  digit = (number / 100) % 10;
  tico.displayBuff[1] = digit == 0 && tico.displayBuff[0] == D_SEG_OFF ? D_SEG_OFF : display->encodeDigit (digit);
  digit = (number / 10) % 10;
  tico.displayBuff[2] = pointChar >= 4 && digit == 0 && tico.displayBuff[1] == D_SEG_OFF ? D_SEG_OFF : display->encodeDigit (digit);
  digit = (number) % 10;
  tico.displayBuff[3] = display->encodeDigit (digit);

  if (pointChar < 4) {
    tico.displayBuff[pointChar] = tico.displayBuff[pointChar] | D_SEG_POINT;
  }
}

// display a time value or the word OFF if time is zero
void displayTimeOrOff(unsigned long time, int precision = 1000) {
  if (time == 0) {
    displayMessage(msg_OFF);
  } else {
    displayInteger(time / precision);
  }
}

void clearDisplay() {
  displayMessage(NULL);
}

void onTimerStatusChanged (Watch::Event *evt) {
  switch (tico.timerState)
  {
    case TimerStates::RunningDown:
    case TimerStates::RunningUp:
      timerDisplay.change(100, evt->currentMillis, true);
      tico.buzzer.sound = soundStart;
      if (tico.currentPrintStep == PrintSteps::Printing) {
        tico.setRele(HIGH, evt->currentMillis);
      }
      break;
    default:
      timerDisplay.change(100, evt->currentMillis, false);

      if (tico.timerState == TimerStates::Settings) {
          displayCurrentParDes();
          tico.setRele(LOW, evt->currentMillis);
      } else if (tico.timerState == TimerStates::Stopped) {
          tico.settings.timerMode = tico.timerStartMode;
          displayCurrentMode();
          tico.setRele(LOW, evt->currentMillis);
      } else if (tico.timerState == TimerStates::FocusOn) {
          tico.buzzer.sound = soundAck;
          tico.setRele(HIGH, evt->currentMillis);
          displayMessage(msg_FOCUSON);
      }
  }
}

void onCurrentPrintStepChanged (Watch::Event *evt) {
  switch (tico.currentPrintStep)
  {
    case PrintSteps::Printing:
      tico.buzzer.sound = soundStart;
      tico.setRele(HIGH, evt->currentMillis);
      break;
    case PrintSteps::TestStripCountDown:
    case PrintSteps::PrintingCountDown:
    case PrintSteps::Fix1CountDown:
    case PrintSteps::Fix2CountDown:
    case PrintSteps::StopCountDown:
      tico.buzzer.sound = soundStartCountDown;
      tico.setRele(LOW, evt->currentMillis);
      break;
    default:
      tico.buzzer.sound = soundStart;
      tico.setRele(LOW, evt->currentMillis);
      break;
  }
}

void onCurrentParChanged (Watch::Event *evt) {
  displayCurrentParDes();
}

void onSettingsChanged (Watch::Event *evt) {
  if (tico.timerState == Stopped) {
    EEPROM.put(EE_ADDR_TIMER_SETTINGS, tico.settings);
    tico.timerStartMode = tico.settings.timerMode;
    if (!tico.isMessageInfoVisible) {
      displayCurrentMode();
    }
  } else if (tico.timerState == Settings) {
    EEPROM.put(EE_ADDR_TIMER_SETTINGS, tico.settings);
    switch (tico.currentPar)
    {
      case PARAM_BRIGHTNESS:
        display->setBrightness(tico.settings.brightness);
        break;
    }
    displayCurrentParValue();
  }
}

void onEncoderChanged (Watch::Event *evt) {
  long encoderReduced = encoderValue / ENCODER_TICKS;
  if (encoderReduced == tico.lastencoderValue)
    return;

  tico.isMessageInfoVisible = false;

  long newValue;
  int direction = encoderReduced > tico.lastencoderValue ? 1 : -1;
  if (tico.timerChangeDirection != direction || (evt->currentMillis - tico.timerLastChange) > 500) {
    tico.timerChangeCount = 0;
  } else if (tico.timerChangeCount < ENCODER_ACCEL_THRESHOLD) {
    tico.timerChangeCount += 1;
  }
  tico.timerChangeDirection = direction;
  tico.timerLastChange = evt->currentMillis;
  tico.lastencoderValue = encoderReduced;

  if (tico.timerState == Stopped)
  {
    long step = 1000;

    if (tico.settings.timerMode == TimerModes::EnlargerTimer) {
      if (tico.settings.printTime < FIX_THRESHOLD / 2)
        step = 100;
      else if (tico.settings.printTime <= FIX_THRESHOLD)
        step = 500;
    }
    
    step = min(ENCODER_MAX_TIMER_STEP, step * (tico.timerChangeCount >= ENCODER_ACCEL_THRESHOLD ? 10 : 1));
    if (tico.timerAdd != step) {
      tico.timerAdd = step;
      if (tico.settings.timerMode == TimerModes::Devel && tico.settings.develMode == DevelMode::Linear) {
        tico.settings.develTime = max(PARAM_TIMER_DEVEL_MIN, (tico.settings.develTime / tico.timerAdd ) * tico.timerAdd );
      } else if (tico.settings.timerMode == TimerModes::EnlargerTimer) {
        tico.settings.printTime = max(PARAM_TIMER_ENLARGER_MIN, (tico.settings.printTime / tico.timerAdd ) * tico.timerAdd);
      }
    }

    switch (tico.settings.timerMode)
    {
      case TimerModes::Devel:
        if (tico.settings.develMode == DevelMode::Factorial) {
          tico.settings.develFactor =  min(PARAM_TIMER_FACT_MAX, max(PARAM_TIMER_FACT_MIN, tico.settings.develFactor + ((float) direction * 0.5)));
        } else {
          tico.settings.develTime = min(PARAM_TIMER_DEVEL_MAX, max(PARAM_TIMER_DEVEL_MIN, ((long)tico.settings.develTime + direction * tico.timerAdd)));
        }
        break;
      case TimerModes::TestStrips:
        tico.settings.testStripBaseTime = min(PARAM_TIMER_TESTSTRIP_MAX, max(PARAM_TIMER_TESTSTRIP_MIN, tico.settings.testStripBaseTime + 1000 * direction));
        break;
      case TimerModes::FactorialCalculator:
        tico.testStripCount = min(99, max(0, tico.testStripCount + direction));
        displayCurrentMode(); // aggiorno il display dato che non avviene automaticamente come per i settings
        break;
      case TimerModes::EnlargerTimer:
        newValue = ((long)tico.settings.printTime + direction * tico.timerAdd);

        if (tico.settings.printTime >= FIX_THRESHOLD / 2 && newValue < FIX_THRESHOLD / 2) {
          tico.settings.printTime = FIX_THRESHOLD / 2 - 100; // se passo dalla soglia di regolazione dei secondi ai 1/100 di secondo parto dal valore massimo - 100ms
        } else if (tico.settings.printTime < FIX_THRESHOLD / 2 && newValue > FIX_THRESHOLD / 2) {
          tico.settings.printTime = FIX_THRESHOLD / 2; // se passo dalla soglia di regolazione dei 1/100s ai secondi riparto dalla soglia
        } else if (tico.settings.printTime >= FIX_THRESHOLD && newValue < FIX_THRESHOLD) {
          tico.settings.printTime = FIX_THRESHOLD - 500; // se passo dalla soglia di regolazione dei secondi ai 1/100 di secondo parto dal valore massimo - 100ms
        } else if (tico.settings.printTime < FIX_THRESHOLD && newValue > FIX_THRESHOLD) {
          tico.settings.printTime = FIX_THRESHOLD; // se passo dalla soglia di regolazione dei 1/100s ai secondi riparto dalla soglia
        } else {
          tico.settings.printTime = min(PARAM_TIMER_ENLARGER_MAX, max(PARAM_TIMER_ENLARGER_MIN, newValue));
        }
        break;
      case TimerModes::Metronome:
        tico.settings.bps = min(PARAM_BPM_MAX, max(PARAM_BPM_MIN, tico.settings.bps + direction));
        break;
    }

  } else if (tico.timerState == Settings) {
    switch (tico.currentPar)
    {
      case PARAM_BRIGHTNESS:
        tico.settings.brightness = min(max(1, tico.settings.brightness + direction), 7);
        break;
      case PARAM_TSTOP:
        tico.settings.stopTime = min(PARAM_TIMER_STOP_MAX, max(PARAM_TIMER_STOP_MIN, (long)tico.settings.stopTime + direction * 1000));
        break;
      case PARAM_TFIX1:
        tico.settings.fix1Time = min(PARAM_TIMER_FIX_MAX, max(PARAM_TIMER_FIX_MIN, (long)tico.settings.fix1Time + direction * 1000));
        break;
      case PARAM_TFIX2:
        tico.settings.fix2Time = min(PARAM_TIMER_FIX_MAX, max(PARAM_TIMER_FIX_MIN, (long)tico.settings.fix2Time + direction * 1000));
        break;        
      case PARAM_TESTSTRIP_STRIPCOUNT: 
        tico.settings.testStripStripCount = min(PARAM_TESTSTRIP_STRIPCOUNT_MAX, max(PARAM_TESTSTRIP_STRIPCOUNT_MIN, tico.settings.testStripStripCount + direction));
        break;        
      case PARAM_TESTSTRIP_CDOWN: 
        tico.settings.testStripCountDownTime = min(PARAM_TESTSTRIP_COUNTDOWN_MAX, max(PARAM_TESTSTRIP_COUNTDOWN_MIN, (long)tico.settings.testStripCountDownTime + direction * 1000));
        break;
      case PARAM_TESTSTRIP_FSTOP: 
        tico.settings.testStripFStop = min(PARAM_TESTSTRIP_FSTOP_MAX, max(PARAM_TESTSTRIP_FSTOP_MIN, (long)tico.settings.testStripFStop + direction));
        break;        
      case PARAM_TESTSTRIP_PRETIME: 
        tico.settings.testStripPreExpos = min(PARAM_TESTSTRIP_PREEXPOS_MAX, max(PARAM_TESTSTRIP_PREEXPOS_MIN, (long)tico.settings.testStripPreExpos + direction * 1000));
        break;        
      case PARAM_TESTSTRIP_MODE: 
        tico.settings.testStripTimeCalc = min(PARAM_TESTSTRIP_TIMECALC_MAX, max(PARAM_TESTSTRIP_TIMECALC_MIN, (long)tico.settings.testStripTimeCalc + direction));
        break;        
      case PARAM_CDOWN:
        tico.settings.countDownTime = min(PARAM_TIMER_COUNTDOWN_MAX, max(PARAM_TIMER_COUNTDOWN_MIN, (long)tico.settings.countDownTime + direction * 1000));
        break;
      case PARAM_RESET:
        if (tico.reset == 0 && direction < 0)
          tico.reset = 3;
        else
          tico.reset = (tico.reset + direction) % 4;
        displayCurrentParValue();
        break;
      case PARAM_SOUND:
        tico.settings.sounds = (SoundMode) (((int)tico.settings.sounds + direction) % SoundModeCount);
        break;
      case PARAM_AFTER_PRINT:
        if ((int)tico.settings.afterPrintGoTo == 0 && direction < 0)
          tico.settings.afterPrintGoTo = (TimerModes)1;
        else
          tico.settings.afterPrintGoTo = (TimerModes)(((int)tico.settings.afterPrintGoTo + direction) % 2);
        break;
      case PARAM_AFTER_DEVEL:
        if (tico.settings.afterDevelGoTo == TimerModes::Devel)
          tico.settings.afterDevelGoTo = (TimerModes)TimerModes::Metronome;
        else
          tico.settings.afterDevelGoTo = TimerModes::Devel;
        break;    
      case PARAM_DEVEL_MODE:
        if ((int)tico.settings.develMode == 0 && direction < 0)
          tico.settings.develMode = DevelMode::Factorial;
        else
          tico.settings.develMode = (DevelMode)(((int)tico.settings.develMode + direction) % 2);
        break;
      case PARAM_STATISTICS:
        if (tico.currentStat == 0 && direction < 0)
          tico.currentStat = 1;
        else
          tico.currentStat = (tico.currentStat + direction) % 2;
        displayCurrentParValue();
        break;  
    }
  }
}

void onBuzzerChanged (Watch::Event *evt) {
  if (tico.settings.sounds != SoundMode::Quiet) {
    if (tico.buzzer.sound == NULL) {
      timerSound.change(50, 0, false);
    } else {
      tico.buzzer.soundIndex = 1;
      tico.buzzer.soundStartMillis = evt->currentMillis;
      unsigned int freq = tico.buzzer.sound[1];
      if (freq == 0)
        noTone(buzzerPin);
      else
        tone(buzzerPin, freq);
      timerSound.change(50, evt->currentMillis, true);
    }
  }
}

void onTimerSoundElapsed (Watch::Event *evt) {
  if (evt->currentMillis - tico.buzzer.soundStartMillis > tico.buzzer.sound[tico.buzzer.soundIndex * 2]) {
    if (tico.buzzer.soundIndex < tico.buzzer.sound[0]) {
      tico.buzzer.soundIndex++;
      unsigned int freq = tico.buzzer.sound[tico.buzzer.soundIndex * 2 -1];
      if (freq == 0)
        noTone(buzzerPin);
      else
        tone(buzzerPin, freq);
      
      tico.buzzer.soundStartMillis = evt->currentMillis;
    } else {
      // finish
      noTone(buzzerPin);
      tico.buzzer.sound = NULL;
    }
  }
}

void onTimerDisplayElapsed (Watch::Event *evt) {
  unsigned long number;
 
  if (tico.isPaused()) {
    if (((evt->currentMillis - tico.timerPausedStart) / 500) % 2 == 1) {
      clearDisplay();
      return;
    } else {
      number = evt->currentMillis - tico.timerPausedStart;
    }
  } else {
    number = evt->currentMillis - tico.timerStartValue;
    if (tico.timerState == RunningDown) {
      number = tico.timerInterval - number;
    }
  }
  
  displayInteger(number / 100, 2);
  if ((tico.timerState == RunningDown || tico.timerState == RunningUp) && ((number / 100) % 10 <= 1 || tico.isPaused())) {
    switch (tico.currentPrintStep) {
      case PrintSteps::Printing:
        if (tico.settings.timerMode == TimerModes::TestStrips)
          tico.displayBuff[0] = D_SEG_P;
        else if (tico.settings.timerMode == TimerModes::Metronome)
          tico.displayBuff[0] = D_SEG_B;
        else
          tico.displayBuff[0] = D_SEG_I;
        break;
      case PrintSteps::Development:
        tico.displayBuff[0] = D_SEG_S;
        break;
      case PrintSteps::Stop:
        tico.displayBuff[0] = D_SEG_A;
        break;
      case PrintSteps::Fix1:
        tico.displayBuff[0] = D_SEG_F;
        break;
      case PrintSteps::Fix2:
        tico.displayBuff[0] = D_SEG_F | D_SEG_POINT;
        break;
      case PrintSteps::Counter:
        tico.displayBuff[0] = D_SEG_B | D_SEG_POINT;
        break;
      case PrintSteps::TestStripCountDown:
      case PrintSteps::PrintingCountDown:
      case PrintSteps::StopCountDown:
      case PrintSteps::Fix1CountDown:
      case PrintSteps::Fix2CountDown:
        tico.displayBuff[0] = D_SEG_C | D_SEG_POINT;
        break;
    }
  }
}

void onWelcomeMessageDone() {
  displayTimerModeInfoMessage();
}

void onSettingMessageDone() {
  tico.currentPar = 0;
  tico.timerState = Settings;
}

void onButton1Clicked (Watch::Event *evt) {
  Watch::Button *btn = (Watch::Button *)evt;

  if (btn->state == BTN_CLICK && tico.isPaused()) {
    tico.resume(evt->currentMillis);
  } else if (btn->state == BTN_CLICK 
             && tico.settings.timerMode != TimerModes::Metronome
             && tico.settings.timerMode != TimerModes::TestStrips
             && tico.timerState == TimerStates::RunningDown) {
    tico.pause(evt->currentMillis);
  } else if (btn->state == BTN_LONG_CLICK 
             && (tico.settings.timerMode == TimerModes::EnlargerTimer || tico.settings.timerMode == TimerModes::Metronome)
             && tico.timerState == TimerStates::Stopped) {
    // focus
    tico.toggleFocus();
  } else if (btn->state == BTN_CLICK && tico.timerState == TimerStates::FocusOn) {
    tico.toggleFocus();
  } else {
    switch (tico.settings.timerMode)
    {
      case TimerModes::FactorialCalculator:
        if (btn->state == BTN_CLICK) {
          tico.settings.timerMode = TimerModes::TestStrips;
          displayTimerModeInfoMessage();
        }
        break;
      case TimerModes::EnlargerTimer:
        if (btn->state == BTN_CLICK) {
          if (tico.timerState == TimerStates::Stopped) {
            tico.startPrint(btn->currentMillis);
          }
        } else if (btn->state == BTN_LONG_CLICK && tico.timerState != TimerStates::Stopped) {
          tico.cancel();
        }
        break;
      case TimerModes::TestStrips:
        if (btn->state == BTN_CLICK) {
          if (tico.timerState == TimerStates::Stopped) {
            tico.startTestStrip(btn->currentMillis);
          } else {
            tico.endTestStrip(btn->currentMillis);
          }
        } else if (btn->state == BTN_LONG_CLICK) {
          if (btn->state == BTN_LONG_CLICK && tico.timerState == TimerStates::Stopped) {
            tico.testStripCount = 0;
            tico.settings.timerMode = TimerModes::FactorialCalculator;
            displayTimerModeInfoMessage();
          } else {
            tico.cancel();
          }
        }
        break;
      case TimerModes::Devel:
          if (tico.timerState == TimerStates::Stopped) {
            if (btn->state == BTN_CLICK) {
              if (tico.settings.develMode == DevelMode::Linear)
              {
                tico.timerInterval = tico.settings.develTime;
                tico.timerState = TimerStates::RunningDown;
              }
              else
              {
                tico.timerInterval = 0;
                tico.timerState = TimerStates::RunningUp;
              }
              tico.currentPrintStep = PrintSteps::Development;
              tico.timerStartValue = btn->currentMillis;
            }
          } else if (tico.timerState == TimerStates::RunningDown) {
            if (btn->state == BTN_LONG_CLICK) {
              tico.cancel();
            }
          } else if (tico.timerState == TimerStates::RunningUp) {
            if (btn->state == BTN_CLICK) {
              if (tico.currentPrintStep == PrintSteps::Development) {
                tico.timerInterval = (btn->currentMillis - tico.timerStartValue) * (tico.settings.develFactor - 1);
                tico.timerState = TimerStates::RunningDown;
                tico.timerStartValue = btn->currentMillis;
                tico.buzzer.sound = soundStart;
              } else {
                tico.stop();
              }
            } else if (btn->state == BTN_LONG_CLICK) {
              tico.cancel();
            }
        }
        break;
      case TimerModes::Metronome:
        if (tico.timerState == TimerStates::Stopped) {
          tico.timerState = TimerStates::RunningUp;
          tico.currentPrintStep = PrintSteps::Printing;
          tico.timerStartValue = btn->currentMillis;
          tico.buzzer.sound = soundStart;
        } else if (tico.timerState == TimerStates::RunningUp) {
          tico.stop();
        }
        break;
    }
  }
}

void onButton2Clicked (Watch::Event *evt) {
  Watch::Button *btn = (Watch::Button *)evt;

  if (tico.timerState == Stopped) {
    if (btn->state == BTN_CLICK) {
      int newMode = ((int)tico.settings.timerMode) + 1;
      if (newMode > MAX_TIMER_MODES)
        newMode = 0;
      tico.settings.timerMode = (TimerModes)newMode;
      tico.buzzer.sound = soundWelcome;
      displayTimerModeInfoMessage();
    } else if (btn->state == BTN_LONG_CLICK) {
      display->showMessage(msg_IMPOSTAZIONI, sizeof(msg_IMPOSTAZIONI), onSettingMessageDone, 1);
      tico.buzzer.sound = soundAck;
    }
  } else if (tico.timerState == Settings) {
    if (btn->state == BTN_CLICK) {
      tico.currentPar = (tico.currentPar + 1) % PARAM_COUNT;
      tico.buzzer.sound = soundAck;
    } else if (btn->state == BTN_LONG_CLICK) {
      if (tico.reset > 0) {
        if (tico.reset == 1 || tico.reset == 3)
          tico.resetSettings();
        if (tico.reset == 2 || tico.reset == 3)
          tico.resetStatistics();
        EEPROM.put(EE_ADDR_TIMER_SETTINGS, tico.settings);
        tico.buzzer.sound = soundReset;
        tico.reset = 0;
      }
      tico.timerState = Stopped;
    }
  }
}

void onButtonFocusClicked (Watch::Event *evt) {
  Watch::Button *btn = (Watch::Button *)evt;
 
  if (btn->state == BTN_CLICK) {
    if (tico.settings.timerMode == TimerModes::Devel && (tico.timerState == TimerStates::RunningDown || tico.timerState == TimerStates::RunningUp)) {
      // skip step
      tico.skipCurrentStep = true;
    } else {
      tico.toggleFocus();
    }
  }
}

void gotoDevel(unsigned long currentMillis) {
  switch (tico.settings.afterPrintGoTo)
  {
    case TimerModes::Devel:
      tico.startDevel(currentMillis);
      break;
    default:
      // questo caso non si dovrebbe mai verificare
      tico.timerState = Stopped; 
      break;
  }
}

void updateStatus(unsigned long currentMillis)
{
  if (tico.isPaused())
    return;

  switch (tico.settings.timerMode)
  {
    case TimerModes::EnlargerTimer:
    case TimerModes::Devel:
    case TimerModes::TestStrips:
      if (tico.timerState == TimerStates::RunningDown)
      {
        unsigned long elapsed = currentMillis - tico.timerStartValue;
        bool isSkipping = tico.skipCurrentStep;

        tico.skipCurrentStep = false; // reset eventuale flag di skip
        if (elapsed >= tico.timerInterval || isSkipping) {
          // set next step
          switch (tico.currentPrintStep)
          {
            case PrintSteps::Printing:
              if (tico.settings.timerMode == TimerModes::TestStrips 
                  && tico.testStripDone == false)  {
                // imposto la prossima striscia di test
                if (!tico.setNextStepCountDown(currentMillis, PrintSteps::TestStripCountDown)) {
                  tico.nextTestStrip(currentMillis);
                }
              } else {
                if (tico.settings.afterPrintGoTo != TimerModes::EnlargerTimer) {
                  if (!tico.setNextStepCountDown(currentMillis, PrintSteps::PrintingCountDown)) {
                    gotoDevel(currentMillis);
                  }
                } else {
                  tico.timerState = Stopped;
                }
              }
              break;
            case PrintSteps::TestStripCountDown:
              if (tico.testStripDone) {
                if (tico.settings.afterPrintGoTo != TimerModes::EnlargerTimer) {
                  if (!tico.setNextStepCountDown(currentMillis, PrintSteps::PrintingCountDown)) {
                    gotoDevel(currentMillis);
                  }
                } else {
                  tico.timerState = Stopped;
                }
              } else {
                tico.nextTestStrip(currentMillis);
              }
              break;
            case PrintSteps::PrintingCountDown:
              gotoDevel(currentMillis);
              break;
            case PrintSteps::Development:
              if (tico.settings.stopTime != 0) {
                if (isSkipping || !tico.setNextStepCountDown(currentMillis, PrintSteps::StopCountDown)) {
                  tico.currentPrintStep = PrintSteps::Stop;
                  tico.timerInterval = tico.settings.stopTime;
                }
              } else if (tico.settings.fix1Time != 0) {
                if (isSkipping || !tico.setNextStepCountDown(currentMillis, PrintSteps::Fix1CountDown)) {
                  tico.currentPrintStep = PrintSteps::Fix1;
                  tico.timerInterval = tico.settings.fix1Time;
                }
              } else if (tico.settings.fix2Time != 0) {
                if (isSkipping || !tico.setNextStepCountDown(currentMillis, PrintSteps::Fix2CountDown)) {
                  tico.currentPrintStep = PrintSteps::Fix2;
                  tico.timerInterval = tico.settings.fix2Time;
              }
              } else if (tico.settings.afterDevelGoTo == TimerModes::Metronome) {
                tico.startCounter(currentMillis);
              } else {
                tico.timerState = Stopped;
              }
              break;
            case PrintSteps::StopCountDown:
              tico.currentPrintStep = PrintSteps::Stop;
              tico.timerInterval = tico.settings.stopTime;
              break;
            case PrintSteps::Stop:
              if (tico.settings.fix1Time != 0) {
                if (isSkipping || !tico.setNextStepCountDown(currentMillis, PrintSteps::Fix1CountDown)) {
                  tico.currentPrintStep = PrintSteps::Fix1;
                  tico.timerInterval = tico.settings.fix1Time;
                }
              } else if (tico.settings.fix2Time != 0) {
                if (isSkipping || !tico.setNextStepCountDown(currentMillis, PrintSteps::Fix2CountDown)) {
                  tico.currentPrintStep = PrintSteps::Fix2;
                  tico.timerInterval = tico.settings.fix2Time;
                }                
              } else if (tico.settings.afterDevelGoTo == TimerModes::Metronome) {
                tico.startCounter(currentMillis);
              } else {
                tico.timerState = Stopped;
              }
              break;
            case PrintSteps::Fix1CountDown:
              tico.currentPrintStep = PrintSteps::Fix1;
              tico.timerInterval = tico.settings.fix1Time;
              break;
            case PrintSteps::Fix2CountDown:
              tico.currentPrintStep = PrintSteps::Fix2;
              tico.timerInterval = tico.settings.fix2Time;
              break;              
            case Fix1:
              if (tico.settings.fix2Time != 0) {
                if (isSkipping || !tico.setNextStepCountDown(currentMillis, PrintSteps::Fix2CountDown)) {
                  tico.currentPrintStep = PrintSteps::Fix2;
                  tico.timerInterval = tico.settings.fix2Time;
                }
              } else if (tico.settings.afterDevelGoTo == TimerModes::Metronome) {
                tico.startCounter(currentMillis);
              } else {
                tico.timerState = Stopped;
              }
              break;
            case Fix2:
              if (tico.settings.afterDevelGoTo == TimerModes::Metronome) {
                tico.startCounter(currentMillis);
              } else {
                tico.timerState = Stopped;
              }
              break;
            case Counter:
            default:
              tico.timerState = Stopped;
              break;
          }
          tico.timerStartValue = currentMillis;
        } else if (tico.settings.sounds == SoundMode::All) {
          unsigned long countDown = (tico.timerInterval - elapsed);
          switch (tico.currentPrintStep)
          {
            case PrintSteps::Counter:
              if (tico.settings.sounds == SoundMode::All && (countDown  % 10000) < 50) {
                tico.buzzer.sound = soundCountDown;
              }
              break;
            case PrintSteps::Fix1CountDown:
            case PrintSteps::Fix2CountDown:
            case PrintSteps::StopCountDown:
            case PrintSteps::PrintingCountDown:
              if ((countDown  % 1000) < 50) {
                if (countDown <= 3050) {
                  // differenzio beep countdown negli ultimi 3 secondi
                  tico.buzzer.sound = soundCountDownHigh;
                } else {
                  tico.buzzer.sound = soundCountDown;
                }
              }
              break;
            default:
              if (tico.currentPrintStep == PrintSteps::Printing) {
                // beep ad ogni secondo se stampo
                if (countDown % 1000 < 50) {
                  tico.buzzer.sound = soundMetronome;
                }
              } else if (tico.settings.timerMode != TimerModes::TestStrips) {
                if (elapsed % 60000 < 50) {
                  // beep differenziato ogni minuto relativo all'inizio del count down
                  tico.buzzer.sound = soundRelativeMinute;
                } else if (elapsed % 30000 < 50) {
                  // beep differenziato ogni 30 secondi relativo all'inizio del count down
                  tico.buzzer.sound = soundRelativeHalfMinute;
                } else if (countDown < 60050) {
                  // nell'ultimo minuto
                  if (countDown % 60000 < 50) {
                    // beep differenziato ogni minuto
                    tico.buzzer.sound = soundMinute;
                  } else if (countDown % 30000 < 50) {
                    // beep differenziato ogni 30 secondi
                    tico.buzzer.sound = soundHalfMinute;
                  } else if (countDown % 15000 < 50 && (countDown > 14950 && countDown <= 15050)) {
                    // beep avvertimento ai 15 secondi
                    tico.buzzer.sound = sound15Sec;                  
                  } else if ((countDown > 100 && countDown <= 10050) && (countDown % 1000) < 50) {
                    // beep ad ogni secondo se stampo oppure negli ultimi 10 secondi di ogni bagno
                    tico.buzzer.sound = soundMetronome;
                  }
                }
              } else if (tico.settings.testStripCountDownTime == 0) {
                // se la pausa e' disattivata faccio il count down degli ultimi 5 secondi
                if (countDown > 100 && countDown <= 5050 && countDown  % 1000 < 50) {
                  tico.buzzer.sound = soundMetronome;
                }
              }
            case PrintSteps::TestStripCountDown:
            break;
          }
        }
      }
      break;
    case TimerModes::Metronome:
      if (tico.timerState == TimerStates::RunningUp) {
        double f = 1000.0 / (double)tico.settings.bps;
        unsigned long elapsed = (currentMillis - tico.timerStartValue) % (unsigned long)f;
       
        if (elapsed < 20) {
          tico.buzzer.sound = soundMetronome;
        }
      }
      break;
    case TimerModes::FactorialCalculator:
      break;
  }
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

void onDisplayChanged(Watch::Event *evt) {
  if (!tico.isMessageInfoVisible)
    display->showMessage(tico.displayBuff, sizeof(tico.displayBuff));
}

void displayCurrentMode() {
  switch (tico.settings.timerMode)
  {
    case TimerModes::Devel:
      if (tico.settings.develMode == DevelMode::Factorial) {
        displayInteger((unsigned long) (tico.settings.develFactor * 10), 2);
        tico.displayBuff[0] = D_SEG_F | D_SEG_POINT;
      } else {
        displayInteger(tico.settings.develTime / 1000);
        tico.displayBuff[0] = D_SEG_S | D_SEG_POINT;
      }
      break;
    case TimerModes::EnlargerTimer:
      if (tico.settings.printTime < FIX_THRESHOLD)
        displayInteger(tico.settings.printTime / 100, 2);
      else
        displayInteger(tico.settings.printTime / 1000);
      tico.displayBuff[0] = D_SEG_I | D_SEG_POINT;
      break;
    case TimerModes::Metronome:
      displayInteger(tico.settings.bps);
      tico.displayBuff[0] = D_SEG_B | D_SEG_POINT;
      break;
    case TimerModes::TestStrips:
      displayInteger(tico.settings.testStripBaseTime / 1000);
      tico.displayBuff[0] = D_SEG_P | D_SEG_POINT;
      break;
    case TimerModes::FactorialCalculator:
      displayInteger((tico.settings.testStripPreExpos + tico.calculateCurrentStripTime()) / 100, 2);
      tico.displayBuff[0] = D_SEG_F | D_SEG_POINT;
      break;
  }
}

void displayCurrentParValue() {
  uint8_t *msg;

  switch (tico.currentPar)
  {
    case PARAM_BRIGHTNESS:
      displayInteger(tico.settings.brightness);
      break;
    case PARAM_TSTOP:
      displayTimeOrOff(tico.settings.stopTime);
      break;
    case PARAM_TFIX1:
      displayTimeOrOff(tico.settings.fix1Time);
      break;
    case PARAM_TFIX2:
      displayTimeOrOff(tico.settings.fix2Time);
      break;      
    case PARAM_CDOWN:
      if (tico.settings.countDownTime >= PARAM_TIMER_COUNTDOWN_MANUAL)
        displayMessage(msg_MAN);
      else
        displayTimeOrOff(tico.settings.countDownTime);
      break;
    case PARAM_TESTSTRIP_FSTOP:
      if (tico.settings.testStripFStop == 0) {
        displayMessage(msg_LINEARE);
      } else if (tico.settings.testStripFStop < 10) {
        displayInteger(tico.settings.testStripFStop);
        tico.displayBuff[0] = D_SEG_F | D_SEG_POINT;
        tico.displayBuff[1] = D_SEG_1;
        tico.displayBuff[2] = D_SEG_MINUS;
      } else {
        displayInteger(tico.settings.testStripFStop);
        tico.displayBuff[0] = D_SEG_1;
        tico.displayBuff[1] = D_SEG_MINUS;
      }
      break;   
    case PARAM_TESTSTRIP_STRIPCOUNT:
      if (tico.settings.testStripStripCount > 0)
        displayInteger(tico.settings.testStripStripCount);
      else
        displayMessage(msg_MAN);
      break;           
    case PARAM_TESTSTRIP_CDOWN:
      if (tico.settings.testStripCountDownTime >= PARAM_TESTSTRIP_COUNTDOWN_MANUAL)
        displayMessage(msg_MAN);
      else
        displayTimeOrOff(tico.settings.testStripCountDownTime);
      break;
    case PARAM_TESTSTRIP_MODE:
      if (tico.settings.testStripTimeCalc == PARAM_TESTSTRIP_TIMECALC_SINGLE)
        displayMessage(msg_SING);
      else
        displayMessage(msg_DIFF);
      break;  
    case PARAM_TESTSTRIP_PRETIME:
      displayTimeOrOff(tico.settings.testStripPreExpos);
      break;      
    case PARAM_RESET:
      if (tico.reset == 1) {
        msg = msg_RESET_PARAMS;
      } else if (tico.reset == 2) {
        msg = msg_RESET_STATS;
      } else if (tico.reset == 3) {
        msg = msg_RESET_ALL;
      } else {
        msg = msg_NO;
      }
      for(unsigned int i=0;i < sizeof(tico.displayBuff); i++) {
        tico.displayBuff[i] = msg[i];
      }
      break;
    case PARAM_SOUND:
      switch (tico.settings.sounds)
      {
        case SoundMode::All:
          msg = msg_SI;
          break;
        case SoundMode::Quiet:
          msg = msg_NO;
          break;
        case SoundMode::StartStop:
          msg = msg_STARTSTOP;
          break;
      }
      displayMessage(msg);
      break;
    case PARAM_AFTER_PRINT:
      switch (tico.settings.afterPrintGoTo)
      {
        case TimerModes::EnlargerTimer:
          msg = msg_ENLARGER;
          break;
        case TimerModes::Devel:
          msg = msg_DEVEL;
          break;
        default:
          break;
      }
   
      displayMessage(msg);
      break;
    case PARAM_AFTER_DEVEL:
      switch (tico.settings.afterDevelGoTo)
      {
        case TimerModes::Devel:
          msg = msg_STOP;
          break;
        case TimerModes::Metronome:
          msg = msg_CRON;
          break;
        default:
          break;
      }
   
      displayMessage(msg);
      break;
    case PARAM_DEVEL_MODE:
      switch (tico.settings.develMode)
      {
        case DevelMode::Linear:
          msg = msg_LINEARE;
          break;
        case DevelMode::Factorial:
          msg = msg_FAT;
          break;
        default:
          break;
      }
   
      displayMessage(msg);
      break;
    case PARAM_STATISTICS:
      if (tico.currentStat == 0)
      {
        int tmp = tico.settings.onTime / 1000;

        if (tmp < 60) {
          displayInteger(tmp);
          tico.displayBuff[0] = D_SEG_S | D_SEG_POINT;
        } else {
          displayInteger(tmp / 60);
          tico.displayBuff[0] = D_SEG_M | D_SEG_POINT;
        }
      } else {
        displayInteger(tico.settings.onCount);
        tico.displayBuff[0] = D_SEG_c | D_SEG_POINT;
      }
      break;
  }

  display->showMessage(tico.displayBuff, 4); // forzo refresh
}

void displayCurrentParDes() {
  switch (tico.currentPar)
  {
    case PARAM_BRIGHTNESS:
      display->showMessage(msg_PARAM_BRIGHTNESS, sizeof(msg_PARAM_BRIGHTNESS), displayCurrentParValue);
      break;
    case PARAM_TSTOP:
      display->showMessage(msg_PARAM_TSTOP, sizeof(msg_PARAM_TSTOP), displayCurrentParValue);
      break;
    case PARAM_TFIX1:
      display->showMessage(msg_PARAM_TFIX1, sizeof(msg_PARAM_TFIX1), displayCurrentParValue);
      break;
    case PARAM_TFIX2:
      display->showMessage(msg_PARAM_TFIX2, sizeof(msg_PARAM_TFIX2), displayCurrentParValue);
      break;      
    case PARAM_TESTSTRIP_FSTOP:
      display->showMessage(msg_PARAM_TESTSTRIP_FSTOP, sizeof(msg_PARAM_TESTSTRIP_FSTOP), displayCurrentParValue);
      break;
    case PARAM_TESTSTRIP_STRIPCOUNT:
      display->showMessage(msg_PARAM_TESTSTRIP_STRIPCOUNT, sizeof(msg_PARAM_TESTSTRIP_STRIPCOUNT), displayCurrentParValue);
      break;       
    case PARAM_TESTSTRIP_CDOWN:
      display->showMessage(msg_PARAM_TESTSTRIP_COUNTDOWN, sizeof(msg_PARAM_TESTSTRIP_COUNTDOWN), displayCurrentParValue);
      break;   
    case PARAM_TESTSTRIP_PRETIME:
      display->showMessage(msg_PARAM_TESTSTRIP_PRETIME, sizeof(msg_PARAM_TESTSTRIP_PRETIME), displayCurrentParValue);
      break; 
    case PARAM_TESTSTRIP_MODE:
      display->showMessage(msg_PARAM_TESTSTRIP_TIMECALC, sizeof(msg_PARAM_TESTSTRIP_TIMECALC), displayCurrentParValue);
      break;    
    case PARAM_CDOWN:
      display->showMessage(msg_PARAM_TCOUNTDOWN, sizeof(msg_PARAM_TCOUNTDOWN), displayCurrentParValue);
      break;
    case PARAM_RESET:
      display->showMessage(msg_PARAM_RESET, sizeof(msg_PARAM_RESET), displayCurrentParValue);
      break;
    case PARAM_SOUND:
      display->showMessage(msg_PARAM_SOUND, sizeof(msg_PARAM_SOUND), displayCurrentParValue);
      break;
    case PARAM_AFTER_PRINT:
      display->showMessage(msg_PARAM_AFTER_PRINT, sizeof(msg_PARAM_AFTER_PRINT), displayCurrentParValue);
      break;
    case PARAM_AFTER_DEVEL:
      display->showMessage(msg_PARAM_AFTER_DEVEL, sizeof(msg_PARAM_AFTER_DEVEL), displayCurrentParValue);
      break;     
    case PARAM_DEVEL_MODE:
      display->showMessage(msg_PARAM_DEVEL_MODE, sizeof(msg_PARAM_DEVEL_MODE), displayCurrentParValue);
      break;
    case PARAM_STATISTICS:
      display->showMessage(msg_PARAM_STATISTICS, sizeof(msg_PARAM_STATISTICS), displayCurrentParValue);
      break;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);

  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

  pinMode(rele1Pin, OUTPUT);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT);
  pinMode(buttonFocusPin, INPUT_PULLUP);

  EEPROM.get(EE_ADDR_TIMER_SETTINGS, tico.settings);
  if (tico.settings.magic != TICO_MAGIC_SIGNATURE) {
    tico.resetSettings();
    tico.resetStatistics();
  }

  tico.validateSettings();
  tico.timerStartMode = tico.settings.timerMode;

  display = new Display(displayClkPin, displayDIOPin, tico.settings.brightness);
  display->showMessage(msg_TICO, sizeof(msg_TICO), onWelcomeMessageDone, 1);

  Watch::setup(ctx, millis());
  tico.buzzer.sound = soundWelcome;
  Serial.println("TiCO is ready");
}


void loop(){
  unsigned long currentMillis = millis();

  Watch::loop(ctx, currentMillis);

  updateStatus(currentMillis);

  display->loop(currentMillis);
}
