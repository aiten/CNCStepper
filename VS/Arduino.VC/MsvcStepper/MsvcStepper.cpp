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

#define _CRT_SECURE_NO_WARNINGS

#include <Stdio.h>
#include <StdLib.h>

#include <Arduino.h>
#include <avr/interrupt.h>
#include "MsvcStepper.h"

////////////////////////////////////////////////////////////

CMsvcStepper::CMsvcStepper()
{
	_isReferenceMove  = false;
	DelayOptimization = true;;
	SplitFile         = true;
	UseSpeedSign      = false;
	_TimerEvents      = nullptr;
	CacheSize         = _STORETIMEVALUES;
	_oldCacheSize     = -1;
}

////////////////////////////////////////////////////////////

void CMsvcStepper::Init()
{
	__super::Init();
}

////////////////////////////////////////////////////////////

void CMsvcStepper::OnWait(EnumAsByte(EWaitType) wait)
{
	__super::OnWait(wait);
	DoISR();

	if (wait == MovementQueueFull && CanQueueMovement()) // doISR has finished move
	{
		SMovement& mv = _movements._queue.Head();
		while (!mv.IsFinished())
		{
			DoISR();
			if (mv.IsDownMove())
			{
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////

void CMsvcStepper::DoISR()
{
	HandleInterrupt();
}

////////////////////////////////////////////////////////////

void CMsvcStepper::HandleIdle()
{
	// if (IsBusy()) => timer is running on idle
	{
		DoISR();
	}

	static uint32_t lastTimerInterrupt = 0;

	if (lastTimerInterrupt + 100 < millis())
	{
		lastTimerInterrupt = millis();
		if (CHAL::_TimerEvent0)
		{
			CHAL::_TimerEvent0();
		}
	}
}

////////////////////////////////////////////////////////////

void CMsvcStepper::OnStart()
{
	_refMoveStart = 0;
	__super::OnStart();
}

////////////////////////////////////////////////////////////

void CMsvcStepper::OnIdle(uint32_t idleTime)
{
	__super::OnIdle(idleTime);
}

////////////////////////////////////////////////////////////

uint8_t CMsvcStepper::GetReferenceValue(uint8_t referenceId)
{
	uint8_t refHitValue = _pod._referenceHitValue[referenceId];
	uint8_t refOffValue = _pod._referenceHitValue[referenceId] == LOW ? HIGH : LOW;

	if (!_isReferenceMove || referenceId != _isReferenceId)
	{
		return refOffValue;
	}

	_referenceMoveSteps++;

	return (_referenceMoveSteps / 16) % 2 == 0 ? refHitValue : refOffValue;
}

////////////////////////////////////////////////////////////

bool CMsvcStepper::MoveReference(axis_t axis, uint8_t referenceId, bool toMin, steprate_t vMax, sdist_t maxDist, sdist_t distToRef, sdist_t distIfRefIsOn)
{
	_referenceMoveSteps = 15;
	_isReferenceMove    = true;
	_isReferenceId      = referenceId;
	bool ret            = __super::MoveReference(axis, referenceId, toMin, vMax, maxDist, distToRef, distIfRefIsOn);
	_isReferenceMove    = false;
	return ret;
}

////////////////////////////////////////////////////////////

void CMsvcStepper::StartTimer(timer_t timerB)
{
	timerB += TIMEROVERHEAD;
	_TimerEvents[_eventIdx].TimerValues = timerB;
	__super::StartTimer(timerB);
};

////////////////////////////////////////////////////////////

void CMsvcStepper::SetIdleTimer()
{
	__super::SetIdleTimer();
}

////////////////////////////////////////////////////////////

void CMsvcStepper::StepRequest(bool isr)
{
	_refMoveStart++;
	__super::StepRequest(isr);
}

////////////////////////////////////////////////////////////

void CMsvcStepper::StepBegin(const SStepBuffer* stepBuffer)
{
	_TimerEvents[_eventIdx].Steps = stepBuffer->_steps;
	_TimerEvents[_eventIdx].Count = stepBuffer->_count;

	int multiplier = stepBuffer->DirStepCount;

	for (int i = 0; i < NUM_AXIS; i++)
	{
		_TimerEvents[_eventIdx].Axis[i].MoveAxis   = 0;
		_TimerEvents[_eventIdx].Axis[i].Distance   = stepBuffer->_distance[i];
		_TimerEvents[_eventIdx].Axis[i].Multiplier = multiplier % 8;

		multiplier = multiplier / 16;
	}
	strcpy_s(_TimerEvents[_eventIdx].MSCInfo, stepBuffer->_spMSCInfo);
}

////////////////////////////////////////////////////////////
/*
void  CStepper::Step(axis_t axis , bool  directionUp , uint8_t count )
{
	// CStepper:: is abstract => for call of base class in derived class!
}
*/
////////////////////////////////////////////////////////////

void CMsvcStepper::Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool /* isSameDirection */)
{
	for (axis_t axis = 0; axis < NUM_AXIS; axis++)
	{
		_TimerEvents[_eventIdx].Axis[axis].MoveAxis = (directionUp & ((1 << axis))) != 0 ? steps[axis] : -steps[axis];
	}

	_TotalSteps++;
	_eventIdx++;

	if (_eventIdx < CacheSize) { }
	else
	{
		WriteTestResults(_fileName);
		InitCache();
	}
}

////////////////////////////////////////////////////////////

void CMsvcStepper::OptimizeMovementQueue(bool force)
{
	if (!DelayOptimization || force || _movements._queue.IsFull())
	{
		__super::OptimizeMovementQueue(force);
	}
}

////////////////////////////////////////////////////////////

void CMsvcStepper::InitTest(const char* fileName)
{
	if (_oldCacheSize != CacheSize)
	{
		delete[] _TimerEvents;
		_TimerEvents = nullptr;
	}
	if (_TimerEvents == nullptr)
	{
		_TimerEvents  = new STimerEvent[CacheSize];
		_oldCacheSize = CacheSize;
	}
	_flushCount = 0;
	_fileName   = fileName;

	Init();

	SetDefaultMaxSpeed(5000, 100, 150);
	ContinueMove();

	_TotalSteps = 1;
	_exportIdx  = 1;

	for (axis_t x = 0; x < NUM_AXIS_MVC; x++)
	{
		if (x < NUM_AXIS)
		{
			SetJerkSpeed(x, 500);
			SetPosition(x, 0);
		}
		_sumTime[x]  = 0;
		_count[x]    = 0;
		_sumTime[x]  = 0;
		_total[x]    = 0;
		_speed[x][0] = 0;
	}
	_totalTime = 0;
	_lastTimer = 0;

	MSCInfo = "";

	SetWaitConditional(false);

	InitCache();
}

////////////////////////////////////////////////////////////

void CMsvcStepper::EndTest(const char* fileName)
{
	_fileName = fileName ? fileName : _fileName;

	OptimizeMovementQueue(true);
	WaitBusy();

	WriteTestResults(_fileName);
}

////////////////////////////////////////////////////////////

void CMsvcStepper::WriteTestResults(const char* fileName)
{
	bool append           = (_flushCount++) != 0;
	char fname[_MAX_PATH] = { 0 };
	fname[0]              = 0;

	strcat(fname, fileName);

	if (append)
	{
		if (SplitFile)
		{
			char* dot = strrchr(fname, '.');
			char  ext[16];
			strcpy(ext, dot);
			*(dot++) = '#';
			_itoa(_flushCount, dot, 10);
			strcat(dot, ext);
			append = false;
		}
	}

	FILE* f = fopen(fname, append ? "at" : "wt");

	int timerconstant = 65536 * 32;

	for (int i = 0; i < _eventIdx; i++)
	{
		int outtotaltime = int(_totalTime / 1000);
		int timer        = _TimerEvents[i].TimerValues;
		if (timer == 0)
		{
			timer = _lastTimer;
		}
		else
		{
			int stepidx;
			for (stepidx = 1; i + stepidx < _eventIdx; stepidx++)
			{
				if (_TimerEvents[i + stepidx].TimerValues)
				{
					break;
				}
			}
			_lastTimer = timer / stepidx;
			timer      = _lastTimer;
		}

		for (int x = 0; x < NUM_AXIS; x++)
		{
			int outspeed = 10 * (timerconstant / timer) * _TimerEvents[i].Axis[x].Multiplier;
			_total[x] += _TimerEvents[i].Axis[x].MoveAxis;
			_sumTime[x] += outspeed;
			_count[x] += 1;
			if (_TimerEvents[i].Axis[x].MoveAxis != 0)
			{
				//				speed[j] = sumtime[j] / count[j];
				//				sprintf(speed[j],"%i",outspeed / count[j]);
				int speed = int(__int64(outspeed) * __int64(_TimerEvents[i].Axis[x].Distance) / __int64(_TimerEvents[i].Steps));
				if (UseSpeedSign && _TimerEvents[i].Axis[x].MoveAxis < 0)
				{
					speed = -speed;
				}
				sprintf(_speed[x], "%i", speed);
				_count[x]   = 0;
				_sumTime[x] = 0;
			}
			else
			{
				_speed[x][0] = 0;
			}
		}

		int sysspeed = 10 * (timerconstant / timer) * _TimerEvents[i].Count;

		fprintf(f, "%i;%i;%i;%i;%s;%s;%s;%s;%s;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%s\n",
		        _exportIdx++,
		        _TimerEvents[i].TimerValues,
		        outtotaltime,
		        sysspeed,
		        _speed[0],
		        _speed[1],
		        _speed[2],
		        _speed[3],
		        _speed[4],
		        _TimerEvents[i].Axis[0].MoveAxis,
		        _TimerEvents[i].Axis[1].MoveAxis,
		        _TimerEvents[i].Axis[2].MoveAxis,
		        _TimerEvents[i].Axis[3].MoveAxis,
		        _TimerEvents[i].Axis[4].MoveAxis,
		        _total[0],
		        _total[1],
		        _total[2],
		        _total[3],
		        _total[4],
		        _TimerEvents[i].MSCInfo);
		_totalTime += timer;
	}
	fclose(f);
}
