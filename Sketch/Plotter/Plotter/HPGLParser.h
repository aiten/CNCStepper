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

#include <Parser.h>

////////////////////////////////////////////////////////

class CHPGLParser : public CParser
{
private:

	typedef CParser super;

public:

	CHPGLParser(CStreamReader* reader, Stream* output) : CParser(reader, output) { };

	virtual void Parse() override;

	static mm1000_t HPGLToMM1000X(int32_t xx);
	static mm1000_t HPGLToMM1000Y(int32_t yy);

	static void Init()
	{
		super::Init();
		_state.Init();
	}

	struct SState
	{
		bool _HPGLIsAbsolute;
		bool dummy;

		int _HPOffsetX;
		int _HPOffsetY;

		feedrate_t FeedRate;
		feedrate_t FeedRateUp;
		feedrate_t FeedRateDown;

		// Plotter

		void Init()
		{
			_HPGLIsAbsolute = true;

			_HPOffsetX = 0;
			_HPOffsetY = 0;

			SetFeedRates();
		}

		void SetFeedRates()
		{
			FeedRateUp   = CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenUpFeedRate));
			FeedRateUp   = CMotionControlBase::GetInstance()->GetMaxFeedRate(X_AXIS, FeedRateUp);
			FeedRateUp   = -FeedRateUp;
			FeedRateDown = CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenDownFeedRate));
		}
	};

	static SState _state;

private:

	void ReadAndSkipSemicolon();

	void SelectPenCommand();
	void PenVelocityCommand();
	void PenVelocityNormalCommand();

	void IgnoreCommand();
	void InitCommand();
	void PenMoveCommand(uint8_t cmdIdx);
};

////////////////////////////////////////////////////////
