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

#define HARDWARETYPE_CNC3325         1 // CNC-3325, micro steps 32, DRV8825, CNCShield V3.51, zero
#define HARDWARETYPE_ESP32C6         2 // ESP32-C6 super mini, micro steps 32, DRV8825, CNCShield V3.51
#define HARDWARETYPE_CNCShield3x    10 // All "Test" must be configured with eeprom
#define HARDWARETYPE_L298N	        11 // (test for sketch maxsize with arduino nano)
#define HARDWARETYPE_SMC	        12 // 
#define HARDWARETYPE_TB6560         13 // 
#define HARDWARETYPE_CUSTOM	        99 // custom

#define HARDWARETYPE HARDWARETYPE_ESP32C6

////////////////////////////////////////////////////////

#define GO_DEFAULT_STEPRATE     ((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))  // steps/sec
#define G1_DEFAULT_MAXSTEPRATE  ((steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate)))  // steps/sec
#define G1_DEFAULT_FEEDPRATE    100000  // in mm1000 / min

////////////////////////////////////////////////////////

#if HARDWARETYPE==HARDWARETYPE_CNC3325
#include "ConfigurationMachine_CNC3325.h"
#elif HARDWARETYPE==HARDWARETYPE_ESP32C6
#include "ConfigurationMachine_Esp32C6.h"
#elif HARDWARETYPE==HARDWARETYPE_CNCShield3x
#include "ConfigurationMachine_CNCShield3x.h"
#elif HARDWARETYPE==HARDWARETYPE_L298N
#include "ConfigurationMachine_L298N.h"
#elif HARDWARETYPE==HARDWARETYPE_SMC
#include "ConfigurationMachine_SMC800.h"
#elif HARDWARETYPE==HARDWARETYPE_TB6560
#include "ConfigurationMachine_TB6560.h"
#elif HARDWARETYPE==HARDWARETYPE_CUSTOM
#include "ConfigurationMachine_Custom.h"
#else
#endif

////////////////////////////////////////////////////////

#define STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

////////////////////////////////////////////////////////

#define DISABLELEDBLINK

////////////////////////////////////////////////////////
