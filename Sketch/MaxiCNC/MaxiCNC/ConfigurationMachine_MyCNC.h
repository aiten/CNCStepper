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

#define CMyStepper CStepperRampsFD

////////////////////////////////////////////////////////

#define USBBAUDRATE 115200

////////////////////////////////////////////////////////

#define MYUSE_LCD

////////////////////////////////////////////////////////

#define MYNUM_AXIS	4

////////////////////////////////////////////////////////

#define X_MAXSIZE 134000				// in mm1000_t
#define Y_MAXSIZE 134000
#define Z_MAXSIZE 83000
#define A_MAXSIZE 360000
#define B_MAXSIZE 360000
#define C_MAXSIZE 360000

////////////////////////////////////////////////////////

//#define STEPPERDIRECTION 0
//#define STEPPERDIRECTION (1<<X_AXIS) + (1<<Y_AXIS)	// set bit to invert direction of each axis
#define STEPPERDIRECTION (1<<Y_AXIS)					// set bit to invert direction of each axis

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

#define X_USEREFERENCE	EReverenceType::ReferenceToMin
#define Y_USEREFERENCE	EReverenceType::ReferenceToMax
#define Z_USEREFERENCE	EReverenceType::ReferenceToMax
//#define Z_USEREFERENCE  EReverenceType::NoReference
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
#define Y_REFERENCEHITVALUE_MIN 255
#define Z_REFERENCEHITVALUE_MIN 255
#define A_REFERENCEHITVALUE_MIN 255
#define B_REFERENCEHITVALUE_MIN 255
#define C_REFERENCEHITVALUE_MIN 255

#define X_REFERENCEHITVALUE_MAX 255
#define Y_REFERENCEHITVALUE_MAX LOW
#define Z_REFERENCEHITVALUE_MAX LOW
#define A_REFERENCEHITVALUE_MAX 255
#define B_REFERENCEHITVALUE_MAX 255
#define C_REFERENCEHITVALUE_MAX 255

#define MOVEAWAYFROMREF_MM1000 125

#define DISABLE_ISANYREFERENCE

#undef SPINDLE_ANALOGSPEED
#define SPINDLE_MAXSPEED	255			// analog 255
#define SPINDLE_FADETIMEDELAY  0	// 8ms * 255 => 2040ms from 0 to max, 4080 from -max to +max

////////////////////////////////////////////////////////

#include "ConfigurationStepper_RampsFD.h"

////////////////////////////////////////////////////////

#define GO_DEFAULT_STEPRATE		  ((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_MAXSTEPRATE	((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))	// steps/sec
#define G1_DEFAULT_FEEDPRATE	  100000	// in mm1000 / min

#define STEPRATE_REFMOVE	  (CNC_MAXSPEED/3)
#define FEEDRATE_REFMOVE_PHASE2		200000

////////////////////////////////////////////////////////

#define CONTROLLERFAN_ONTIME	  10000			// switch off controller fan if idle for 10 Sec
#define CONTROLLERFAN_FAN_PIN	  CAT(BOARDNAME,_FET2D9_PIN)
#define CONTROLLERFAN_ANALOGSPEED
#define CONTROLLERFAN_ANALOGSPEED_INVERT

////////////////////////////////////////////////////////

#define COOLANT_PIN	CAT(BOARDNAME,_AUX2_8)	// Ramps1.4 D42

#define COOLANT_PIN_ON  LOW
#define COOLANT_PIN_OFF HIGH

////////////////////////////////////////////////////////

#define SPINDLE_ENABLE_PIN	CAT(BOARDNAME,_AUX2_6)	// Ramps1.4 D40

#define SPINDLE_DIGITAL_ON  LOW
#define SPINDLE_DIGITAL_OFF HIGH

////////////////////////////////////////////////////////

#define PROBE_PIN	CAT(BOARDNAME,_AUX2_7)	// Ramps 1.4 D44 
#define PROBE2_PIN	CAT(BOARDNAME,_AUX2_5)	// Ramps 1.4 A10 

#define PROBE_PIN_ON  LOW
#define PROBE_PIN_OFF HIGH

#if MYNUM_AXIS < 6
// LCD KILL is shared with E1 (RampsFD) (DIR)
#define HOLDRESUME_PIN		CAT(BOARDNAME, _LCD_KILL_PIN)
#define HOLDRESUME_PIN_ON	CAT(BOARDNAME, _LCD_KILL_PIN_ON)
#endif

////////////////////////////////////////////////////////

#define LCD_GROW 64
#define LCD_GCOL 128

#define ROTARY_ENC           CAT(BOARDNAME,_LCD_ROTARY_ENC)
#define ROTARY_ENC_ON		 CAT(BOARDNAME,_LCD_ROTARY_ENC_ON)

//#define LCD_MENU_MOVE100

////////////////////////////////////////////////////////

#include <MessageCNCLibEx.h>

#if defined(__SAM3X8E__)
#define MESSAGE_MYCONTROL_VERSION			F(", My-CNC, due, Ramps FD")
#define MESSAGE_LCD_HEADLINE				F("My-CNC RampsFD-D")
#else
#define MESSAGE_MYCONTROL_VERSION			F(", My-CNC, mega2560, Ramps FD")
#define MESSAGE_LCD_HEADLINE				F("My-CNC RampsFD-M")
#endif
