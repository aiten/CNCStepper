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
//
// * Sample for CNCShield v3.51 (Uno)
// * CNCShield v3.51
// * use 3 Axis
// * DRV8825 with ms32
// 
////////////////////////////////////////////////////////

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#define MYNUM_AXIS 3

////////////////////////////////////////////////////////

#include "Machine_Default.h"

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0
//#define STEPPERDIRECTION (1 << X_AXIS) + (1 << Y_AXIS)    // set bit to invert direction of each axis

#define STEPSPERROTATION  200
#define MICROSTEPPING   16
#define SCREWLEAD     4.0

#define STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

////////////////////////////////////////////////////////

#define CNC_MAXSPEED 20000        // steps/sec = 6.25 rot/sec
#define CNC_ACC  350              // 0.184 sec to full speed
#define CNC_DEC  400              // 0.141 sec to break
#define CNC_JERKSPEED 1000

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

#define SPINDLE_FADE

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#define CMyStepper CStepperCNCShield

#define CNCSHIELD_NUM_AXIS MYNUM_AXIS
//#define CNCSHIELD_GBRL09

#include <Steppers/StepperCNCShield_pins.h>

// change some pin definition here:

//#undef CNCLIB_USE_A4998
//#define CNCLIB_USE_DRV8825
//#undef CNCLIB_USE_TMC220X

#include <Steppers/StepperCNCShield.h>
#include "Stepper_CNCShield3x.h"

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#define GO_DEFAULT_STEPRATE		  ((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_MAXSTEPRATE	((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_FEEDPRATE	  100000	// in mm1000 / min
#define STEPRATE_REFMOVE		(CNC_MAXSPEED/3)

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION          F(", CNCShield")
