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

#include <stdlib.h>
#include <string.h>

#include <Arduino.h>
#include <ctype.h>

#include "HAL.h"

////////////////////////////////////////////////////////

#if defined(__SAMD21G18A__)

static void IgnoreIrq() {}

void TC5_Handler()
{
	TcCount16* TC = GetTimer0Struct();

	if (TC->INTFLAG.bit.OVF == 1)                    // A overflow caused the interrupt
	{
		TC->INTFLAG.bit.OVF = 1;                     // writing a one clears the flag ovf flag
		WaitForSyncTC(TC);
	}

	CHAL::_TimerEvent0();
}


void TC4_Handler()
{
	TcCount16* TC = GetTimer1Struct();

	if (TC->INTFLAG.bit.OVF == 1)                    // A overflow caused the interrupt
	{
		TC->INTFLAG.bit.OVF = 1;                     // writing a one clears the flag ovf flag
		WaitForSyncTC(TC);
	}
	
//	StepperSerial.println("TC");
	CHAL::_TimerEvent1();
}

void I2S_Handler()
{
	CHAL::_BackgroundEvent();
}

CHAL::HALEvent CHAL::_BackgroundEvent = IgnoreIrq;

////////////////////////////////////////////////////////

const uint8_t CHAL::_flashStorage[EEPROM_SIZE] = { };
uint8_t CHAL::_flashBuffer[EEPROM_SIZE];

#define WaitReady()   while (NVMCTRL->INTFLAG.bit.READY == 0) {}

void CHAL::FlashWriteWords(uint32_t *flash_ptr, const uint32_t *src, uint32_t n_words)
{
	// Set automatic page write
	NVMCTRL->CTRLB.bit.MANW = 0;

	while (n_words > 0) 
	{
		uint32_t len = min(FLASH_PAGE_SIZE >> 2, n_words);
		n_words -= len;

		// Execute "PBC" Page Buffer Clear
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
		WaitReady();

		while (len--)
		{
			*flash_ptr++ = *src++;
		}

		// Execute "WP" Write Page
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
		WaitReady();
	}
}

void CHAL::FlashErase(void *flash_ptr, uint32_t size)
{
	uint32_t ROW_SIZE = FLASH_PAGE_SIZE * sizeof(uint32_t);
	uint8_t *ptr = (uint8_t *)flash_ptr;
	int32_t isize = size;

	while (isize > 0)
	{
		FlashEraseRow(ptr);
		ptr += ROW_SIZE;
		isize -= ROW_SIZE;
	}
}

void CHAL::FlashEraseRow(void *flash_ptr)
{
	NVMCTRL->ADDR.reg = ((uint32_t)flash_ptr) / 2;
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
	WaitReady();
}

void CHAL::FlashRead(const void *flash_ptr, void *data, uint32_t size)
{
	memcpy(data, flash_ptr, size);
}


////////////////////////////////////////////////////////

#endif
