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
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include <Arduino.h>
#include "ConfigurationStepperLib.h"

//////////////////////////////////////////

#ifdef _MSC_VER

#else

#define strcpy_s strcpy
#define strncpy_s strncpy

#define _itoa(a,b,c) itoa(a,b,c)
#define _ltoa(a,b,c) ltoa(a,b,c)

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START  irqflags_t _sreg = SREG; cli();
#define CRITICAL_SECTION_END    SREG = _sreg;
#endif //CRITICAL_SECTION_START

#endif

////////////////////////////////////////////////////////

#define TIMER0VALUE(freq)	((timer_t)((uint32_t)TIMER0FREQUENCE/(uint32_t)(freq)))
#define TIMER1VALUE(freq)	((timer_t)((uint32_t)TIMER1FREQUENCE/(uint32_t)(freq)))
#define TIMER2VALUE(freq)	((timer_t)((uint32_t)TIMER2FREQUENCE/(uint32_t)(freq)))
//#define TIMER3VALUE(freq)	((timer_t)((uint32_t)TIMER3FREQUENCE/(uint32_t)freq))
//#define TIMER4VALUE(freq)	((timer_t)((uint32_t)TIMER4FREQUENCE/(uint32_t)freq))
//#define TIMER5VALUE(freq)	((timer_t)((uint32_t)TIMER5FREQUENCE/(uint32_t)freq))

#define TIMER2MICROSEC					(1.0/(float) TIMER2FREQUENCE*1000000.0)
#define TIMER2VALUEFROMMICROSEC(msec)	((timer_t)((msec) / TIMER2MICROSEC))

////////////////////////////////////////////////////////

#if defined(__SAM3X8E__)

typedef uint32_t pin_t;

#define ALWAYSINLINE		__attribute__((__always_inline__)) 
#define ALWAYSINLINE_SAM	__attribute__((__always_inline__)) 
#define ALWAYSINLINE_AVR
#define NEVER_INLINE		__attribute__((__noinline__))
#define NEVER_INLINE_AVR	__attribute__((__noinline__))
#define NEVER_INLINE_SAM
#define ALIGN_WORD			__attribute__((aligned (4)))

#elif defined(__SAMD21G18A__)

typedef uint32_t pin_t;

#define ALWAYSINLINE		__attribute__((__always_inline__)) 
#define ALWAYSINLINE_SAM	__attribute__((__always_inline__)) 
#define ALWAYSINLINE_AVR
#define NEVER_INLINE		__attribute__((__noinline__))
#define NEVER_INLINE_AVR	__attribute__((__noinline__))
#define NEVER_INLINE_SAM
#define ALIGN_WORD			__attribute__((aligned (4)))

#define irqflags_t uint8_t

#elif defined(__AVR_ARCH__)

#define ALWAYSINLINE		__attribute__((__always_inline__)) 
#define ALWAYSINLINE_AVR	__attribute__((__always_inline__)) 
#define ALWAYSINLINE_SAM
#define NEVER_INLINE		__attribute__((__noinline__))
#define NEVER_INLINE_AVR	__attribute__((__noinline__))
#define NEVER_INLINE_SAM
#define ALIGN_WORD			__attribute__((aligned (2)))

#define irqflags_t uint8_t
typedef uint8_t pin_t;

#else

#define ALWAYSINLINE
#define ALWAYSINLINE_AVR
#define NEVER_INLINE
#define NEVER_INLINE_AVR
#define NEVER_INLINE_SAM
#define ALIGN_WORD

#define irqflags_t uint8_t
typedef uint8_t pin_t;

#endif


////////////////////////////////////////////////////////

class CHAL
{
public:

	typedef void (*HALEvent)();

	// min 8 bit, AVR: HW Timer 0
	static void InitTimer0(HALEvent evt);
	static void RemoveTimer0();
	static void StartTimer0(timer_t timer);
	static void StopTimer0();

	// min 16 bit (AVR: 2MHZ, HW Timer1) 
	static void InitTimer1OneShot(HALEvent evt);
	static void RemoveTimer1();
	static void StartTimer1OneShot(timer_t timer);
	static void StopTimer1();

	static HALEvent _TimerEvent0;
	static HALEvent _TimerEvent1;

#if !defined( __AVR_ATmega328P__)

	// min 8 bit, (AVR: HW Timer5) 
	static void InitTimer2OneShot(HALEvent evt);
	static void RemoveTimer2();
	static void StartTimer2OneShot(timer_t   timer);
	static void ReStartTimer2OneShot(timer_t timer);
	static void StopTimer2();

	static HALEvent _TimerEvent2;

#endif

#if defined(__SAM3X8E__) || defined(__SAMD21G18A__)

	static void BackgroundRequest();
	static void InitBackground(HALEvent evt);

	static HALEvent _BackgroundEvent;

#endif

#if defined(__SAMD21G18A__)
	
#define EEPROM_SIZE	512		// must be x*256

	static const uint8_t _flashStorage[EEPROM_SIZE] __attribute__((__aligned__(256)));
	static uint8_t _flashBuffer[EEPROM_SIZE] __attribute__((__aligned__(4)));

	static void FlashWriteWords(uint32_t *flash_ptr, const uint32_t *data, uint32_t nwords);
	static void FlashErase(void *flash_ptr, uint32_t size);
	static void FlashEraseRow(void *flash_ptr);
	static void FlashRead(const void *flash_ptr, void *data, uint32_t size);

#endif

	static inline void DisableInterrupts() ALWAYSINLINE;
	static inline void EnableInterrupts() ALWAYSINLINE;

	static inline void DelayMicroseconds(unsigned int us) ALWAYSINLINE ;
	static inline void DelayMicroseconds0250() ALWAYSINLINE; // delay 1/4 us (4 nop on AVR)
	static inline void DelayMicroseconds0312() ALWAYSINLINE; // delay 0.312us (5 nop on AVR)
	static inline void DelayMicroseconds0375() ALWAYSINLINE; // delay 0.312us (6 nop on AVR)
	static inline void DelayMicroseconds0438() ALWAYSINLINE; // delay 0.312us (7 nop on AVR)
	static inline void DelayMicroseconds0500() ALWAYSINLINE; // delay 1/2 (8 nop on AVR)

	static inline irqflags_t GetSREG() ALWAYSINLINE;
	static inline void       SetSREG(irqflags_t) ALWAYSINLINE;

	static inline void pinMode(pin_t            pin, uint8_t mode);
	static inline void pinModeInput(pin_t       pin) NEVER_INLINE_AVR;
	static inline void pinModeInputPullUp(pin_t pin) NEVER_INLINE_AVR;
	static inline void pinModeOutput(pin_t      pin) NEVER_INLINE_AVR;

	static void    digitalWrite(pin_t pin, uint8_t lowOrHigh);
	static uint8_t digitalRead(pin_t  pin);

	static void analogWrite8(pin_t pin, uint8_t val);

	static uint16_t analogRead(pin_t pin);

	static void attachInterruptPin(pin_t pin, void (*userFunc)(), int mode) ALWAYSINLINE;

	static bool HaveEeprom() ALWAYSINLINE;
	static void InitEeprom() ALWAYSINLINE;
	static void FlushEeprom() ALWAYSINLINE;
	static bool NeedFlushEeprom() ALWAYSINLINE;

	static void     eeprom_write_dword(uint32_t*      __p, uint32_t __value) ALWAYSINLINE;
	static uint32_t eeprom_read_dword(const uint32_t* __p) ALWAYSINLINE;
	//static uint8_t eeprom_read_byte(const uint8_t * __p) ALWAYSINLINE;

	static uint32_t* GetEepromBaseAdr() ALWAYSINLINE;

#if defined(_MSC_VER)

	static void SetEepromFilename(const char* fileName) { _eepromFileName = fileName; }

private:

	static const char* _eepromFileName;
	static uint32_t    _eepromBuffer[2048];

#else

#endif
};

//////////////////////////////////////////

class CCriticalRegion
{
private:
	irqflags_t _sreg;

public:

	inline CCriticalRegion() ALWAYSINLINE : _sreg(CHAL::GetSREG()) { CHAL::DisableInterrupts(); };
	inline ~CCriticalRegion() ALWAYSINLINE { CHAL::SetSREG(_sreg); }
};

//////////////////////////////////////////

#include "HAL_AVR.h"
#include "HAL_Sam3x8e.h"
#include "HAL_SamD21g18a.h"
#include "HAL_Msvc.h"

//////////////////////////////////////////
