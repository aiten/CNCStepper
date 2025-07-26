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

#if defined(__SAMD21G18A__)

//#include <itoa.h>

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

#ifndef interruptsStatus
#define interruptsStatus() __interruptsStatus()
static inline unsigned char __interruptsStatus(void) __attribute__((always_inline, unused));
static inline unsigned char __interruptsStatus(void)
{
	// See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/CHDBIBGJ.html
	return (__get_PRIMASK() ? 0 : 1);
}
#endif

inline irqflags_t CHAL::GetSREG()			{ return interruptsStatus(); }
inline void CHAL::SetSREG(irqflags_t a)		{ if (a != GetSREG()) { if (a) EnableInterrupts(); else DisableInterrupts(); } }

#define IRQTYPE I2S_IRQn
//#define IRQTYPE TC3_IRQn

inline void CHAL::BackgroundRequest()			{ NVIC_SetPendingIRQ(IRQTYPE); }
inline void CHAL::InitBackground(HALEvent evt)	{ NVIC_EnableIRQ(IRQTYPE);  NVIC_SetPriority(IRQTYPE, BACKGROUND_INTERRUPTPRIORITY); _BackgroundEvent = evt; }

#define HALFastdigitalRead(a)	CHAL::digitalRead(a)
#define HALFastdigitalWrite(a,b) CHAL::digitalWrite(a,b)
#define HALFastdigitalWriteNC(a,b) CHAL::digitalWrite(a,b)

inline uint8_t CHAL::digitalRead(pin_t pin)
{
	return ::digitalRead(pin);
}

inline void CHAL::digitalWrite(pin_t pin, uint8_t val)
{
	::digitalWrite(pin,(PinStatus) val);
}

inline void CHAL::pinMode(pin_t pin, uint8_t mode)
{ 
	::pinMode(pin,(PinMode) mode); 
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
	::attachInterrupt(digitalPinToInterrupt(pin), userFunc, (PinStatus) mode);
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

inline void CHAL::DelayNanoseconds(unsigned int) {}

inline void CHAL::DelayMicroseconds(unsigned int usec)
{
	::delayMicroseconds(usec);
}

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

inline void WaitForSyncGCLK()
{
	while (GCLK->STATUS.bit.SYNCBUSY == 1);
}

inline void InitGClk(int clkgen,int dest,int clockdiv)
{
	// Clock should be AVR Compatible 2Mhz => 48Mhz/24

	REG_GCLK_GENCTRL =    // GCLK_GENCTRL_DIVSEL |
		GCLK_GENCTRL_IDC |
		GCLK_GENCTRL_GENEN |
		GCLK_GENCTRL_SRC_DFLL48M |
		GCLK_CLKCTRL_GEN_GCLK1 |
		GCLK_GENCTRL_ID(clkgen);
	WaitForSyncGCLK();

	if (clockdiv > 1)
	{
		REG_GCLK_GENDIV = GCLK_GENDIV_DIV(clockdiv) |	// Divide the 48MHz clock source by divisor x: 48MHz/x=xxMHz
			GCLK_GENDIV_ID(clkgen);						// Select Generic Clock (GCLK) 
		WaitForSyncGCLK();
	}

	// Enable clock for TC
	REG_GCLK_CLKCTRL = (uint16_t)(GCLK_CLKCTRL_CLKEN |
		GCLK_CLKCTRL_GEN(clkgen) |
		GCLK_CLKCTRL_GEN_GCLK1 |
		GCLK_CLKCTRL_ID(dest));
	WaitForSyncGCLK();
}

////////////////////////////////////////////////////////

inline void WaitForSyncTC(TcCount16* TC)
{
	while (TC->STATUS.bit.SYNCBUSY == 1);
}

////////////////////////////////////////////////////////

inline TcCount16* GetTimer0Struct() { return (TcCount16*)TC5; }

inline void  CHAL::RemoveTimer0() {}

inline void CHAL::StartTimer0(timer_t delay)
{
	// do not use 32bit

	uint16_t timer_count = (uint16_t)delay;
	if (timer_count == 0) timer_count = 1;

	TcCount16* TC = GetTimer0Struct();

	TC->CTRLBSET.bit.CMD = TC_CTRLBCLR_CMD_RETRIGGER_Val;
	TC->CC[0].reg = timer_count;

	TC->CTRLA.reg |= TC_CTRLA_ENABLE;

	// don't care about wait (we are in ISR)
	// WaitForSyncTC(TC);
}

inline void  CHAL::InitTimer0(HALEvent evt)
{
	_TimerEvent0 = evt;

	InitGClk(TIMER0_CLKGEN, GCM_TC4_TC5, TIMERCLOCKDIV);

	TcCount16* TC = GetTimer0Struct();

	TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;			// Disable
	WaitForSyncTC(TC);

	TC->CTRLA.reg = TC_CTRLA_MODE_COUNT16 |		// Set Timer counter Mode to 32 bits
		TC_CTRLA_WAVEGEN_MFRQ |					// use TOP
		TC_CTRLA_PRESCALER_DIV1024;				// Set perscaler
	WaitForSyncTC(TC);

	TC->CC[0].reg = 100;
	WaitForSyncTC(TC);

	// Interrupts
	TC->INTENSET.reg = 0;                     // disable all interrupts
	TC->INTENSET.bit.OVF = 1;                 // enable overflow

	NVIC_DisableIRQ(TC5_IRQn);				  // Configure interrupt request
	NVIC_ClearPendingIRQ(TC5_IRQn);
	NVIC_SetPriority(TC5_IRQn, TIMER0_INTERRUPTPRIORITY);
	NVIC_EnableIRQ(TC5_IRQn);
}

inline void CHAL::StopTimer0()
{
	//NVIC_DisableIRQ(ZEROTIMER3_IRQTYPE);

	//TODO...
}

////////////////////////////////////////////////////////

inline TcCount16* GetTimer1Struct() { return (TcCount16*)TC4; }

inline void  CHAL::RemoveTimer1() {}

inline void CHAL::StartTimer1OneShot(timer_t delay)
{
	// do not use 32bit => 2Mhz Timer as AVR

	uint16_t timer_count = (uint16_t) delay;
	if(timer_count == 0) timer_count = 1;

	TcCount16* TC = GetTimer1Struct();

	TC->CTRLBSET.bit.CMD = TC_CTRLBCLR_CMD_RETRIGGER_Val;
	TC->CC[0].reg = timer_count;
	
	TC->CTRLA.reg |= TC_CTRLA_ENABLE;

	// don't care about wait (we are in ISR)
	WaitForSyncTC(TC);

//	if (timer_count != 64516)
//	{
//		StepperSerial.print("TCS:");
//		StepperSerial.println(timer_count);
//	}
}

////////////////////////////////////////////////////////

inline void  CHAL::InitTimer1OneShot(HALEvent evt)
{
	InitGClk(TIMER1_CLKGEN, GCM_TC4_TC5, TIMERCLOCKDIV);

	_TimerEvent1 = evt;

	TcCount16* TC = GetTimer1Struct();

	TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;			// Disable
	WaitForSyncTC(TC);

	TC->CTRLA.reg = TC_CTRLA_MODE_COUNT16 |		// Set Timer counter Mode to 32 bits
		TC_CTRLA_WAVEGEN_MFRQ |					// use TOP
		TC_CTRLA_PRESCALER_DIV8;				// Set perscaler
	WaitForSyncTC(TC);

	TC->CTRLBSET.bit.ONESHOT = 1;
	WaitForSyncTC(TC);

	TC->CC[0].reg = 100;
	WaitForSyncTC(TC);

	TC->CTRLBSET.bit.CMD = TC_CTRLBCLR_CMD_STOP_Val;
	WaitForSyncTC(TC);

		// Interrupts
	TC->INTENSET.reg = 0;                     // disable all interrupts
	TC->INTENSET.bit.OVF = 1;                 // enable overflow

	NVIC_DisableIRQ(TC4_IRQn);				  // Configure interrupt request
	NVIC_ClearPendingIRQ(TC4_IRQn);
	NVIC_SetPriority(TC4_IRQn, TIMER1_INTERRUPTPRIORITY);
	NVIC_EnableIRQ(TC4_IRQn);

//	// Enable TC
//	TC->CTRLA.reg |= TC_CTRLA_ENABLE;
//	WaitForSyncTC(TC);
}

////////////////////////////////////////////////////////

inline void CHAL::StopTimer1()
{
	NVIC_DisableIRQ(TC4_IRQn);

	TcCount16* TC = (TcCount16*)TC4;

	TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;			// Disable
	WaitForSyncTC(TC);
}

////////////////////////////////////////////////////////

#endif
