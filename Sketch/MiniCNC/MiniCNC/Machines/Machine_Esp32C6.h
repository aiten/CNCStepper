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
// use 3-4 Axis
// DRV8825 with ms32
// 
////////////////////////////////////////////////////////

#define BLINK_LED 15
#define BLINK_TIMEOUT		1000		

////////////////////////////////////////////////////////

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#define MYNUM_AXIS 3

////////////////////////////////////////////////////////

#define X_MAXSIZE 134000				// in mm1000_t
#define Y_MAXSIZE 134000
#define Z_MAXSIZE 83000
#define A_MAXSIZE 360000
#define B_MAXSIZE 360000
#define C_MAXSIZE 360000

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0		// set bit to invert direction of each axis

#define STEPSPERROTATION  200
#define MICROSTEPPING     32
#define SCREWLEAD         5.0

////////////////////////////////////////////////////////

#define CNC_MAXSPEED ((steprate_t)50000)			// steps/sec => 50000 => 7.8 rot /sec
#define CNC_ACC  496								// 0.257 => time to full speed
#define CNC_DEC  565								// 0.1975 => time to break
#define CNC_JERKSPEED 2240

#define X_MAXSPEED 0
#define Y_MAXSPEED 0
#define Z_MAXSPEED (CNC_MAXSPEED-5000)
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

#define REFMOVE_1_AXIS  255 // Z_AXIS
#define REFMOVE_2_AXIS  255 // X_AXIS
#define REFMOVE_3_AXIS  255 // Y_AXIS
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
// esp32 do not have a CNCShield => we have to define all pins!

#define CNCLIB_USE_DRV8825

#define CNCSHIELD_NUM_AXIS MYNUM_AXIS

#define CNCSHIELD_PIN_STEP_OFF		0
#define CNCSHIELD_PIN_STEP_ON		1

#define CNCSHIELD_PIN_DIR_OFF		0
#define CNCSHIELD_PIN_DIR_ON		1

// Enable: LOW Active
#define CNCSHIELD_PIN_ENABLE_OFF	1
#define CNCSHIELD_PIN_ENABLE_ON		0

////////////////////////////////////////////////////////

#define CNCSHIELD_ENABLE_PIN		20

#define CNCSHIELD_X_STEP_PIN		5
#define CNCSHIELD_X_DIR_PIN			14
#define CNCSHIELD_X_MIN_PIN			0
#define CNCSHIELD_X_MAX_PIN			0

#define CNCSHIELD_Y_STEP_PIN		6
#define CNCSHIELD_Y_DIR_PIN			18
#define CNCSHIELD_Y_MIN_PIN			1
#define CNCSHIELD_Y_MAX_PIN			1

#define CNCSHIELD_Z_STEP_PIN		7
#define CNCSHIELD_Z_DIR_PIN			19
#define CNCSHIELD_Z_MIN_PIN			2
#define CNCSHIELD_Z_MAX_PIN			2

#define CNCSHIELD_A_STEP_PIN		x	
#define CNCSHIELD_A_DIR_PIN			xx	

#define CNCSHIELD_SPINDLE_ENABLE_PIN	3
#define CNCSHIELD_SPINDLE_DIR_PIN		4

#define CNCSHIELD_SPINDLE_DIGITAL_ON		LOW
#define CNCSHIELD_SPINDLE_DIGITAL_OFF		HIGH
#define CNCSHIELD_SPINDLE_ANALOG_MAX		255

#define CNCSHIELD_SPINDLE_DIR_CLW	LOW
#define CNCSHIELD_SPINDLE_DIR_CCLW	HIGH

//#define CNCSHIELD_ABORT_PIN		PIN_A0
//#define CNCSHIELD_HOLD_PIN		PIN_A1
//#define CNCSHIELD_RESUME_PIN		PIN_A2
//#define CNCSHIELD_COOLANT_PIN		PIN_A3

#define CNCSHIELD_ABORT_PIN_ON		LOW
#define CNCSHIELD_ABORT_PIN_OFF		HIGH

#define CNCSHIELD_HOLD_PIN_ON		LOW
#define CNCSHIELD_HOLD_PIN_OFF		HIGH

#define CNCSHIELD_RESUME_PIN_ON		LOW
#define CNCSHIELD_RESUME_PIN_OFF	HIGH

#define CNCSHIELD_COOLANT_PIN_ON	LOW
#define CNCSHIELD_COOLANT_PIN_OFF	HIGH

#define CNCSHIELD_PROBE_PIN			9
#define CNCSHIELD_PROBE_PIN_ON		LOW
#define CNCSHIELD_PROBE_PIN_OFF		HIGH

#include "Stepper_CNCShield3x.h"

////////////////////////////////////////////////////////

#define STEPRATE_REFMOVE		(CNC_MAXSPEED/3)

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION          F(", CNC-Esp32C6")
