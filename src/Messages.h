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

#include "Display.h"

#ifndef MESSAGES_H
#define MESSAGES_H

uint8_t msg_TICO[] = { D_SEG_T, D_SEG_i | D_SEG_POINT, D_SEG_C | D_SEG_POINT, D_SEG_O | D_SEG_POINT, D_SEG_OFF, D_SEG_1 | D_SEG_POINT, D_SEG_5 };
uint8_t msg_IMPOSTAZIONI[] = { D_SEG_I, D_SEG_M, D_SEG_P, D_SEG_O, D_SEG_S, D_SEG_T, D_SEG_A, D_SEG_Z, D_SEG_I, D_SEG_O, D_SEG_N, D_SEG_I };
uint8_t msg_PARAM_BRIGHTNESS[] = { D_SEG_L, D_SEG_U, D_SEG_M, D_SEG_I, D_SEG_N, D_SEG_O, D_SEG_S, D_SEG_I, D_SEG_T, D_SEG_A };
uint8_t msg_PARAM_TSTOP[] = { D_SEG_T | D_SEG_POINT, D_SEG_OFF, D_SEG_S, D_SEG_T, D_SEG_O, D_SEG_P };
uint8_t msg_PARAM_TFIX1[] = { D_SEG_T | D_SEG_POINT, D_SEG_OFF, D_SEG_F, D_SEG_I, D_SEG_S, D_SEG_S, D_SEG_A, D_SEG_G, D_SEG_G, D_SEG_I, D_SEG_O, D_SEG_OFF, D_SEG_1 };
uint8_t msg_PARAM_TFIX2[] = { D_SEG_T | D_SEG_POINT, D_SEG_OFF, D_SEG_F, D_SEG_I, D_SEG_S, D_SEG_S, D_SEG_A, D_SEG_G, D_SEG_G, D_SEG_I, D_SEG_O, D_SEG_OFF, D_SEG_2 };
uint8_t msg_PARAM_TCOUNTDOWN[] = { D_SEG_T | D_SEG_POINT, D_SEG_OFF, D_SEG_P, D_SEG_A, D_SEG_U, D_SEG_S, D_SEG_A };
uint8_t msg_PARAM_TESTSTRIP_STRIPCOUNT[] = { D_SEG_P, D_SEG_R | D_SEG_POINT, D_SEG_OFF, D_SEG_N, D_SEG_R};
uint8_t msg_PARAM_TESTSTRIP_COUNTDOWN[] = { D_SEG_P, D_SEG_R | D_SEG_POINT, D_SEG_OFF, D_SEG_P, D_SEG_A, D_SEG_U, D_SEG_S, D_SEG_A };
uint8_t msg_PARAM_TESTSTRIP_FSTOP[] = { D_SEG_P , D_SEG_R | D_SEG_POINT, D_SEG_OFF, D_SEG_F, D_SEG_MINUS, D_SEG_S, D_SEG_T, D_SEG_O, D_SEG_P };
uint8_t msg_PARAM_TESTSTRIP_PRETIME[] = { D_SEG_P, D_SEG_R | D_SEG_POINT, D_SEG_OFF, D_SEG_P, D_SEG_R, D_SEG_E, D_SEG_S, D_SEG_P | D_SEG_POINT};
uint8_t msg_PARAM_TESTSTRIP_TIMECALC[] = { D_SEG_P, D_SEG_R | D_SEG_POINT, D_SEG_OFF, D_SEG_T | D_SEG_POINT, D_SEG_C, D_SEG_A, D_SEG_L, D_SEG_C | D_SEG_POINT};
uint8_t msg_PARAM_RESET[] = { D_SEG_R , D_SEG_E, D_SEG_S, D_SEG_E, D_SEG_T};
uint8_t msg_PARAM_SOUND[] = { D_SEG_B , D_SEG_E, D_SEG_E, D_SEG_P, D_SEG_OFF};
uint8_t msg_PARAM_AFTER_PRINT[] = { D_SEG_D, D_SEG_O, D_SEG_P, D_SEG_O, D_SEG_OFF, D_SEG_S, D_SEG_T, D_SEG_A, D_SEG_M, D_SEG_P, D_SEG_A};
uint8_t msg_PARAM_AFTER_DEVEL[] = { D_SEG_D, D_SEG_O, D_SEG_P, D_SEG_O, D_SEG_OFF, D_SEG_S, D_SEG_V, D_SEG_I, D_SEG_L, D_SEG_U, D_SEG_P, D_SEG_P, D_SEG_O};
uint8_t msg_PARAM_DEVEL_MODE[] = { D_SEG_T , D_SEG_I, D_SEG_P, D_SEG_O, D_SEG_OFF, D_SEG_S, D_SEG_V, D_SEG_I, D_SEG_L, D_SEG_U, D_SEG_P, D_SEG_P, D_SEG_O};
uint8_t msg_PARAM_STATISTICS[] = { D_SEG_S, D_SEG_T, D_SEG_A, D_SEG_T, D_SEG_S | D_SEG_POINT };
uint8_t msg_PARAM_VOICE_CONTROL[] = { D_SEG_C | D_SEG_POINT, D_SEG_OFF, D_SEG_V, D_SEG_O, D_SEG_C, D_SEG_A, D_SEG_L, D_SEG_E };

uint8_t msg_SI[] = { D_SEG_OFF, D_SEG_S, D_SEG_I, D_SEG_OFF };
uint8_t msg_NO[] = { D_SEG_OFF, D_SEG_N, D_SEG_o, D_SEG_OFF };
uint8_t msg_OFF[] = { D_SEG_O, D_SEG_F, D_SEG_F, D_SEG_OFF };
uint8_t msg_MAN[] = { D_SEG_M, D_SEG_A, D_SEG_N | D_SEG_POINT, D_SEG_OFF };
uint8_t msg_LINEARE[] = { D_SEG_L, D_SEG_I, D_SEG_N | D_SEG_POINT, D_SEG_OFF };
uint8_t msg_FAT[] = { D_SEG_F, D_SEG_A, D_SEG_T, D_SEG_T | D_SEG_POINT };
uint8_t msg_STOP[] = { D_SEG_S, D_SEG_T, D_SEG_O, D_SEG_P };
uint8_t msg_CRON[] = { D_SEG_C, D_SEG_R, D_SEG_O, D_SEG_N | D_SEG_POINT };
uint8_t msg_RESET_PARAMS[] = { D_SEG_P, D_SEG_A, D_SEG_R, D_SEG_A | D_SEG_POINT};
uint8_t msg_RESET_STATS[] = { D_SEG_S, D_SEG_T, D_SEG_A, D_SEG_T | D_SEG_POINT};
uint8_t msg_RESET_ALL[] = { D_SEG_T, D_SEG_U, D_SEG_T, D_SEG_T | D_SEG_POINT};
uint8_t msg_SING[] = { D_SEG_S, D_SEG_I, D_SEG_N, D_SEG_G | D_SEG_POINT };
uint8_t msg_DIFF[] = { D_SEG_D, D_SEG_I, D_SEG_F, D_SEG_F | D_SEG_POINT };
uint8_t msg_ENABLED[] = { D_SEG_A, D_SEG_T, D_SEG_T | D_SEG_POINT,D_SEG_OFF };
uint8_t msg_TRAIN[] = { D_SEG_A, D_SEG_D, D_SEG_D | D_SEG_POINT, D_SEG_OFF };

uint8_t msg_ENLARGER[] = { D_SEG_S, D_SEG_T, D_SEG_O, D_SEG_P };
uint8_t msg_DEVEL[] = { D_SEG_S, D_SEG_V, D_SEG_I, D_SEG_L | D_SEG_POINT };

uint8_t msg_STARTSTOP[] = { D_SEG_P, D_SEG_R, D_SEG_i, D_SEG_N | D_SEG_POINT };
uint8_t msg_FOCUSON[] = { D_SEG_F | D_SEG_POINT, D_SEG_OFF, D_SEG_O, D_SEG_N };

uint8_t msg_INFO_TIMERMODE_EnlargerTimer[] = { D_SEG_I, D_SEG_N, D_SEG_G, D_SEG_R, D_SEG_A, D_SEG_N, D_SEG_D | D_SEG_POINT };
uint8_t msg_INFO_TIMERMODE_Devel[] = { D_SEG_S, D_SEG_V, D_SEG_I | D_SEG_POINT, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF };
uint8_t msg_INFO_TIMERMODE_FactorialCalculator[] = {D_SEG_C, D_SEG_A, D_SEG_L, D_SEG_C | D_SEG_POINT, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF  };
uint8_t msg_INFO_TIMERMODE_Metronome[] = { D_SEG_M, D_SEG_E, D_SEG_T, D_SEG_R , D_SEG_O, D_SEG_N, D_SEG_O, D_SEG_M, D_SEG_O };
uint8_t msg_INFO_TIMERMODE_TestStrips[] = { D_SEG_P, D_SEG_R | D_SEG_POINT, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF, D_SEG_OFF };


#endif