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

#pragma comment (lib, "StepperSystem.lib")

#include <stdio.h>
#include <ctype.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <conio.h>
#include <io.h>
#include <windows.h>
#include "trace.h"
#include <assert.h>

#include <functional>

#define OUTPUT 1
#define INPUT_PULLUP 1
#define INPUT 2
#define CS12 1
#define CS11 1
#define TOIE1 1
#define LOW 0
#define HIGH 1

#define TOIE0 0
#define OCIE0A 1
#define OCIE0B 2
#define OCF0B   2
#define OCF0A   1
#define TOV0    0

#define TOIE1 1
#define TOV1 0

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define INTERNAL 3
#define DEFAULT 1
#define EXTERNAL 0

#define NOT_A_PIN 0
#define NOT_A_PORT 0

#define NOT_AN_INTERRUPT (-1)


#define ISR(a) void a(void)

#define __FlashStringHelper char

//#define max(a,b) ((a)>=(b)?(a):(b))
//#define min(a,b) ((a)<=(b)?(a):(b))

#define strcpy_P(a,b) strcpy(a,b)
#define strcat_P(a,b) strcat(a,b)
#define strcmp_P(a,b) strcmp(a,b)
#define strcasecmp_P(a,b) _stricmp(a,b)

#define memcpy_P(a,b,c) memcpy(a,b,c)

#define eeprom_read_block(a,b,c) memcpy(a,b,c)
#define eeprom_write_block(a,b,c) memcpy(b,a,c)

inline void     eeprom_write_dword(uint32_t*      __p, uint32_t __value) { *__p = __value; }
inline uint32_t eeprom_read_dword(const uint32_t* __p) { return *__p; }
inline uint8_t  eeprom_read_byte(const uint8_t*   __p) { return *__p; }

#define __FlashStringHelper char
#define F(a) a
#define PROGMEM 
inline char         pgm_read_byte(const char* p) { return *p; }
typedef const char* PGM_P;


inline void attachInterrupt(uint8_t, void (*)(), int /* mode */) {};

inline void detachInterrupt(uint8_t) {};

inline uint8_t digitalPinToInterrupt(uint8_t p) { return ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT)); }

typedef unsigned char uint8_t;
typedef signed char   int8_t;

inline int random(int, int) { return 0;  }
inline void analogWrite(int16_t, int) {};
inline int  analogRead(int16_t ) { return 0; };

inline void    digitalWrite(int16_t, int16_t) {};
extern uint8_t digitalRead(int16_t pin);

inline void pinMode(int16_t, int16_t) {};

#define DIGITALREADNOVALUE 255
extern std::function<uint8_t(int16_t)> digitalReadEvent;

extern uint8_t digitalReadFromFile(int16_t pin);

#define LED_BUILTIN (13)

#define PIN_A0   (14)
#define PIN_A1   (15)
#define PIN_A2   (16)
#define PIN_A3   (17)
#define PIN_A4   (18)
#define PIN_A5   (19)
#define PIN_A6   (20)
#define PIN_A7   (21)

static uint8_t A0 = PIN_A0;
static uint8_t A1 = PIN_A1;
static uint8_t A2 = PIN_A2;

static uint8_t  PORTA;
static uint8_t  PORTB;
static uint8_t  PORTC;
static uint8_t  PORTD;
static uint8_t  PORTE;
static uint8_t  PORTF;
static uint8_t  PORTK;
static uint8_t  PORTL;
static uint8_t  DDRL;
static uint8_t  DDRD;
static uint8_t  DDRB;
static uint8_t  TCCR0A;
static uint8_t  TCCR0B;
static uint16_t TCNT0;
static uint8_t  TIMSK0;
static uint16_t TIFR0;
static uint16_t OCR0B;
static uint8_t  TCCR1A;
static uint8_t  TCCR1B;
static uint16_t TCNT1;
static uint8_t  TIMSK1;
static uint16_t TIFR1;

static uint8_t PINA;
static uint8_t PINA0;
static uint8_t PINA1;
static uint8_t PINA2;
static uint8_t PINA3;
static uint8_t PINA4;
static uint8_t PINA5;
static uint8_t PINA6;
static uint8_t PINA7;

static uint8_t PINB;
static uint8_t PINB0;
static uint8_t PINB1;
static uint8_t PINB2;
static uint8_t PINB3;
static uint8_t PINB4;
static uint8_t PINB5;
static uint8_t PINB6;
static uint8_t PINB7;

static uint8_t PINC;
static uint8_t PINC0;
static uint8_t PINC1;
static uint8_t PINC2;
static uint8_t PINC3;
static uint8_t PINC4;
static uint8_t PINC5;
static uint8_t PINC6;
static uint8_t PINC7;

static uint8_t PIND;
static uint8_t PIND0;
static uint8_t PIND1;
static uint8_t PIND2;
static uint8_t PIND3;
static uint8_t PIND4;
static uint8_t PIND5;
static uint8_t PIND6;
static uint8_t PIND7;

static uint8_t PINE;
static uint8_t PINE0;
static uint8_t PINE1;
static uint8_t PINE2;
static uint8_t PINE3;
static uint8_t PINE4;
static uint8_t PINE5;
static uint8_t PINE6;
static uint8_t PINE7;

static uint8_t PINF;
static uint8_t PINF0;
static uint8_t PINF1;
static uint8_t PINF2;
static uint8_t PINF6;
static uint8_t PINF7;

static uint8_t PINJ;
static uint8_t PINJ0;
static uint8_t PINJ1;
static uint8_t PINJ2;
static uint8_t PINJ3;
static uint8_t PINJ4;
static uint8_t PINJ5;
static uint8_t PINJ6;
static uint8_t PINJ7;

static uint8_t PINK;
static uint8_t PINK0;

static uint8_t PINL;
static uint8_t PINL1;
static uint8_t PINL3;


static uint8_t SREG;

inline uint32_t    pgm_read_dword(const void* p) { return *(uint32_t*)(p); }
inline uint16_t    pgm_read_word(const void*  p) { return *(uint16_t*)(p); }
inline uint8_t     pgm_read_byte(const void*  p) { return *(uint8_t*)(p); }
inline const void* pgm_read_ptr(const void*   p) { return *((void **)(p)); }

//extern unsigned int GetTickCount();
#pragma warning(suppress: 28159)
inline uint32_t millis() { return GetTickCount(); }

//extern void Sleep(unsigned int ms);
inline void delay(uint32_t ms) { Sleep(ms); }

#define STDIO 0
#define HEX 16

class Stream
{
public:

	Stream()
	{
		_istty = _isatty(STDIO) != 0;
	}

	virtual ~Stream() = default;

	void SetIdle(void (*pIdle)()) { _pIdle = pIdle; }

	void print(char         c) { printf("%c", c); };
	void print(unsigned int ui) { printf("%u", ui); };
	void print(int          i) { printf("%i", i); };
	//	void print(int32_t          l) { printf("%li", l); };
	//	void print(uint32_t ul) { printf("%lu", ul); };

	void print(uint32_t ul, uint8_t base)
	{
		if (base == 10) printf("%u", ul);
		if (base == 16) printf("%x", ul);
	}

	void print(const char* s) { printf("%s", s); };
	void print(float       f) { printf("%f", f); };

	void println() { printf("\n"); };
	void println(unsigned int ui) { printf("%u\n", ui); };
	void println(char         c) { printf("%c\n", c); };
	void println(int          i) { printf("%i\n", i); };
	//	void println(int32_t          l) { printf("%li\n", l); };
	//	void println(uint32_t ul) { printf("%lu\n", ul); };

	#define DEC 10
	
	void println(uint32_t ul, uint8_t base)
	{
		print(ul, base);
		println();
	};
	void println(const char* s) { printf("%s\n", s); };
	void println(float       f) { printf("%f\n", f); };

	void begin(int) { };

	virtual int available()
	{
		if (_last)
			return 1;

		if (!_istty)
		{
			if (feof(stdin) != 0)
			{
				_istty = true;
				return 0;
			}
		}
		if (!_istty || _kbhit())
			return 1;

		if (_pIdle) _pIdle();
		return 0;
	}

	virtual char read()
	{
		char ch = _last;
		if (ch)
		{
			_last = 0;
		}
		else
		{
			if (_istty)
			{
				ch = char(_getwch());
				if (ch == '\r')
					_last = '\n';
			}
			else
			{
				ch = char(_fgetchar());
			}
		}

		_putch(ch);
		return ch;
	}

	size_t readBytesUntil(char /* character */, char* /* buffer */, int /* length */)
	{
		return 0;
	}

private:

	void (*_pIdle)() = nullptr;
	char   _last     = 0;
	bool   _istty;
};

class HardwareSerial : public Stream {};

class CSerial : public HardwareSerial {};


extern CSerial Serial;

#define __attribute__
