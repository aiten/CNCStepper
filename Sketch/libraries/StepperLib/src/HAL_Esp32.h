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
// Due 32Bit
////////////////////////////////////////////////////////

#if defined(ESP32)

#undef pgm_read_ptr
inline  const void* pgm_read_ptr(const void* p) { return *((void **)p); }
inline  int pgm_read_int(const void* p) { return * ((const int*) p); }

// Clock should be AVR Compatible 2Mhz => 48Mhz/8(prescaler)/3(genclk)
#define TIMERCLOCKDIV		3	
#define TIMER0_CLKGEN		5
#define TIMER1_CLKGEN		5

#define TIMERBASEFREQUENCE	(F_CPU/TIMER1_CLKGEN)


#define TIMER0FREQUENCE		(TIMERBASEFREQUENCE/TIMER0PRESCALE)
#define TIMER0PRESCALE      1024			

// compatible to AVR => no 32 bit Timers
#if 1
#define TIMER1FREQUENCE		2000000L	
//#define TIMER1PRESCALE      (8*3)
#else
#define TIMER1FREQUENCE		(TIMERBASEFREQUENCE/TIMER1PRESCALE)
#define TIMER1PRESCALE      2			
#endif

#define TIMER1MIN			4
#define TIMER1MAX			0xffffffffl

#define TIMER0_INTERRUPTPRIORITY	3		// Polling
#define TIMER1_INTERRUPTPRIORITY	2		// Main timer for stepper, USB => 1
#define BACKGROUND_INTERRUPTPRIORITY	3

#define MAXINTERRUPTSPEED	(65535/7)		// maximal possible interrupt rate => steprate_t

#define SPEED_MULTIPLIER_1	0
#define SPEED_MULTIPLIER_2	(MAXINTERRUPTSPEED*1)
#define SPEED_MULTIPLIER_3	(MAXINTERRUPTSPEED*2)
#define SPEED_MULTIPLIER_4	(MAXINTERRUPTSPEED*3)
#define SPEED_MULTIPLIER_5	(MAXINTERRUPTSPEED*4)
#define SPEED_MULTIPLIER_6	(MAXINTERRUPTSPEED*5)
#define SPEED_MULTIPLIER_7	(MAXINTERRUPTSPEED*6)

#define TIMEROVERHEAD		1				// decrease Timervalue for ISR overhead before set new timer


inline void CHAL::DisableInterrupts()		{ noInterrupts(); }
inline void CHAL::EnableInterrupts()		{ interrupts(); }


#define HALFastdigitalRead(a)	CHAL::digitalRead(a)
#define HALFastdigitalWrite(a,b) CHAL::digitalWrite(a,b)
#define HALFastdigitalWriteNC(a,b) CHAL::digitalWrite(a,b)

inline uint8_t CHAL::digitalRead(pin_t pin)
{
	return ::digitalRead(pin);
}

inline void CHAL::digitalWrite(pin_t pin, uint8_t val)
{
	::digitalWrite(pin,val);
}

inline void CHAL::pinMode(pin_t pin, uint8_t mode)
{ 
	::pinMode(pin,mode); 
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

inline void CHAL::attachInterruptPin(pin_t pin, void(*userFunc)(void), int mode)
{
	::attachInterrupt(digitalPinToInterrupt(pin), userFunc, mode);
}

///////////////////////////////////////////

inline void CHAL::eeprom_write_dword(uint32_t* ptr_buffer, uint32_t value)
{
	*ptr_buffer = value;
}

inline uint32_t CHAL::eeprom_read_dword(const uint32_t* ptr_buffer)
{
	return *ptr_buffer;
}

/*
inline uint8_t CHAL::eeprom_read_byte(const uint8_t *)
{
	return 0;
}
*/

inline bool CHAL::HaveEeprom()
{
	return true;
}

inline uint32_t* CHAL::GetEepromBaseAdr()
{
	return (uint32_t*) _flashBuffer;
}

inline void CHAL::InitEeprom()
{
	FlashRead(_flashStorage, _flashBuffer, sizeof(_flashStorage));
}

inline void CHAL::FlushEeprom()
{
	FlashErase((void*)_flashStorage, sizeof(_flashBuffer));
	FlashWriteWords((uint32_t*) _flashStorage, (uint32_t*) _flashBuffer, (sizeof(_flashStorage)+sizeof(uint32_t)-1)/sizeof(uint32_t));
}

inline bool CHAL::NeedFlushEeprom()
{
	return true;
}

////////////////////////////////////////////////////////

inline void CHAL::DelayMicroseconds(unsigned int usec)
{
	::delayMicroseconds(usec);
}

inline void CHAL::DelayMicroseconds0500()
{
}

inline void CHAL::DelayMicroseconds0250()
{
}

////////////////////////////////////////////////////////


inline void  CHAL::RemoveTimer0() {}

inline void CHAL::StartTimer0(timer_t delay)
{
}

inline void  CHAL::InitTimer0(HALEvent evt)
{
}

inline void CHAL::StopTimer0()
{
}

////////////////////////////////////////////////////////

inline TcCount16* GetTimer1Struct() { return (TcCount16*)TC4; }

inline void  CHAL::RemoveTimer1() {}

inline void CHAL::StartTimer1OneShot(timer_t delay)
{
}

////////////////////////////////////////////////////////

inline void  CHAL::InitTimer1OneShot(HALEvent evt)
{
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer1()
{
}

////////////////////////////////////////////////////////

#endif
