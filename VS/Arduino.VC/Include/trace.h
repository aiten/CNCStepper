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

#include <crtdbg.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
#define TRACEMAXSTRING 1024

inline void Trace(const char* format, ...)
{
	char szBuffer[TRACEMAXSTRING];
	va_list args;
	va_start(args, format);
	int nBuf;
	nBuf = _vsnprintf_s(szBuffer,
		TRACEMAXSTRING,
		format,
		args);
	va_end(args);

	_RPT0(_CRT_WARN, szBuffer);
}
#define TraceEx char szBuffer[TRACEMAXSTRING]; _snprintf_s(szBuffer,TRACEMAXSTRING,"%s(%d): ", \
	&strrchr(__FILE__,'\\')[1],__LINE__); \
	_RPT0(_CRT_WARN,szBuffer); \
	Trace 

#else
inline void Trace(const char* , ...) {};
inline void TraceEx(const char* , ...) {};
#endif


