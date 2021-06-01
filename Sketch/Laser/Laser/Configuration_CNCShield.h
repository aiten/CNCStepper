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

#define CMyStepper CStepperCNCShield

////////////////////////////////////////////////////////

#define CNCSHIELD_NUM_AXIS MYNUM_AXIS
//#define CNCSHIELD_GBRL09

#include <Steppers/StepperCNCShield_pins.h>

// change some pin definition here:


#include <Steppers/StepperCNCShield.h>

////////////////////////////////////////////////////////

#ifdef CNCSHIELD_ABORT_PIN
#define KILL_PIN		CNCSHIELD_ABORT_PIN
#define KILL_PIN_ON		CNCSHIELD_ABORT_PIN_ON
#endif

#ifdef CNCSHIELD_HOLD_PIN
#define HOLD_PIN CNCSHIELD_HOLD_PIN
#define HOLD_PIN_ON		CNCSHIELD_HOLD_PIN_ON
#endif

#ifdef CNCSHIELD_RESUME_PIN
#define RESUME_PIN CNCSHIELD_RESUME_PIN
#define RESUME_PIN_ON		CNCSHIELD_RESUME_PIN_ON
#endif

#ifdef CNCSHIELD_PROBE_PIN
#define PROBE_PIN_PIN		CNCSHIELD_PROBE_PIN
#define PROBE_PIN_ON		CNCSHIELD_PROBE_PIN_ON
#endif

#ifdef CNCSHIELD_COOLANT_PIN
#define COOLANT_PIN			CNCSHIELD_COOLANT_PIN
#define COOLANT_PIN_ON		CNCSHIELD_COOLANT_PIN_ON
#define COOLANT_PIN_OFF		CNCSHIELD_COOLANT_PIN_OFF
#endif

#ifdef CNCSHIELD_SPINDLE_ENABLE_PIN
#define SPINDLE_ENABLE_PIN	CNCSHIELD_SPINDLE_ENABLE_PIN
#define SPINDLE_DIGITAL_ON	CNCSHIELD_SPINDLE_DIGITAL_ON
#define SPINDLE_DIGITAL_OFF	CNCSHIELD_SPINDLE_DIGITAL_OFF
#define SPINDLE_DIR_PIN		CNCSHIELD_SPINDLE_DIR_PIN
#define SPINDLE_DIR_CLW		CNCSHIELD_SPINDLE_DIR_CLW
#define SPINDLE_DIR_CCLW	CNCSHIELD_SPINDLE_DIR_CCLW
#endif

#undef USECONTROLERFAN
#ifdef USECONTROLERFAN
#define CONTROLLERFAN_ONTIME	10000			// switch off controllerfan if idle for 10 Sec
#define CONTROLLERFAN_FAN_PIN	14 // 10

#define CONTROLLERFAN_DIGITAL_ON  HIGH
#define CONTROLLERFAN_DIGITAL_OFF LOW
#undef  CONTROLLERFAN_ANALOGSPEED
#endif

////////////////////////////////////////////////////////

#define DISABLELEDBLINK

////////////////////////////////////////////////////////
