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

#include "Beep.h"

////////////////////////////////////////////////////////

#ifndef _MSC_VER
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

const SPlayTone SPlayTone::PlayOK[] PROGMEM =
{
	{ ToneA6, MilliSecToDuration(100) },
	{ ToneEnd, 0 }
};

const SPlayTone SPlayTone::PlayError[] PROGMEM
{
	{ ToneA6, MilliSecToDuration(200) },
	{ ToneA7, MilliSecToDuration(200) },
	{ ToneA6, MilliSecToDuration(200) },
	{ ToneA7, MilliSecToDuration(200) },
	{ ToneEnd, 0 }
};

// Fuer Elise: Beethoven

#define Duration0	4000
#define Duration1	MilliSecToDuration(Duration0/1)
#define Duration2	MilliSecToDuration(Duration0/2)
#define Duration4	MilliSecToDuration(Duration0/4)
#define Duration8	MilliSecToDuration(Duration0/8)
#define Duration16	MilliSecToDuration(Duration0/16)

const SPlayTone SPlayTone::PlayInfo[] PROGMEM
{
	{ ToneE5, Duration16 },
	{ ToneDx5, Duration16 },

	{ ToneE5, Duration16 },
	{ ToneDx5, Duration16 },
	{ ToneE5, Duration16 },
	{ ToneB4, Duration16 },
	{ ToneD5, Duration16 },
	{ ToneC5, Duration16 },

	{ ToneA4, Duration8 },
	{ ToneNo, Duration16 },
	{ ToneC4, Duration16 },
	{ ToneE4, Duration16 },
	{ ToneA4, Duration16 },

	{ ToneB4, Duration8 },
	{ ToneNo, Duration16 },
	{ ToneE4, Duration16 },
	{ ToneGx4, Duration16 },
	{ ToneB4, Duration16 },

	{ ToneC5, Duration8 },
	{ ToneNo, Duration16 },
	{ ToneE4, Duration16 },
	{ ToneE5, Duration16 },
	{ ToneDx5, Duration16 },

	{ ToneE5, Duration16 },
	{ ToneDx5, Duration16 },
	{ ToneE5, Duration16 },
	{ ToneB4, Duration16 },
	{ ToneD5, Duration16 },
	{ ToneC5, Duration16 },

	{ ToneA4, Duration8 },
	{ ToneNo, Duration16 },
	{ ToneC4, Duration16 },
	{ ToneE4, Duration16 },
	{ ToneA4, Duration16 },

	{ ToneB4, Duration8 },
	{ ToneNo, Duration16 },
	{ ToneE4, Duration16 },
	{ ToneC5, Duration16 },
	{ ToneB4, Duration16 },

	{ ToneA4, Duration4 },

	{ ToneEnd, 0 }
};
