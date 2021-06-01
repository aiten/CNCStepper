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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "Lcd.h"
#include "Control.h"

////////////////////////////////////////////////////////////

template <> CLcd* CSingleton<CLcd>::_instance = nullptr;

////////////////////////////////////////////////////////////

void CLcd::Init()
{
	_nextdrawtime = Splash() + millis();
	_splash       = true;

	DrawRequest(CLcd::DrawForceAll);		//first draw doesn't return fast! call at init time
}

////////////////////////////////////////////////////////////

void CLcd::Poll()
{
	DrawRequest(CLcd::DrawAll);
}

////////////////////////////////////////////////////////////

void CLcd::Invalidate()
{
	_invalidate = true;
}

////////////////////////////////////////////////////////////

void CLcd::TimerInterrupt() {}

////////////////////////////////////////////////////////////

void CLcd::Command(char* /* buffer */) {}

////////////////////////////////////////////////////////////

void CLcd::DrawRequest(EDrawType draw)
{
	if (_splash)
	{
		if (_nextdrawtime > millis()) return;
		// splash timeout;
		_splash = false;
		Draw(DrawFirst);
	}

	if (_invalidate || draw == DrawForceAll || _nextdrawtime < millis())
	{
		_invalidate   = false;
		_nextdrawtime = Draw(draw) + millis();
	}
}

////////////////////////////////////////////////////////////

uint8_t CLcd::InitPostCommand(EnumAsByte(EGCodeSyntaxType) /* syntaxType */, char* cmd)
{
	cmd[0] = 0;
	return 0;
}

////////////////////////////////////////////////////////////

bool CLcd::PostCommand(EnumAsByte(EGCodeSyntaxType) syntaxType, FLSTR cmd, Stream* output)
{
	char buffer[32];

	auto    cmd1       = (const char*)cmd;
	uint8_t idx        = InitPostCommand(syntaxType, buffer);
	uint8_t idxprogmem = 0;

	for (; idx < sizeof(buffer); idx++, idxprogmem++)
	{
		buffer[idx] = pgm_read_byte(&cmd1[idxprogmem]);

		if (buffer[idx] == 0)
		{
			return PostCommand(buffer, output);
		}
	}

	return false;
}

////////////////////////////////////////////////////////////

bool CLcd::PostCommand(char* cmd, Stream* output)
{
	if (!CControl::GetInstance()->PostCommand(cmd, output))
	{
		ErrorBeep();
		return false;
	}
	return true;
}
