/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) Herbert Aitenbichler

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include <Steppers/StepperRamps14_pins.h>
#include <Steppers/StepperRamps14.h>
#define BOARDNAME RAMPS14

////////////////////////////////////////////////////////

#define MYNUM_AXIS FROMPENTYPE(3,2)
#define LCD_NUMAXIS  MYNUM_AXIS

////////////////////////////////////////////////////////

#if defined(__SAM3X8E__)

#define ROTARY_EN1           CAT(BOARDNAME,_LCD_ROTARY_EN2)
#define ROTARY_EN2           CAT(BOARDNAME,_LCD_ROTARY_EN1)
#define SD_ENABLE_PIN     52

#else

#define ROTARY_EN1       CAT(BOARDNAME,_LCD_ROTARY_EN1)
#define ROTARY_EN2       CAT(BOARDNAME,_LCD_ROTARY_EN2)
#define SD_ENABLE_PIN    CAT(BOARDNAME,_SDSS_PIN)

#endif

////////////////////////////////////////////////////////

#define LCD_GROW 64
#define LCD_GCOL 128

#define ROTARY_ENC        CAT(BOARDNAME,_LCD_ROTARY_ENC)
#define ROTARY_ENC_ON     CAT(BOARDNAME,_LCD_ROTARY_ENC_ON)

////////////////////////////////////////////////////////

#define CMyStepper CStepperRamps14

//#define EMERGENCY_ENDSTOP 5

#define FEEDRATE_REFMOVE  CStepper::GetInstance()->GetDefaultVmax() / 4

////////////////////////////////////////////////////////

#define CONTROLLERFAN_ANALOGSPEED
#define CONTROLLERFAN_ONTIME	10000			// switch off controllerfan if idle for 10 Sec
#define CONTROLLERFAN_FAN_PIN	RAMPS14_FET2D9_PIN

#ifdef RAMPS14_KILL_PIN
#define KILL_PIN		RAMPS14_KILL_PIN
#define KILL_PIN_ON		LOW
#endif

#ifdef RAMPS14_HOLD_PIN
#define HOLD_PIN CNCSHIELD_HOLD_PIN
#define HOLD_PIN_ON CNCSHIELD_HOLD_PIN_ON
#endif

#ifdef RAMPS14_RESUME_PIN
#define RESUME_PIN CNCSHIELD_RESUME_PIN
#define RESUME_PIN_ON CNCSHIELD_RESUME_PIN_ON
#endif

////////////////////////////////////////////////////////
