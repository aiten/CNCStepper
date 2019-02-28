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
// AVR 8bit
////////////////////////////////////////////////////////

#if defined(__AVR_ARCH__)

//#define pgm_read_ptr pgm_read_word
#define pgm_read_int pgm_read_word
#define pgm_read_uint pgm_read_word

#define TIMER0PRESCALE      64
#define TIMER0FREQUENCE		(F_CPU/TIMER0PRESCALE)

#define TIMER1PRESCALE      8
#define TIMER1FREQUENCE		(F_CPU/TIMER1PRESCALE)

#define TIMER1MIN			32
#define TIMER1MAX			0xffff

#define TIMER2PRESCALE      8
#define TIMER2FREQUENCE		(F_CPU/TIMER2PRESCALE)

#define MAXINTERRUPTSPEED	(65535/7)	// maximal possible interrupt rate => steprate_t

#define SPEED_MULTIPLIER_1	0
#define SPEED_MULTIPLIER_2	(MAXINTERRUPTSPEED*1)
#define SPEED_MULTIPLIER_3	(MAXINTERRUPTSPEED*2)
#define SPEED_MULTIPLIER_4	(MAXINTERRUPTSPEED*3)
#define SPEED_MULTIPLIER_5	(MAXINTERRUPTSPEED*4)
#define SPEED_MULTIPLIER_6	(MAXINTERRUPTSPEED*5)
#define SPEED_MULTIPLIER_7	(MAXINTERRUPTSPEED*6)

#define TIMEROVERHEAD		(14)		// decrease Timervalue for ISR overhead before set new timer

#include <avr/interrupt.h>
#include <avr/io.h>

#include "fastio.h"

////////////////////////////////////////////////////////
// For shorter delays use assembly language call 'nop' (no operation). Each 'nop' statement executes in one machine cycle (at 16 MHz) yielding a 62.5 ns (nanosecond) delay. 

inline void CHAL::DelayMicroseconds0250() {	__asm__("nop\n\tnop\n\tnop\n\tnop\n\t"); }

inline void CHAL::DelayMicroseconds0312() {	__asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"); }

inline void CHAL::DelayMicroseconds0375() { __asm__("nop\n\tnop\n\tnop\n\tnopn\tnop\n\tnop\n\t"); }

inline void CHAL::DelayMicroseconds0438() { __asm__("nop\n\tnop\n\tnop\n\tnopn\tnop\n\tnop\n\tnop\n\t"); }

inline void CHAL::DelayMicroseconds0500() {	__asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"); }

inline void CHAL::DelayMicroseconds(unsigned int us) {	::delayMicroseconds(us); }

inline void CHAL::DisableInterrupts()	{	cli(); }
inline void CHAL::EnableInterrupts()	{	sei(); }

inline irqflags_t CHAL::GetSREG()		{ return SREG; }
inline void CHAL::SetSREG(irqflags_t a)	{ SREG=a; }

inline void  CHAL::RemoveTimer0()		{}

inline void  CHAL::InitTimer0(HALEvent evt)
{
	// shared with millis!
	_TimerEvent0 = evt;
}

////////////////////////////////////////////////////////

inline void CHAL::StartTimer0(timer_t)
{
	// shared with millis => set only interrupt mask!
	TIMSK0 |= (1<<OCIE0B);  
	OCR0B = 128;
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer0()
{
	TIMSK0 &= ~(1<<OCIE0B);  
}  

////////////////////////////////////////////////////////

inline void  CHAL::RemoveTimer1() {}

inline void  CHAL::InitTimer1OneShot(HALEvent evt)
{
	_TimerEvent1 = evt;

	TCCR1A = 0x00;							// stetzt Statusregiser A Vom Timer eins auf null
	TCCR1B = 0x00;							// stetzt Statusregiser B Vom Timer eins auf null
	TCCR1B |= (1<<CS11);					// timer laeuft mit 1/8 des CPU Takt.
}

////////////////////////////////////////////////////////

inline void CHAL::StartTimer1OneShot(timer_t timer)
{
	TCNT1  = 0 - timer;  
	TIMSK1 |= (1<<TOIE1);					// Aktiviert Interrupt beim Overflow des Timers 1
	TCCR1B |= (1<<CS11) ;					// timer laeuft mit 1/8 des CPU Takt.
	TIFR1  |= (1<<TOV1);					// clear the overflow flag
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer1()
{
	TCCR1B = 0x00;							// stetzt Statusregiser B Vom Timer eins auf null
	TIMSK1 &= ~(1<<TOIE1);					// Deaktiviert Interrupt beim Overflow des Timers 1
	TCNT1=0;  
}  

////////////////////////////////////////////////////////

#if !defined(__AVR_ATmega328P__)

////////////////////////////////////////////////////////
// Timer2 => HW Timer 5

inline void  CHAL::RemoveTimer2() {}

inline void  CHAL::InitTimer2OneShot(HALEvent evt)
{
	_TimerEvent2 = evt;

	TCCR5A = 0x00;							// stetzt Statusregiser A Vom Timer eins auf null
	TCCR5B = 0x00;							// stetzt Statusregiser B Vom Timer eins auf null
	TCCR5B |= (1<<CS51);					// timer laeuft mit 1/8 des CPU Takt.
}

////////////////////////////////////////////////////////

inline void CHAL::StartTimer2OneShot(timer_t timer)
{
	ReStartTimer2OneShot(timer);
	TIMSK5 |= (1<<TOIE5);					// Aktiviert Interrupt beim Overflow des Timers 
	TCCR5B |= (1<<CS51);					// timer laeuft mit 1/8 des CPU Takt.
}

inline void CHAL::ReStartTimer2OneShot(timer_t timer)
{
	TCNT5 = 0 - timer;
	TIFR5 |= (1 << TOV5);					// clear the overflow flag
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer2()
{
	TCCR5B = 0x00;							// stetzt Statusregiser B Vom Timer eins auf null
	TIMSK5 &= ~(1<<TOIE5);					// Deaktiviert Interrupt beim Overflow des Timers 
	TCNT5=0;  
}  

#endif

////////////////////////////////////////////////////////

#define HALFastdigitalWrite(a,b) WRITE(a,b)
#define HALFastdigitalWriteNC(a,b) _WRITE_NC(a,b)
#define HALFastdigitalRead(a) READ(a)

inline void CHAL::pinMode(pin_t pin, uint8_t mode)
{
	::pinMode(pin, mode);
}

void CHAL::pinModeOutput(pin_t pin)
{ 
	::pinMode(pin, OUTPUT);
}

void CHAL::pinModeInputPullUp(pin_t pin)
{
	::pinMode(pin, INPUT_PULLUP);
}

inline void CHAL::attachInterruptPin(pin_t pin, void(*userFunc)(void), int mode)
{
	::attachInterrupt(digitalPinToInterrupt(pin), userFunc, mode);
}

inline void CHAL::eeprom_write_dword(uint32_t *  __p, uint32_t  	__value)
{ 
	::eeprom_write_dword(__p, __value);
}

inline uint32_t CHAL::eeprom_read_dword(const uint32_t * __p)
{ 
	return ::eeprom_read_dword(__p);
}

/*
inline uint8_t CHAL::eeprom_read_byte(const uint8_t * __p)
{ 
	return ::eeprom_read_byte(__p);
}
*/

inline bool CHAL::HaveEeprom()
{
	return true;
}

inline uint32_t* CHAL::GetEepromBaseAdr()
{
	return (uint32_t*)NULL;
}

inline void CHAL::InitEeprom()
{
}

inline void CHAL::FlushEeprom()
{
}

inline bool CHAL::NeedFlushEeprom()
{
	return false;
}

#endif
