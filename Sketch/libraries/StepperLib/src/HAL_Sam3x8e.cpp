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

#include <stdlib.h>
#include <string.h>

#include <Arduino.h>
#include <ctype.h>

#include "HAL.h"

////////////////////////////////////////////////////////

#if defined(__SAM3X8E__)

static void IgnoreIrq() {}

void TC6_Handler()
{
	TC_GetStatus(DUETIMER0_TC, DUETIMER0_CHANNEL);
	CHAL::_TimerEvent0();
}

//__attribute__((__interrupt__))
//__attribute__((nesting))
void TC8_Handler()
{
	TC_GetStatus(DUETIMER1_TC, DUETIMER1_CHANNEL);
	CHAL::_TimerEvent1();
}

void TC7_Handler()
{
	TC_GetStatus(DUETIMER2_TC, DUETIMER2_CHANNEL);
	CHAL::_TimerEvent2();
}

void CAN0_Handler()
{
	CHAL::_BackgroundEvent();
}

CHAL::HALEvent CHAL::_BackgroundEvent = IgnoreIrq;

////////////////////////////////////////////////////////

#endif
