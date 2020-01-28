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

#include <StepperLib.h>
#include "MessageCNCLib.h"

#ifdef REDUCED_SIZE

#undef _USE_LCD

#else

#define _USE_LCD

#endif

////////////////////////////////////////////////////////

typedef float   expr_t;       // type for expression parser
typedef int32_t mm1000_t;     // 1/1000 mm
typedef int32_t feedrate_t;   // mm_1000 / min
typedef int32_t inch100000_t; // 1/100000 inch

#define NUM_AXISXYZ		3	// 3dimensions

#define SCALE_FEEDRATE	3
#define SCALE_MM		3
#define SCALE_INCH		5

////////////////////////////////////////////////////////
// GCode Parser

#define NUM_MAXPARAMNAMELENGTH 16
#define NUM_PARAMETERRANGE	255

#if defined(__SAM3X8E__) || defined(__SAMD21G18A__) || defined(_MSC_VER)

#define NUM_PARAMETER	16		// slotCount, map from uint8_t to < NUM_PARAMETER
#define G54ARRAYSIZE	6

#else

#define NUM_PARAMETER	8
#define G54ARRAYSIZE	2

#endif

////////////////////////////////////////////////////////
//
// Control

#define SERIALBUFFERSIZE	128			// even size 

#define TIMEOUTCALLIDLE		333			// time in ms after move completed to call Idle
#define TIMEOUTCALLPOLL		500			// time in ms to call Poll() next if not idle => ASSERT( TIMEOUTCALLPOLL > TIMEOUTCALLIDLE)

#define IDLETIMER0VALUE     TIMER0VALUE(500)	// AVR don't care ... Timer 0 shared with millis, other ?Hz

#define BLINK_LED			LED_BUILTIN
#define TIMEOUTBLINK		1000		// blink of led 13

////////////////////////////////////////////////////////
