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
//
// Arduino Zero => do not use eeprom => configure all values
// CNCShield v3.51
// use 3 Axis
// DRV8825 with ms32
// 
////////////////////////////////////////////////////////

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#define MYNUM_AXIS 3

////////////////////////////////////////////////////////

#define X_MAXSIZE 150000        // in mm1000_t
#define Y_MAXSIZE 105000
#define Z_MAXSIZE 30000
#define A_MAXSIZE 360000
#define B_MAXSIZE 360000
#define C_MAXSIZE 360000

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0		// set bit to invert direction of each axis

#define STEPSPERROTATION	200
#define MICROSTEPPING		32
#define SCREWLEAD			4.0

////////////////////////////////////////////////////////

//#define CNC_MAXSPEED 20000        // steps/sec = 6.25 rot/sec
//#define CNC_ACC  350              // 0.184 sec to full speed
//#define CNC_DEC  400              // 0.141 sec to break
//#define CNC_JERKSPEED 1000

#define CNC_MAXSPEED 40000        // steps/sec
#define CNC_ACC  565
#define CNC_DEC  495
#define CNC_JERKSPEED 1600

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
// NoReference, ReferenceToMin, ReferenceToMax

#define X_USEREFERENCE	EReverenceType::ReferenceToMax
#define Y_USEREFERENCE	EReverenceType::ReferenceToMax
#define Z_USEREFERENCE	EReverenceType::ReferenceToMax
#define A_USEREFERENCE	EReverenceType::NoReference
#define B_USEREFERENCE	EReverenceType::NoReference
#define C_USEREFERENCE	EReverenceType::NoReference

#define REFMOVE_1_AXIS  Z_AXIS
#define REFMOVE_2_AXIS  X_AXIS
#define REFMOVE_3_AXIS  Y_AXIS
#define REFMOVE_4_AXIS  255
#define REFMOVE_5_AXIS  255
#define REFMOVE_6_AXIS  255

#define X_REFERENCEHITVALUE_MIN LOW
#define Y_REFERENCEHITVALUE_MIN LOW
#define Z_REFERENCEHITVALUE_MIN LOW
#define A_REFERENCEHITVALUE_MIN LOW
#define B_REFERENCEHITVALUE_MIN LOW
#define C_REFERENCEHITVALUE_MIN LOW

#define X_REFERENCEHITVALUE_MAX LOW
#define Y_REFERENCEHITVALUE_MAX LOW
#define Z_REFERENCEHITVALUE_MAX LOW
#define A_REFERENCEHITVALUE_MAX LOW
#define B_REFERENCEHITVALUE_MAX LOW
#define C_REFERENCEHITVALUE_MAX LOW

#define MOVEAWAYFROMREF_MM1000 500

////////////////////////////////////////////////////////

#define SPINDLE_ANALOGSPEED
#define SPINDLE_MAXSPEED	10000			// analog 255
#define SPINDLE_FADETIMEDELAY  8    // 8ms * 255 => 2040ms from 0 to max, 4080 from -max to +max
#define SPINDLE_FADE

////////////////////////////////////////////////////////

#include "ConfigurationStepper_CNCShield.h"

////////////////////////////////////////////////////////

#define STEPRATE_REFMOVE		(CNC_MAXSPEED/3)

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION          F(", CNC-3325")
