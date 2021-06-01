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

#define POSITIVE 1

class LiquidCrystal_I2C
{
public:
	LiquidCrystal_I2C(int, int, int) {};
	LiquidCrystal_I2C(int, int, int, int, int, int, int, int) {};
	LiquidCrystal_I2C(int, int, int, int, int, int, int, int, int, int) {};

	void init() {};
	void setBacklightPin(int, int) {};
	void setBacklight(int   ) {};
	void backlight() {};
	void clear() {};
	void begin(int         , int) {};
	void setCursor(int     , int) {};
	void print(const char* ) {};
	void print(char        ) {};
	void print(uint8_t     ) {};
	void print(unsigned int) {};
	void print(int         ) {};
	void print(float       , int = 2) {};
	void print(int16_t     ) {};
	//	void print(uint32_t) {};
	void home() {};
};
