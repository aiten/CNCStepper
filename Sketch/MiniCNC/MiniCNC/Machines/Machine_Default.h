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

#define X_MAXSIZE 100000        // in mm1000_t
#define Y_MAXSIZE 100000
#define Z_MAXSIZE 50000
#define A_MAXSIZE 360000
#define B_MAXSIZE 360000
#define C_MAXSIZE 360000

////////////////////////////////////////////////////////
// NoReference, ReferenceToMin, ReferenceToMax

#define X_USEREFERENCE	EReverenceType::NoReference
#define Y_USEREFERENCE	EReverenceType::NoReference
#define Z_USEREFERENCE	EReverenceType::NoReference
#define A_USEREFERENCE	EReverenceType::NoReference
#define B_USEREFERENCE	EReverenceType::NoReference
#define C_USEREFERENCE	EReverenceType::NoReference

#define REFMOVE_1_AXIS  255
#define REFMOVE_2_AXIS  255
#define REFMOVE_3_AXIS  255
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
#define SPINDLE_MAXSPEED  10000     // analog 255
#define SPINDLE_FADETIMEDELAY  8    // 8ms * 255 => 2040ms from 0 to max, 4080 from -max to +max
//#define SPINDLE_FADE

////////////////////////////////////////////////////////

#define STEPRATE_REFMOVE    (CNC_MAXSPEED/3)

////////////////////////////////////////////////////////
