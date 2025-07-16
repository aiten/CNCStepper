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

#define RAMPSFD_DISABLE_E1
#define LCD_NUMAXIS			MYNUM_AXIS

////////////////////////////////////////////////////////

#include <Steppers/StepperRampsFD_pins.h>

//swap X with E PINs because damaged pin on X socket => use e0
#undef RAMPSFD_X_STEP_PIN
#undef RAMPSFD_X_DIR_PIN
#undef RAMPSFD_X_ENABLE_PIN
#undef RAMPSFD_E0_STEP_PIN
#undef RAMPSFD_E0_DIR_PIN
#undef RAMPSFD_E0_ENABLE_PIN

#define RAMPSFD_E0_STEP_PIN      63
#define RAMPSFD_E0_DIR_PIN     62
#define RAMPSFD_E0_ENABLE_PIN    48

#define RAMPSFD_X_STEP_PIN     36
#define RAMPSFD_X_DIR_PIN      28
#define RAMPSFD_X_ENABLE_PIN   42

////////////////////////////////////////

#include <Steppers/StepperRampsFD.h>
#define BOARDNAME RAMPSFD

////////////////////////////////////////////////////////

#define ROTARY_EN1           CAT(BOARDNAME,_LCD_ROTARY_EN1)
#define ROTARY_EN2           CAT(BOARDNAME,_LCD_ROTARY_EN2)
#define SD_ENABLE_PIN		 CAT(BOARDNAME,_SDSS_PIN)
