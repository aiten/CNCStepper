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

uint16_t CHAL::analogRead(pin_t pin)
{
	return uint16_t(::analogRead(pin));
}

////////////////////////////////////////////////////////

static void IgnoreIrq() {}

CHAL::HALEvent CHAL::_TimerEvent0 = IgnoreIrq;
CHAL::HALEvent CHAL::_TimerEvent1 = IgnoreIrq;

#if !defined(__AVR_ATmega328P__)

CHAL::HALEvent CHAL::_TimerEvent2 = IgnoreIrq;

#endif

#ifdef _MSC_VER

std::function<uint8_t(int16_t)> digitalReadEvent = nullptr;

#endif
