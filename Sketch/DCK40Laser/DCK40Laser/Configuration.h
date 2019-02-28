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

#define STEPPERTYPE 5   // RampsFD

////////////////////////////////////////////////////////

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#define MYNUM_AXIS 2
#define LCD_NUMAXIS  MYNUM_AXIS
#define MYUSE_LCD

////////////////////////////////////////////////////////

#define X_MAXSIZE 320000				// in mm1000_t
#define Y_MAXSIZE 220000
#define Z_MAXSIZE 100000
#define A_MAXSIZE 1000
#define B_MAXSIZE 1000
#define C_MAXSIZE 1000

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0
//#define STEPPERDIRECTION (1 << X_AXIS) + (1 << Y_AXIS)		// set bit to invert direction of each axis

#define STEPSPERROTATION	400
#define MICROSTEPPING		16

#define TOOTH 20
#define TOOTHSIZE 2.032
#define SCREWLEAD (TOOTH*TOOTHSIZE)

#define X_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Y_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Z_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define A_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define B_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define C_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

////////////////////////////////////////////////////////

#define CNC_MAXSPEED 55000        // steps/sec
#define CNC_ACC  700
#define CNC_DEC  800
#define CNC_JERKSPEED 1000

////////////////////////////////////////////////////////
// NoReference, ReferenceToMin, ReferenceToMax

#define X_USEREFERENCE	EReverenceType::ReferenceToMin
#define Y_USEREFERENCE	EReverenceType::ReferenceToMax
#define Z_USEREFERENCE	EReverenceType::NoReference
#define A_USEREFERENCE	EReverenceType::NoReference
#define B_USEREFERENCE	EReverenceType::NoReference
#define C_USEREFERENCE	EReverenceType::NoReference


#define REFMOVE_1_AXIS  X_AXIS
#define REFMOVE_2_AXIS  Y_AXIS
#define REFMOVE_3_AXIS  255
#define REFMOVE_4_AXIS  255
#define REFMOVE_5_AXIS  255
#define REFMOVE_6_AXIS  255

#define X_REFERENCEHITVALUE_MIN HIGH
#define Y_REFERENCEHITVALUE_MIN 255
#define Z_REFERENCEHITVALUE_MIN 255
#define A_REFERENCEHITVALUE_MIN 255
#define B_REFERENCEHITVALUE_MIN 255
#define C_REFERENCEHITVALUE_MIN 255

#define X_REFERENCEHITVALUE_MAX 255
#define Y_REFERENCEHITVALUE_MAX HIGH
#define Z_REFERENCEHITVALUE_MAX 255
#define A_REFERENCEHITVALUE_MAX 255
#define B_REFERENCEHITVALUE_MAX 255
#define C_REFERENCEHITVALUE_MAX 255

#define MOVEAWAYFROMREF_MM1000 250

#define SPINDLE_ANALOGSPEED
#define SPINDLE_ISLASER
#define SPINDLE_MAXSPEED	255		// analog 255
#define SPINDLE_FADETIMEDELAY  0    // e.g. 8ms * 255 => 2040ms from 0 to max, 4080 from -max to +max

////////////////////////////////////////////////////////

#if STEPPERTYPE==5
#include "Configuration_RampsFD.h"
#endif

////////////////////////////////////////////////////////

#define GO_DEFAULT_STEPRATE		((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_MAXSTEPRATE	((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_FEEDPRATE	100000	// in mm1000 / min

#define STEPRATE_REFMOVE	5000

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION					F(", DC-K40 Laser")
