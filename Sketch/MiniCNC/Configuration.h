////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2017 Herbert Aitenbichler

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

//#define STEPPERTYPE 1		// CStepperL298N
//#define STEPPERTYPE 2		// CStepperSMC800
//#define STEPPERTYPE 3		// CStepperTB6560
#define STEPPERTYPE 4		// CStepperCNCShield

////////////////////////////////////////////////////////

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#if Generic

#define X_MAXSIZE 200000        // in mm1000_t
#define Y_MAXSIZE 200000 
#define Z_MAXSIZE 100000 
#define A_MAXSIZE 360000 
#define B_MAXSIZE 360000 
#define C_MAXSIZE 360000 

#define STEPSPERROTATION	200
#define MICROSTEPPING		16
#define SCREWLEAD			1.0

#define X_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Y_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Z_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define A_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define B_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define C_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

#define ConversionToMm1000 MyConvertToMm1000
#define ConversionToMachine MyConvertToMachine

////////////////////////////////////////////////////////

#define CNC_MAXSPEED 27000        // steps/sec
#define CNC_ACC  350
#define CNC_DEC  400

////////////////////////////////////////////////////////
// NoReference, ReferenceToMin, ReferenceToMax

#define X_USEREFERENCE	EReverenceType::ReferenceToMin
#define Y_USEREFERENCE	EReverenceType::ReferenceToMin
#define Z_USEREFERENCE	EReverenceType::ReferenceToMax
#define A_USEREFERENCE	EReverenceType::NoReference
#define B_USEREFERENCE  EReverenceType::NoReference
#define C_USEREFERENCE  EReverenceType::NoReference

#define REFMOVE_1_AXIS  255
#define REFMOVE_2_AXIS  255
#define REFMOVE_3_AXIS  255
#define REFMOVE_4_AXIS  255
#define REFMOVE_5_AXIS  255
#define REFMOVE_6_AXIS  255

#undef SPINDEL_ANALOGSPEED
#define SPINDEL_MAXSPEED	255			// analog 255

#else

#define X_MAXSIZE 160000        // in mm1000_t
#define Y_MAXSIZE 105000 
#define Z_MAXSIZE 30000 
#define A_MAXSIZE 360000 
#define B_MAXSIZE 360000 
#define C_MAXSIZE 360000 

#define STEPSPERROTATION  200
#define MICROSTEPPING   16
#define SCREWLEAD     4.0

#define X_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Y_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Z_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define A_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define B_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define C_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

#define ConversionToMm1000 MyConvertToMm1000
#define ConversionToMachine MyConvertToMachine

////////////////////////////////////////////////////////

#define CNC_MAXSPEED 30000        // steps/sec
#define CNC_ACC  350
#define CNC_DEC  400

////////////////////////////////////////////////////////
// NoReference, ReferenceToMin, ReferenceToMax

#define X_USEREFERENCE  EReverenceType::NoReference
#define Y_USEREFERENCE  EReverenceType::NoReference
#define Z_USEREFERENCE  EReverenceType::NoReference
#define A_USEREFERENCE  EReverenceType::NoReference
#define B_USEREFERENCE  EReverenceType::NoReference
#define C_USEREFERENCE  EReverenceType::NoReference

#define REFMOVE_1_AXIS  255
#define REFMOVE_2_AXIS  255
#define REFMOVE_3_AXIS  255
#define REFMOVE_4_AXIS  255
#define REFMOVE_5_AXIS  255
#define REFMOVE_6_AXIS  255

#define MOVEAWAYFROMREF_MM1000 500

#define SPINDEL_ANALOGSPEED
#define SPINDEL_MAXSPEED	10000			// analog 255

#endif

////////////////////////////////////////////////////////

#if STEPPERTYPE==1
#include "Configuration_L298N.h"
#elif STEPPERTYPE==2
#include "Configuration_SMC800.h"
#elif STEPPERTYPE==3
#include "Configuration_TB6560.h"
#elif STEPPERTYPE==4
#include "Configuration_CNCShield.h"
#endif

////////////////////////////////////////////////////////

#define GO_DEFAULT_STEPRATE		((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, maxsteprate)))	// steps/sec
#define G1_DEFAULT_MAXSTEPRATE	((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, maxsteprate)))	// steps/sec
#define G1_DEFAULT_FEEDPRATE	100000	// in mm1000 / min

#define STEPRATERATE_REFMOVE	(CNC_MAXSPEED/4)

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_Starting					F("MiniCNC:" __DATE__ )
