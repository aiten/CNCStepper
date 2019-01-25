/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

#define A4998DRV8825_CHANGEDIRECTIONMICROS	0


#if defined(USE_A4998)

static void Delay1(uint8_t) ALWAYSINLINE		{ }
static void Delay2() ALWAYSINLINE				{ } 

#elif defined(__SAM3X8E__) || defined(__SAMD21G18A__)

static void Delay1(uint8_t) ALWAYSINLINE		{ CHAL::DelayMicroseconds(1); } 
static void Delay2() ALWAYSINLINE				{ CHAL::DelayMicroseconds(1); } 

#else //AVR

//static void Delay1(uint8_t /* numaxis */) ALWAYSINLINE	{ CHAL::DelayMicroseconds0500(); }
//static void Delay1(uint8_t numaxis) ALWAYSINLINE	{ if (numaxis>3) CHAL::DelayMicroseconds0312(); else CHAL::DelayMicroseconds0500();}
static void Delay1(uint8_t numaxis) ALWAYSINLINE	{ if (numaxis<3) CHAL::DelayMicroseconds0500(); }
static void Delay2() ALWAYSINLINE					{ CHAL::DelayMicroseconds0500(); }

#endif
