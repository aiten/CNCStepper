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

class CPlotter
{
public:

	CPlotter();

	void Init();
	void Initialized();

	void Idle(unsigned int idleTime);
	void Resume(bool       restorePenDown);
	bool IsResumePenDown() const { return _isPenDownTimeout; }

	void PenDown();
	void PenUp();
	void PenUpNow();

	void DelayPenUp()
	{
		_isDelayPen     = true;
		_isDelayPenDown = false;
	}

	void DelayPenDown()
	{
		_isDelayPen     = true;
		_isDelayPenDown = true;
	}

	void DelayPenNow();    // go

	bool    IsPenDown() { return _isPenDown; }
	uint8_t GetPen() { return _pen; }
	bool    SetPen(uint8_t pen);

private:

	bool _isDelayPen;
	bool _isDelayPenDown;
	bool _isPenDown;
	bool _isPenDownTimeout;

	uint8_t _pen;
	bool    _havePen;

	bool MoveToPenPosition(feedrate_t feedRate, mm1000_t pos);

	bool PenToDepot();
	bool PenFromDepot(uint8_t    pen);
	bool ToPenChangePos(uint8_t  pen);
	bool OffPenChangePos(uint8_t pen);

	mm1000_t ConvertConfigPos(mm1000_t pos, axis_t axis);
};

////////////////////////////////////////////////////////

extern CPlotter Plotter;
