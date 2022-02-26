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

#include <Arduino.h>

#pragma once

//#define StepperSerial SerialUSB
//#define StepperSerial Serial
extern class HardwareSerial& StepperSerial;

////////////////////////////////////////////////////////

//#define _NO_LONG_MESSAGE
//#define _NO_DUMP

////////////////////////////////////////////////////////

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define A_AXIS 3	// rotary around X
#define B_AXIS 4	// rotary around Y
#define C_AXIS 5	// rotary around Z
#define U_AXIS 6	// Relative axis parallel to U
#define V_AXIS 7	// Relative axis parallel to V
#define W_AXIS 8	// Relative axis parallel to W

////////////////////////////////////////////////////////

typedef uint8_t axis_t;		// type for "axis"

typedef int32_t  sdist_t;	// type of stepper coordinate system (signed)
typedef uint32_t udist_t;	// type of stepper coordinate system (unsigned)

typedef const __FlashStringHelper* FLSTR;

typedef FLSTR error_t;

////////////////////////////////////////////////////////
//
// Stepper

#define REFERENCESTABLETIME	2						// time in ms for reference must not change (in Reference move) => signal bounce

#define IDLETIMER1VALUE		TIMER1VALUE(31)			// Idle timer value (stepper timer not moving), must fit into 16 bit
#define TIMEOUTSETIDLE		1			   			// set level after 1s

#define WAITTIMER1VALUE		TIMER1VALUE(100)		// Idle timer value for "no step" movement

#define TIMER1VALUEMAXSPEED	TIMER1VALUE(STEPRATE_MAX)

////////////////////////////////////////////////////////

#define SYNC_STEPBUFFERCOUNT	8					// allow only x element in step buffer when io or wait starts

////////////////////////////////////////////////////////

#define REDUCED_SIZE_virtual virtual				// only virtual for "full" version

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// usual with Ramps1.4

#undef use32bit
#define use16bit

#define NUM_AXIS			5

#define STEPBUFFERSIZE		128		// size 2^x but not 256
#define MOVEMENTBUFFERSIZE	64

////////////////////////////////////////////////////////

#elif defined(__AVR_ATmega328P__)

// usual with SMC800, L298N or TB6560

#undef use32bit
#define use16bit

#define STEPBUFFERSIZE		16		// size 2^x but not 256
#define MOVEMENTBUFFERSIZE	8

#define NUM_AXIS 4

#define REDUCED_SIZE
#define _NO_LONG_MESSAGE
#define _NO_DUMP

#undef REDUCED_SIZE_virtual
#define REDUCED_SIZE_virtual	// no "virtual" for reduced_size 

////////////////////////////////////////////////////////

#elif defined(__SAM3X8E__) || defined(__SAMD21G18A__)

// usual with Ramps FD

#define use32bit
#undef use16bit

#define NUM_AXIS			6

#define STEPBUFFERSIZE		128		// size 2^x but not 256
#define MOVEMENTBUFFERSIZE	64

////////////////////////////////////////////////////////

#elif defined (_MSC_VER)

// test environment only

typedef unsigned long long uint64_t;

//#undef use32bit
//#define use16bit

#undef use16bit
#define use32bit

#define STEPBUFFERSIZE		16		// size 2^x but not 256
#define MOVEMENTBUFFERSIZE	8

//#define NUM_AXIS 5
#define NUM_AXIS 3

#undef REFERENCESTABLETIME
#define REFERENCESTABLETIME	0

#define MOVEMENTINFOSIZE	128

//#define REDUCED_SIZE

////////////////////////////////////////////////////////

#else
ToDo;
#endif

////////////////////////////////////////////////////////
// Global types and configuration
////////////////////////////////////////////////////////

#if defined(use16bit)

#define MAXSTEPSPERMOVE		0xffff			// split in moves
#define MAXACCDECSTEPS		(0x10000/4 -10)	// max stepps for acc and dec ramp ( otherwise overrun)

#define STEPRATE_MAX		(65535l)		// see range for steprate_t

typedef uint16_t timer_t;			// timer type (16bit)
typedef uint16_t mdist_t;			// type for one movement (16bit)
typedef uint16_t steprate_t;		// type for speed (Hz), Steps/sec

#define mudiv	udiv
#define mudiv_t	udiv_t

typedef uint32_t uintXX_t;	// 16 => 32
typedef int32_t  intXX_t;	// 16 => 32;

////////////////////////////////////////////////////////

#elif defined(use32bit)

#define MAXSTEPSPERMOVE		0xffffffff	// split in Moves
#define MAXACCDECSTEPS		0x1000000

#define STEPRATE_MAX		(128000l)	// limit steprate_t

typedef uint32_t timer_t;    // timer type (32bit)
typedef uint32_t mdist_t;    // type for one movement (32bit)
typedef uint32_t steprate_t; // type for speed (Hz), Steps/sec

#define mudiv	ldiv
#define mudiv_t	ldiv_t

typedef uint64_t uintXX_t;		// 32 => 64 
typedef int64_t  intXX_t;		// 32 => 64

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

#define EnumAsByte(a) a
#define debugvirtual virtual
#define stepperstatic 
#define stepperstatic_avr 

#else

#if defined(__AVR_ARCH__)
#define stepperstatic_avr static
#else
#define stepperstatic_avr 
#endif


#define stepperstatic static
#define stepperstatic_
#define EnumAsByte(a) uint8_t			// use a 8 bit enum (and not 16, see compiler output)
#define debugvirtual					// only used in msvc for debugging - not used on AVR controller 

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_REFERENCE (NUM_AXIS*2)

/////////////////////////////////////////////////////////////////////////////////////////////////

typedef uint8_t axisArray_t;			// on bit per axis

#if NUM_AXIS > 3
typedef uint32_t DirCount_t;			// 4 bit for each axis (0..7) count, 8 dirUp, see DirCountAll_t
#define DirCountBytes 4
#else
typedef uint16_t DirCount_t;			// 4 bit for each axis (0..7) count, 8 dirUp 
#define DirCountBytes 2
#endif

#if NUM_AXIS > 7
#error "NUM_AXIS must be < 8"				// because of last dirCount_t used for info 
#endif

struct DirCountByte_t
{
	union
	{
		struct
		{
			struct DirCountStepByte_t
			{
				uint8_t count1 : 3;
				uint8_t dirUp1 : 1;

				uint8_t count2 : 3;
				uint8_t dirUp2 : 1;
			} byte[DirCountBytes - 1];

			struct DirCountInfoByte_t
			{
				uint8_t count1 : 3;
				uint8_t dirUp1 : 1;

				uint8_t nocount : 1; // do not count step (e.g. move for backlash)
				uint8_t unused1 : 1;
				uint8_t unused2 : 1;
				uint8_t unused3 : 1;
			} byteInfo;
		}     byte;

		DirCount_t all;
	};
};

////////////////////////////////////////////////////////

#include "MessageStepperLib.h"

////////////////////////////////////////////////////////
