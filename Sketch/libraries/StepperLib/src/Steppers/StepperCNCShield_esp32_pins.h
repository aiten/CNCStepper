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

#ifndef CNCSHIELD_NUM_AXIS
#define CNCSHIELD_NUM_AXIS 3
#endif

////////////////////////////////////////////////////////

#undef  CNCLIB_USE_A4998
#define CNCLIB_USE_DRV8825
#undef  CNCLIB_USE_TMC220X

////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// esp32 do not have a CNCShield => we have to define all pins!

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
