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

#include <stdlib.h>
#include <string.h>

#include <Arduino.h>
#include <ctype.h>

#include "HAL.h"

////////////////////////////////////////////////////////

#if defined(ESP32)

#include "esp_dsp.h"

const uint32_t CHAL::_timeout_cycles = microsecondsToClockCycles(1);

hw_timer_t* CHAL::_hwTimer[2] = {0};

void IRAM_ATTR CHAL::OnTimer0()
{
	CHAL::_TimerEvent0();
}

void IRAM_ATTR CHAL::OnTimer1()
{
	CHAL::_TimerEvent1();
}

//static void IgnoreIrq() {}

////////////////////////////////////////////////////////

const uint8_t CHAL::_flashStorage[EEPROM_SIZE] = { };
uint8_t CHAL::_flashBuffer[EEPROM_SIZE];

void CHAL::FlashWriteWords(uint32_t *flash_ptr, const uint32_t *src, uint32_t n_words)
{
}

void CHAL::FlashErase(void *flash_ptr, uint32_t size)
{
}

void CHAL::FlashEraseRow(void *flash_ptr)
{
}

void CHAL::FlashRead(const void *flash_ptr, void *data, uint32_t size)
{
	memcpy(data, flash_ptr, size);
}


////////////////////////////////////////////////////////

#endif
