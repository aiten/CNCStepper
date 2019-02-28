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

#include "U8glibcommon.h"

class U8G2 : public U8GLIBCommon
{
public:
	void setCursor(int x, int y)
	{
		setPrintPos(x, y);
	};
};

class U8G2_ST7920_128X64_1_SW_SPI : public U8G2
{
public:
	U8G2_ST7920_128X64_1_SW_SPI(void*, int, int, int) {};
};

class U8G2_ST7920_128X64_F_SW_SPI : public U8G2
{
public:
	U8G2_ST7920_128X64_F_SW_SPI(void*, int, int, int) {};
};


static void*          U8G2_R0;
static const uint8_t* u8g_font_unifont;
static const uint8_t* u8g_font_unifontr;
static const uint8_t* u8g_font_6x12;
static const uint8_t* u8g_font_6x10;
