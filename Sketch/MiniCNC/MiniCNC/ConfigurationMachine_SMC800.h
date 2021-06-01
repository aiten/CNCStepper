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

#pragma once

////////////////////////////////////////////////////////

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#include "ConfigurationMachine_Default.h"

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0
//#define STEPPERDIRECTION (1 << X_AXIS) + (1 << Y_AXIS)    // set bit to invert direction of each axis

#define STEPSPERROTATION	200
#define MICROSTEPPING		2
#define SCREWLEAD			1.0

////////////////////////////////////////////////////////

#define CNC_MAXSPEED	3000
#define CNC_ACC			130			// 0.2sec to acc
#define CNC_DEC			150			// 0.15sec to break
#define CNC_JERKSPEED	120

#define X_MAXSPEED 0
#define Y_MAXSPEED 0
#define Z_MAXSPEED 0
#define A_MAXSPEED 0
#define B_MAXSPEED 0
#define C_MAXSPEED 0

#define X_ACC 0
#define Y_ACC 0
#define Z_ACC 0
#define A_ACC 0
#define B_ACC 0
#define C_ACC 0

#define X_DEC 0
#define Y_DEC 0
#define Z_DEC 0
#define A_DEC 0
#define B_DEC 0
#define C_DEC 0

#define X_STEPRATE_REFMOVE 0
#define Y_STEPRATE_REFMOVE 0
#define Z_STEPRATE_REFMOVE 0
#define A_STEPRATE_REFMOVE 0
#define B_STEPRATE_REFMOVE 0
#define C_STEPRATE_REFMOVE 0

#define X_STEPSPERMM 0.0
#define Y_STEPSPERMM 0.0
#define Z_STEPSPERMM 0.0
#define A_STEPSPERMM 0.0
#define B_STEPSPERMM 0.0
#define C_STEPSPERMM 0.0

#define X_PROBESIZE 25000
#define Y_PROBESIZE 25000
#define Z_PROBESIZE 25000
#define A_PROBESIZE 25000
#define B_PROBESIZE 25000
#define C_PROBESIZE 25000

////////////////////////////////////////////////////////

#include "ConfigurationStepper_SMC800.h"

////////////////////////////////////////////////////////

#define CONTROLLERFAN_ONTIME	10000			// switch off controllerfan if idle for 10 Sec
//#define CONTROLLERFAN_FAN_PIN	-1 //14 // 10
#undef CONTROLLERFAN_FAN_PIN

#define CONTROLLERFAN_DIGITAL_ON  HIGH
#define CONTROLLERFAN_DIGITAL_OFF LOW

////////////////////////////////////////////////////////

#undef SPINDLE_ANALOGSPEED
#define SPINDLE_ENABLE_PIN	15

#define SPINDLE_DIGITAL_ON  LOW
#define SPINDLE_DIGITAL_OFF HIGH

////////////////////////////////////////////////////////

#define PROBE_PIN	16

#define PROBE_PIN_ON  LOW
#define PROBE_PIN_OFF HIGH

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION          F(", SMC800")
