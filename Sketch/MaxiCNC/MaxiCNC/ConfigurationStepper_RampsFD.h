////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

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
