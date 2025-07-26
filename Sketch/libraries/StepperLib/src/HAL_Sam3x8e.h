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

#if defined(__SAM3X8E__)

#define PIN_A0   (54)
#define PIN_A1   (55)
#define PIN_A2   (56)
#define PIN_A3   (57)
#define PIN_A4   (58)
#define PIN_A5   (59)
#define PIN_A6   (60)
#define PIN_A7   (61)
#define PIN_A8   (62)
#define PIN_A9   (63)
#define PIN_A10  (64)
#define PIN_A11  (65)

#include <itoa.h>

#undef pgm_read_ptr
inline  const void* pgm_read_ptr(const void* p) { return *((void **)p); }
inline  int pgm_read_int(const void* p) { return * ((const int*) p); }

#define TIMER0FREQUENCE		(F_CPU/TIMER0PRESCALE)
#define TIMER0PRESCALE      2			

// compatible to AVR => no 32 bit Timers
#if 1
#define TIMER1FREQUENCE		2000000L	
#define TIMER1PRESCALE      8			
#else
#define TIMER1FREQUENCE		(F_CPU/TIMER1PRESCALE)
#define TIMER1PRESCALE      2			
#endif

#define TIMER1MIN			4
#define TIMER1MAX			0xffffffffl

#define TIMER2FREQUENCE		2000000L	
#define TIMER2PRESCALE      8			
//#define TIMER2FREQUENCE		(F_CPU/TIMER2PRESCALE)

/*
#define TIMER3FREQUENCE		(F_CPU/TIMER3PRESCALE)
#define TIMER3PRESCALE      2			

#define TIMER4FREQUENCE		(F_CPU/TIMER4PRESCALE)
#define TIMER4PRESCALE      2			

#define TIMER5FREQUENCE		(F_CPU/TIMER5PRESCALE)
#define TIMER5PRESCALE      2			
*/
#define MAXINTERRUPTSPEED	(65535/7)		// maximal possible interrupt rate => steprate_t

#define SPEED_MULTIPLIER_1	0
#define SPEED_MULTIPLIER_2	(MAXINTERRUPTSPEED*1)
#define SPEED_MULTIPLIER_3	(MAXINTERRUPTSPEED*2)
#define SPEED_MULTIPLIER_4	(MAXINTERRUPTSPEED*3)
#define SPEED_MULTIPLIER_5	(MAXINTERRUPTSPEED*4)
#define SPEED_MULTIPLIER_6	(MAXINTERRUPTSPEED*5)
#define SPEED_MULTIPLIER_7	(MAXINTERRUPTSPEED*6)

#define TIMEROVERHEAD		1				// decrease Timervalue for ISR overhead before set new timer

inline void CHAL::DisableInterrupts()		{	cpu_irq_disable(); }
inline void CHAL::EnableInterrupts()		{	cpu_irq_enable(); }

inline irqflags_t CHAL::GetSREG()			{ return cpu_irq_save(); }
inline void CHAL::SetSREG(irqflags_t a)		{ cpu_irq_restore(a); }

// use CAN as BackGroundWorker thread
#define IRQTYPE CAN0_IRQn

inline void CHAL::BackgroundRequest()			{ NVIC_SetPendingIRQ(IRQTYPE); }
inline void CHAL::InitBackground(HALEvent evt)	{ NVIC_EnableIRQ(IRQTYPE);  NVIC_SetPriority(IRQTYPE, NVIC_EncodePriority(4, 7, 0)); _BackgroundEvent = evt; }

#define HALFastdigitalRead(a)	CHAL::digitalRead(a)
#define HALFastdigitalWrite(a,b) CHAL::digitalWrite(a,b)
#define HALFastdigitalWriteNC(a,b) CHAL::digitalWrite(a,b)
/*
inline void digitalWriteDirect(int pin, boolean val){
  if(val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
  else    g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
}

inline int digitalReadDirect(int pin){
  return !!(g_APinDescription[pin].pPort -> PIO_PDSR & g_APinDescription[pin].ulPin);
}
*/
inline uint8_t CHAL::digitalRead(pin_t pin)
{
  return (g_APinDescription[pin].pPort -> PIO_PDSR & g_APinDescription[pin].ulPin) ? HIGH : LOW;
//	return ::digitalReadDirect(pin);
}

inline void CHAL::digitalWrite(pin_t pin, uint8_t val)
{
  if(val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
  else    g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
  //	digitalWriteDirect(pin,lowOrHigh);
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

#include "HAL_I2CEEprom.h"

inline void CHAL::eeprom_write_dword(uint32_t *eeadr, uint32_t value)
{
	CHAL_I2C_EEprom24C256::i2c_eeprom_write_dword((unsigned int)eeadr,value);
}

inline uint32_t CHAL::eeprom_read_dword(const uint32_t *eeadr)
{
	return CHAL_I2C_EEprom24C256::i2c_eeprom_read_dword((unsigned int)eeadr);
}
/*
inline uint8_t CHAL::eeprom_read_byte(const uint8_t *eeadr)
{
	return CHAL_I2C_EEprom24C256::i2c_eeprom_read_byte((unsigned int) eeadr);
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
	CHAL_I2C_EEprom24C256::Init();
}

inline void CHAL::FlushEeprom()
{
}

inline bool CHAL::NeedFlushEeprom()
{
	return false;
}

////////////////////////////////////////////////////////

inline void CHAL::DelayMicroseconds(unsigned int usec)
{
	::delayMicroseconds(usec);
}

inline void CHAL::DelayNanoseconds(unsigned int) {}

inline void CHAL::DelayMicroseconds0500()
{
	// uint32_t n = usec * (VARIANT_MCK / 3000000);
	uint32_t n = 1 * (VARIANT_MCK / 3000000) / 2;
	asm volatile(
		"L_%=_delayMicroseconds:"       "\n\t"
		"subs   %0, #1"                 "\n\t"
		"bne    L_%=_delayMicroseconds" "\n"
		: "+r" (n) :
		);
}

inline void CHAL::DelayMicroseconds0250()
{
	// uint32_t n = usec * (VARIANT_MCK / 3000000);
	uint32_t n = 1 * (VARIANT_MCK / 3000000) / 4;
	asm volatile(
		"L_%=_delayMicroseconds:"       "\n\t"
		"subs   %0, #1"                 "\n\t"
		"bne    L_%=_delayMicroseconds" "\n"
		: "+r" (n) :
		);
}


////////////////////////////////////////////////////////

#define DUETIMER2_TC					TC2
#define DUETIMER2_CHANNEL				1
#define DUETIMER2_IRQTYPE				((IRQn_Type) ID_TC7)

#define DUETIMER1_TC					TC2
#define DUETIMER1_CHANNEL				2
#define DUETIMER1_IRQTYPE				((IRQn_Type) ID_TC8)

#define DUETIMER0_TC					TC2
#define DUETIMER0_CHANNEL				0
#define DUETIMER0_IRQTYPE				((IRQn_Type) ID_TC6)

////////////////////////////////////////////////////////

inline void  CHAL::RemoveTimer0() {}

inline void CHAL::StartTimer0(timer_t timer_count)
{
	if (timer_count == 0) timer_count = 1;
	TC_SetRC(DUETIMER0_TC, DUETIMER0_CHANNEL, timer_count);
	TC_Start(DUETIMER0_TC, DUETIMER0_CHANNEL);
}

////////////////////////////////////////////////////////

inline void  CHAL::InitTimer0(HALEvent evt)
{
	_TimerEvent0 = evt;

	pmc_enable_periph_clk(DUETIMER0_IRQTYPE);
	NVIC_SetPriority(DUETIMER0_IRQTYPE, NVIC_EncodePriority(4, 3, 0));

	TC_Configure(DUETIMER0_TC, DUETIMER0_CHANNEL, TC_CMR_WAVSEL_UP_RC | TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK1);

	TC_SetRC(DUETIMER0_TC, DUETIMER0_CHANNEL, 100000L);
	TC_Start(DUETIMER0_TC, DUETIMER0_CHANNEL);

	DUETIMER0_TC->TC_CHANNEL[DUETIMER0_CHANNEL].TC_IER = TC_IER_CPCS;
	DUETIMER0_TC->TC_CHANNEL[DUETIMER0_CHANNEL].TC_IDR = ~TC_IER_CPCS;
	NVIC_EnableIRQ(DUETIMER0_IRQTYPE);
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer0()
{
	NVIC_DisableIRQ(DUETIMER0_IRQTYPE);
	TC_Stop(DUETIMER0_TC, DUETIMER0_CHANNEL);
}

////////////////////////////////////////////////////////

inline void  CHAL::RemoveTimer1() {}

inline void CHAL::StartTimer1OneShot(timer_t delay)
{
	// convert old AVR timer delay value for SAM timers
	delay *= 21;		// 2MhZ to 42MhZ

//	delay /= 2;			// do not know why
	//	uint32_t timer_count = (delay * TIMER1_PRESCALE);

	uint32_t timer_count = delay;

	if(timer_count == 0) timer_count = 1;
	TC_SetRC(DUETIMER1_TC, DUETIMER1_CHANNEL, timer_count);
	TC_Start(DUETIMER1_TC, DUETIMER1_CHANNEL);
}

////////////////////////////////////////////////////////

inline void  CHAL::InitTimer1OneShot(HALEvent evt)
{
	_TimerEvent1 = evt;

	pmc_enable_periph_clk(DUETIMER1_IRQTYPE );
	NVIC_SetPriority(DUETIMER1_IRQTYPE, NVIC_EncodePriority(4, 1, 0));

	TC_Configure(DUETIMER1_TC, DUETIMER1_CHANNEL, TC_CMR_WAVSEL_UP_RC | TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK1);

	TC_SetRC(DUETIMER1_TC, DUETIMER1_CHANNEL, 100000L);
	TC_Start(DUETIMER1_TC, DUETIMER1_CHANNEL);

	DUETIMER1_TC->TC_CHANNEL[DUETIMER1_CHANNEL].TC_IER = TC_IER_CPCS;
	DUETIMER1_TC->TC_CHANNEL[DUETIMER1_CHANNEL].TC_IDR = ~TC_IER_CPCS;
	NVIC_EnableIRQ(DUETIMER1_IRQTYPE); 
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer1()
{
	NVIC_DisableIRQ(DUETIMER1_IRQTYPE);
	TC_Stop(DUETIMER1_TC, DUETIMER1_CHANNEL);
}  

////////////////////////////////////////////////////////

inline void  CHAL::RemoveTimer2() {}

inline void CHAL::StartTimer2OneShot(timer_t delay)
{
	// convert old AVR timer delay value for SAM timers
	delay *= 21;		// 2MhZ to 42MhZ

//	delay /= 2;			// do not know why
						//	uint32_t timer_count = (delay * TIMER1_PRESCALE);

	uint32_t timer_count = delay;

	if (timer_count == 0) timer_count = 1;
	TC_SetRC(DUETIMER2_TC, DUETIMER2_CHANNEL, timer_count);
	TC_Start(DUETIMER2_TC, DUETIMER2_CHANNEL);
}

inline void CHAL::ReStartTimer2OneShot(timer_t delay)
{
	StartTimer2OneShot(delay);
}

////////////////////////////////////////////////////////

inline void  CHAL::InitTimer2OneShot(HALEvent evt)
{
	_TimerEvent2 = evt;

	pmc_enable_periph_clk(DUETIMER2_IRQTYPE);
	NVIC_SetPriority(DUETIMER2_IRQTYPE, NVIC_EncodePriority(0, 0, 0));

	TC_Configure(DUETIMER2_TC, DUETIMER2_CHANNEL, TC_CMR_WAVSEL_UP_RC | TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK1);

	TC_SetRC(DUETIMER2_TC, DUETIMER2_CHANNEL, 100000L);
	TC_Start(DUETIMER2_TC, DUETIMER2_CHANNEL);

	DUETIMER2_TC->TC_CHANNEL[DUETIMER2_CHANNEL].TC_IER = TC_IER_CPCS;
	DUETIMER2_TC->TC_CHANNEL[DUETIMER2_CHANNEL].TC_IDR = ~TC_IER_CPCS;
	NVIC_EnableIRQ(DUETIMER2_IRQTYPE);
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer2()
{
	NVIC_DisableIRQ(DUETIMER2_IRQTYPE);
	TC_Stop(DUETIMER2_TC, DUETIMER2_CHANNEL);
}

////////////////////////////////////////////////////////


#endif
