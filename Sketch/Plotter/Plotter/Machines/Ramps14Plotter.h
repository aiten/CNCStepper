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

//#define LCD_TYPE  1		// LCD_2004_LiquidCrystal_I2C
#define LCD_TYPE  2			// LCD_12864_u8g

#define PENTYPE_ZAXIS 0
#define PENTYPE_SERVO 1

#define PENTYPE PENTYPE_ZAXIS

////////////////////////////////////////////////////////

#define USBBAUDRATE 115200

////////////////////////////////////////////////////////

#define BLINK_LED LED_BUILTIN
#define BLINK_TIMEOUT		1000		

////////////////////////////////////////////////////////

#define EPROM_SIGNATURE_PLOTTER 0x21438703

////////////////////////////////////////////////////////

#define MYUSE_LCD

////////////////////////////////////////////////////////

#define FROMPENTYPE(a,b) (PENTYPE==PENTYPE_ZAXIS ? (a) : (b))

////////////////////////////////////////////////////////

#define X_MAXSIZE 510000		//515mm
#define Y_MAXSIZE 295000		//295mm
#define Z_MAXSIZE 8000			//8
#define A_MAXSIZE 360000
#define B_MAXSIZE 360000
#define C_MAXSIZE 360000

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0		// set bit to invert direction of each axis

#define STEPSPERROTATION	200
#define MICROSTEPPING		16

// 3200steps/rot, T2.5 belt pulley with 12teeth

#define TOOTH 12
#define TOOTHSIZE 2.5
#define SCREWLEAD (TOOTH*TOOTHSIZE)

#define STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

////////////////////////////////////////////////////////

#define CNC_MAXSPEED	25000        // steps/sec
#define CNC_ACC			400
#define CNC_DEC			450
#define CNC_JERKSPEED	1000

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
#define Z_USEREFERENCE	FROMPENTYPE(EReverenceType::ReferenceToMin,EReverenceType::NoReference)
#define A_USEREFERENCE	EReverenceType::NoReference
#define B_USEREFERENCE	EReverenceType::NoReference
#define C_USEREFERENCE	EReverenceType::NoReference

#define REFMOVE_1_AXIS  Z_AXIS
#define REFMOVE_2_AXIS  Y_AXIS
#define REFMOVE_3_AXIS  X_AXIS
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
#define SPINDLE_MAXSPEED		10000		// analog 255
#define SPINDLE_FADETIMEDELAY  0    		// 8ms * 255 => 2040ms from 0 to max, 4080 from -max to +max

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#include <Steppers/StepperRamps14_pins.h>
#include <Steppers/StepperRamps14.h>

#define BOARDNAME RAMPS14

////////////////////////////////////////////////////////

#define MYNUM_AXIS FROMPENTYPE(3,2)
#define LCD_NUMAXIS  MYNUM_AXIS

////////////////////////////////////////////////////////

#if defined(__SAM3X8E__)

#define ROTARY_EN1           CAT(BOARDNAME,_LCD_ROTARY_EN2)
#define ROTARY_EN2           CAT(BOARDNAME,_LCD_ROTARY_EN1)
#define SD_ENABLE_PIN     52

#else

#define ROTARY_EN1       CAT(BOARDNAME,_LCD_ROTARY_EN1)
#define ROTARY_EN2       CAT(BOARDNAME,_LCD_ROTARY_EN2)
#define SD_ENABLE_PIN    CAT(BOARDNAME,_SDSS_PIN)

#endif

////////////////////////////////////////////////////////

#define LCD_GROW 64
#define LCD_GCOL 128

#define ROTARY_ENC        CAT(BOARDNAME,_LCD_ROTARY_ENC)
#define ROTARY_ENC_ON     CAT(BOARDNAME,_LCD_ROTARY_ENC_ON)

////////////////////////////////////////////////////////

#define CMyStepper CStepperRamps14

//#define EMERGENCY_ENDSTOP 5

#define FEEDRATE_REFMOVE  CStepper::GetInstance()->GetDefaultVmax() / 4

////////////////////////////////////////////////////////

#define CONTROLLERFAN_ANALOGSPEED
#define CONTROLLERFAN_FAN_PIN	RAMPS14_FET2D9_PIN

#ifdef RAMPS14_KILL_PIN
#define KILL_PIN		RAMPS14_KILL_PIN
#define KILL_PIN_ON		LOW
#endif

#ifdef RAMPS14_HOLD_PIN
#define HOLD_PIN CNCSHIELD_HOLD_PIN
#define HOLD_PIN_ON CNCSHIELD_HOLD_PIN_ON
#endif

#ifdef RAMPS14_RESUME_PIN
#define RESUME_PIN CNCSHIELD_RESUME_PIN
#define RESUME_PIN_ON CNCSHIELD_RESUME_PIN_ON
#endif

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#define STEPRATE_REFMOVE	(CNC_MAXSPEED/4)

////////////////////////////////////////////////////////
// PlotterControl

#define SERVO1_PIN RAMPS14_SERVO3_PIN
#define SERVO1_CLAMPOPEN		1000
#define SERVO1_CLAMPCLOSE		2000
#define SERVO1_CLAMPOPENDELAY	1000
#define SERVO1_CLAMPCLOSEDELAY	1000

#define PLOTTER_DEFAULT_PENUP_FEEDRATE			LONG_MAX	 // reduced to maxStepRate
#define PLOTTER_DEFAULT_PENDOWN_FEEDRATE		3600000l     // 60 mm/ sec;

#if PENTYPE == PENTYPE_ZAXIS			// Z-AXIS

#define PLOTTER_DEFAULT_Z_PENUP_FEEDRATE		2400000l // 40 mm / sec
#define PLOTTER_DEFAULT_Z_PENDOWN_FEEDRATE		1800000l // 30 mm / sec
#define PLOTTER_DEFAULT_Z_PENCHANGE_FEEDRATE	2400000l // 40 mm / sec

#define PLOTTER_PENDOWNPOS_Z		LONG_MAX
#define PLOTTER_PENUPPOS_Z			(Z_MAXSIZE/2)
#define PLOTTER_PENCHANGEPOS_Z		0

#elif PENTYPE == PENTYPE_SERVO		// servo

#define SERVO2_PIN RAMPS14_SERVO2_PIN

// feedrate are used as delays (in ms)
#define PLOTTER_DEFAULT_Z_PENUP_FEEDRATE		200
#define PLOTTER_DEFAULT_Z_PENDOWN_FEEDRATE		200
#define PLOTTER_DEFAULT_Z_PENCHANGE_FEEDRATE	200

// servo positions (micro sec)
#define PLOTTER_PENDOWNPOS_Z		2000
#define PLOTTER_PENUPPOS_Z			1500
#define PLOTTER_PENCHANGEPOS_Z		1000

#else

error;

#endif

#define PLOTTER_PENCOUNT		8

#define PLOTTER_PENCHANGEPOS_X		LONG_MAX
#define PLOTTER_PENCHANGEPOS_X_OFS	0
#define PLOTTER_PENCHANGEPOS_Y		0
#define PLOTTER_PENCHANGEPOS_Y_OFS	30000l	//mm1000

#define PLOTTER_PENUP_TIMEOUT		1000

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION F(", Plotter")
