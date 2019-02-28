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

////////////////////////////////////////////////////////

#include <StepperLib.h>
#include "CNCLibTypes.h"
#include "Beep.h"

////////////////////////////////////////////////////////

class CControl;

////////////////////////////////////////////////////////

class CLcd : public CSingleton<CLcd>
{
	friend CControl;

public:

	CLcd() { }

	enum EDrawType
	{
		DrawFirst,		// draw after splash
		DrawForceAll,	// draw now
		DrawAll			// draw with timeout
	};

	virtual void Init();
	virtual void DrawRequest(EDrawType draw);

	void Invalidate(); // draw with next timeout

	////////////////////////////////////////////////////////////

	static void InvalidateLcd()
	{
#ifdef _USE_LCD

		if (CLcd::GetInstance())
			CLcd::GetInstance()->Invalidate();
#endif
	}

protected:

	virtual void Poll();
	virtual void TimerInterrupt();

	virtual void Command(char* cmd);

	virtual uint32_t Draw(EDrawType draw) = 0; // return => timeout for next draw

	virtual uint32_t Splash() = 0; // return time to display

	bool IsSplash() const { return _splash; };

public:

	virtual void Beep(const SPlayTone*, bool fromProgmem) =0;

	void OKBeep() { Beep(SPlayTone::PlayOK, true); }
	void ErrorBeep() { Beep(SPlayTone::PlayError, true); }

	bool PostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, FLSTR cmd, Stream* output = nullptr);
	bool PostCommand(char*                        cmd, Stream*      output              = nullptr);

	virtual uint8_t InitPostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, char* cmd);

private:

	uint32_t _nextdrawtime = 0;

	bool _splash     = false;
	bool _invalidate = false;

public:

	void Diagnostic(FLSTR diag) { _diagnostics = diag; }

	bool  IsDiagnostic() const { return _diagnostics != nullptr; };
	FLSTR GetDiagnostic() const { return _diagnostics; }
	void  ClearDiagnostic() { _diagnostics = nullptr; }

private:

	FLSTR _diagnostics = nullptr;
};

////////////////////////////////////////////////////////
