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
// MSC
////////////////////////////////////////////////////////

#if defined(_MSC_VER)

#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define pgm_read_int pgm_read_dword
#define pgm_read_uint pgm_read_dword

#define TIMER0FREQUENCE		62500L
#define TIMER1FREQUENCE		2000000L
#define TIMER2FREQUENCE		62500L
#define TIMER3FREQUENCE		62500L
#define TIMER4FREQUENCE		62500L
#define TIMER5FREQUENCE		62500L

#define TIMER1MIN			40
#define TIMER1MAX			0xffff

#define MAXINTERRUPTSPEED	(65535/7)		// maximal possible interrupt rate => steprate_t

#define SPEED_MULTIPLIER_1	0
#define SPEED_MULTIPLIER_2	(MAXINTERRUPTSPEED*1)
#define SPEED_MULTIPLIER_3	(MAXINTERRUPTSPEED*2)
#define SPEED_MULTIPLIER_4	(MAXINTERRUPTSPEED*3)
#define SPEED_MULTIPLIER_5	(MAXINTERRUPTSPEED*4)
#define SPEED_MULTIPLIER_6	(MAXINTERRUPTSPEED*5)
#define SPEED_MULTIPLIER_7	(MAXINTERRUPTSPEED*6)

#define TIMEROVERHEAD		(0)				// decrease TimerValue for ISR overhead before set new timer

inline void CHAL::DisableInterrupts() { cli(); }
inline void CHAL::EnableInterrupts() { sei(); }

inline void CHAL::DelayMicroseconds0250() {}
inline void CHAL::DelayMicroseconds0312() {}
inline void CHAL::DelayMicroseconds0375() {}
inline void CHAL::DelayMicroseconds0438() {}
inline void CHAL::DelayMicroseconds0500() {}

inline void CHAL::DelayMicroseconds(unsigned int) {}

inline irqflags_t CHAL::GetSREG() { return SREG; }
inline void       CHAL::SetSREG(irqflags_t a) { SREG = a; }

#define __asm__(a)

inline void CHAL::InitTimer0(HALEvent evt) { _TimerEvent0 = evt; }

inline void CHAL::RemoveTimer0() {}
inline void CHAL::StartTimer0(timer_t) {}
inline void CHAL::StopTimer0() {}

inline void CHAL::InitTimer1OneShot(HALEvent evt) { _TimerEvent1 = evt; }
inline void CHAL::RemoveTimer1() {}
inline void CHAL::StartTimer1OneShot(timer_t) {}

inline void CHAL::StopTimer1() {}
inline void CHAL::InitTimer2OneShot(HALEvent evt) { _TimerEvent2 = evt; }
inline void CHAL::RemoveTimer2() {}
inline void CHAL::StartTimer2OneShot(timer_t  ) {}
inline void CHAL::ReStartTimer2OneShot(timer_t) {}
inline void CHAL::StopTimer2() {}

/*
inline void CHAL::InitTimer3(HALEvent evt){ _TimerEvent3 = evt; }
inline void CHAL::RemoveTimer3()			{}
inline void CHAL::StartTimer3(timer_t)		{}
inline void CHAL::StopTimer3()				{}

inline void CHAL::InitTimer4(HALEvent evt){ _TimerEvent4 = evt; }
inline void CHAL::RemoveTimer4()			{}
inline void CHAL::StartTimer4(timer_t)		{}
inline void CHAL::StopTimer4()				{}

inline void CHAL::InitTimer5(HALEvent evt){ _TimerEvent5 = evt; }
inline void CHAL::RemoveTimer5()			{}
inline void CHAL::StartTimer5(timer_t)		{}
inline void CHAL::StopTimer5()				{}
*/
#define HALFastdigitalRead(a) CHAL::digitalRead(a)
#define HALFastdigitalWrite(a,b) CHAL::digitalWrite(a,b)
#define HALFastdigitalWriteNC(a,b) CHAL::digitalWrite(a,b)

inline void CHAL::digitalWrite(pin_t pin, uint8_t lowOrHigh)
{
	::digitalWrite(pin, lowOrHigh);
}

inline uint8_t CHAL::digitalRead(pin_t pin)
{
	return ::digitalRead(pin);
}

inline void CHAL::analogWrite8(pin_t pin, uint8_t val)
{
	::analogWrite(pin, val);
}

inline void CHAL::pinModeOutput(pin_t pin)
{
	::pinMode(pin, OUTPUT);
}

inline void CHAL::pinModeInputPullUp(pin_t pin)
{
	::pinMode(pin, INPUT_PULLUP);
}

inline void CHAL::pinMode(pin_t pin, uint8_t mode)
{
	::pinMode(pin, mode);
}

inline void CHAL::eeprom_write_dword(uint32_t* __p, uint32_t __value)
{
	::eeprom_write_dword(__p, __value);
}

inline uint32_t CHAL::eeprom_read_dword(const uint32_t* __p)
{
	return ::eeprom_read_dword(__p);
}

/*
inline uint8_t CHAL::eeprom_read_byte(const uint8_t * __p)
{
	return ::eeprom_read_byte(__p);
}
*/

inline uint32_t* CHAL::GetEepromBaseAdr()
{
	return _eepromBuffer;
}

inline bool CHAL::NeedFlushEeprom()
{
	return true;
}

////////////////////////////////////////////////////////

#endif
