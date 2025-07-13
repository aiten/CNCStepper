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

#define CHANGEDIRECTIONDILAYMICROS	0

static void DelayChangeDirection()
{
	if (CHANGEDIRECTIONDILAYMICROS)
	{
		CHAL::DelayMicroseconds(CHANGEDIRECTIONDILAYMICROS);
	}
}

#define A4998DRV8825_CHANGEDIRECTIONMICROS	0

#if defined(__SAM3X8E__) || defined(__SAMD21G18A__) || defined(esp32)

static void Delay1(uint8_t) ALWAYSINLINE		{ CHAL::DelayMicroseconds(1); } 
static void Delay2() ALWAYSINLINE				{ CHAL::DelayMicroseconds(1); } 

#else //AVR

//static void Delay1(uint8_t /* numaxis */) ALWAYSINLINE	{ CHAL::DelayMicroseconds0500(); }
//static void Delay1(uint8_t numaxis) ALWAYSINLINE	{ if (numaxis>3) CHAL::DelayMicroseconds0312(); else CHAL::DelayMicroseconds0500();}
static void Delay1(uint8_t numaxis) ALWAYSINLINE	{ if (numaxis<3) CHAL::DelayMicroseconds0500(); }
static void Delay2() ALWAYSINLINE					{ CHAL::DelayMicroseconds0500(); }

#endif
