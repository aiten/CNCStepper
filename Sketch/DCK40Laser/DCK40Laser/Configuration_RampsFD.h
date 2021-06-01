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

#define CMyStepper CStepperRampsFD
#define CMyParser CGCode3DParser
#define CMyControlBase CControl3D

////////////////////////////////////////////////////////

#define BOARDNAME RAMPSFD
#define SD_ENABLE_PIN     CAT(BOARDNAME,_SDSS_PIN)

////////////////////////////////////////////////////////

#define RAMPSFD_DISABLE_E1

////////////////////////////////////////////////////////

#include <Steppers/StepperRampsFD_pins.h>

// change some pin definition here:

#undef RAMPSFD_REF_ON
#undef RAMPSFD_REF_OFF

#define RAMPSFD_REF_ON  1
#define RAMPSFD_REF_OFF 0

#include <Steppers/StepperRampsFD.h>

////////////////////////////////////////////////////////

#define HOLDRESUME_PIN		CAT(BOARDNAME, _LCD_KILL_PIN)
#define HOLDRESUME_PIN_ON	CAT(BOARDNAME, _LCD_KILL_PIN_ON)

#define KILL_PIN		RAMPSFD_ESTOP_PIN
#define KILL_PIN_ON		LOW

#define LASER_PWM_PIN  RAMPSFD_SERVO1_PIN

#define LASER_ENABLE_PIN  RAMPSFD_SERVO2_PIN
#define LASER_ENABLE_PIN_ON  LOW
#define LASER_ENABLE_PIN_OFF HIGH

#define LASERWATER_PIN	RAMPSFD_SERVO3_PIN
#define LASERWATER_PIN_ON  LOW
#define LASERWATER_PIN_OFF HIGH
#define LASERWATER_ONTIME	120000 // 120000		// switch off if idle for 12000 => 2 min Sec
//#define LASERWATER_ONTIME  1000 // 1200000		// switch off if idle for 1200 => 20 min Sec

#define LASERVACUUM_PIN	RAMPSFD_SERVO4_PIN
#define LASERVACUUM_PIN_ON  LOW
#define LASERVACUUM_PIN_OFF HIGH
#define LASERVACUUM_ONTIME	60000					// switch off if idle for ?? Sec

////////////////////////////////////////////////////////

#define LCD_GROW 64
#define LCD_GCOL 128

#define ROTARY_ENC           CAT(BOARDNAME,_LCD_ROTARY_ENC)
#define ROTARY_ENC_ON    CAT(BOARDNAME,_LCD_ROTARY_ENC_ON)

#define ROTARY_EN1           CAT(BOARDNAME,_LCD_ROTARY_EN1)
#define ROTARY_EN2           CAT(BOARDNAME,_LCD_ROTARY_EN2)
#define SD_ENABLE_PIN    CAT(BOARDNAME,_SDSS_PIN)

////////////////////////////////////////////////////////
