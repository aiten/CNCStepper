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

#define HARDWARETYPE_PROXXONMF70	1	// proxxon mf70, ramps 1.4, micro-steps 16
#define HARDWARETYPE_MYCNC	        2	// My-CNC, rampsfd, micro-steps 32
#define HARDWARETYPE_KK1000S        4	// KK1000S, due 
#define HARDWARETYPE_CUSTOM			99	// custom

#define HARDWARETYPE HARDWARETYPE_PROXXONMF70
//#define HARDWARETYPE HARDWARETYPE_MYCNC
//#define HARDWARETYPE HARDWARETYPE_KK1000S

////////////////////////////////////////////////////////

#if HARDWARETYPE==HARDWARETYPE_PROXXONMF70

#include "ConfigurationMachine_ProxxonMF70.h"

#elif HARDWARETYPE==HARDWARETYPE_MYCNC

#include "ConfigurationMachine_MyCNC.h"

#elif HARDWARETYPE==HARDWARETYPE_KK1000S

#include "ConfigurationMachine_KK1000S.h"

#else

#endif

////////////////////////////////////////////////////////

#define STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

////////////////////////////////////////////////////////
