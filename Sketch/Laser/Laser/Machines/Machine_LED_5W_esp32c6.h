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
// esp32 c6
// try to be compatible with CNCShield v3.51
// use 2(-4) Axis
// 8825 with ms32
// 
////////////////////////////////////////////////////////

#define BLINK_LED 15
#define BLINK_TIMEOUT		1000		

////////////////////////////////////////////////////////

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#define MYNUM_AXIS 2

////////////////////////////////////////////////////////

#define X_MAXSIZE 400000				// in mm1000_t
#define Y_MAXSIZE 380000
#define Z_MAXSIZE 100000
#define A_MAXSIZE 50000
#define B_MAXSIZE 360000
#define C_MAXSIZE 360000

////////////////////////////////////////////////////////

#define STEPPERDIRECTION ((1 << X_AXIS) + (1 << Y_AXIS))		// set bit to invert direction of each axis

#define STEPSPERROTATION	200
#define MICROSTEPPING		32

// GT2 with 15Tooth = > 30mm

#define TOOTH 15
#define TOOTHSIZE 2.0
#define SCREWLEAD (TOOTH*TOOTHSIZE)

#define STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

////////////////////////////////////////////////////////

#define CNC_MAXSPEED 51200        // steps/sec (8rpm, 0.2acc, 0.15dec, 32/200, 15/2), see cnclib-Calc-Eeprom
#define CNC_ACC  537
#define CNC_DEC  620
#define CNC_JERKSPEED 2048

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

#define X_USEREFERENCE	EReverenceType::ReferenceToMin
#define Y_USEREFERENCE	EReverenceType::ReferenceToMin
#define Z_USEREFERENCE  EReverenceType::NoReference
#define A_USEREFERENCE  EReverenceType::NoReference
#define B_USEREFERENCE  EReverenceType::NoReference
#define C_USEREFERENCE  EReverenceType::NoReference

#define REFMOVE_1_AXIS  Y_AXIS
#define REFMOVE_2_AXIS  X_AXIS
#define REFMOVE_3_AXIS  255
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

#define SPINDLE_ANALOGSPEED
#define SPINDLE_ISLASER
#define SPINDLE_MAXSPEED	255			// analog 255
#define SPINDLE_FADETIMEDELAY  0		// e.g. 8ms * 255 => 2040ms from 0 to max, 4080 from -max to +max

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#define CMyStepper CStepperCNCShield

#define CNCSHIELD_NUM_AXIS MYNUM_AXIS

#include <Steppers/StepperCNCShield_esp32_pins.h>

// change some pin definition here:

#undef CNCLIB_USE_A4998
#define CNCLIB_USE_DRV8825
#undef CNCLIB_USE_TMC220X

#include <Steppers/StepperCNCShield.h>
#include "Stepper_CNCShield3x.h"

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


#define GO_DEFAULT_STEPRATE		((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_MAXSTEPRATE	((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_FEEDPRATE	100000	// in mm1000 / min
#define STEPRATE_REFMOVE	    (CNC_MAXSPEED/6)

// do not use probe
#undef PROBE_PIN

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION					F(", Laser(esp32)")