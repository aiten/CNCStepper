////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <memory.h>

#include "..\..\..\Sketch\libraries\StepperLib\src\StepperLib.h"
#include "..\..\..\Sketch\libraries\CNCLib\src\MessageCNCLib.h"
#include "..\..\..\Sketch\libraries\CNCLib\src\GCodeParserBase.h"
#include "..\..\..\Sketch\libraries\CNCLib\src\DecimalAsInt.h"

#define _STORETIMEVALUES	100000
#define NUM_AXIS_MVC		5

class CMsvcStepper : public CStepper
{
public:

	CMsvcStepper();

	virtual void OnIdle(uint32_t idleTime) override;
	virtual void OnStart() override;
	virtual void OnWait(EnumAsByte(EWaitType) wait) override;

	virtual void Init() override;

	virtual uint8_t GetReferenceValue(uint8_t referenceId) override;
	virtual bool    IsAnyReference() override { return GetReferenceValue(0) == _pod._referenceHitValue[0]; };

	void MoveRel3(sdist_t dX, sdist_t dY, sdist_t dZ, steprate_t vMax = 0) { MoveRelEx(vMax, X_AXIS, dX, Y_AXIS, dY, Z_AXIS, dZ, -1); }
	void MoveAbs3(udist_t X, udist_t  Y, udist_t  Z, steprate_t  vMax = 0) { MoveAbsEx(vMax, X_AXIS, X, Y_AXIS, Y, Z_AXIS, Z, -1); }

protected:

	virtual void StepBegin(const SStepBuffer* stepBuffer) override;

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override;
	virtual void StepRequest(bool   isr) override;

	virtual void    SetEnable(axis_t axis, uint8_t level, bool /* force */) override { _level[axis] = level; };
	virtual uint8_t GetEnable(axis_t axis) override { return _level[axis]; }

public:

	// function for testing purpose (make public)

	struct SMovementX
	{
		SMovement mv;
	};

	SMovementX GetMovement(uint8_t idxFromHead)
	{
		SMovementX mv;
		idxFromHead = _movements._queue.NextIndex(_movements._queue.GetHeadPos(), idxFromHead);
		mv.mv       = _movements._queue.Buffer[idxFromHead];
		return mv;
	}

	uint8_t GetMovementCount() const
	{
		return _movements._queue.Count();
	}

private:

	uint8_t _level[NUM_AXIS];

public:

	virtual void StartTimer(timer_t timerB) override; // 0 => set idle timer (==Timer not running)
	virtual void SetIdleTimer() override;             // set idle Timer

	virtual void OptimizeMovementQueue(bool force) override;
	virtual bool MoveReference(axis_t       axis, uint8_t referenceId, bool toMin, steprate_t vMax, sdist_t maxDist, sdist_t distToRef, sdist_t distIfRefIsOn) override;

	// Test extensions

	void InitTest(const char* fileName = nullptr);
	void EndTest(const char*  fileName = nullptr);

	bool DelayOptimization;
	bool SplitFile;
	bool UseSpeedSign;
	int  CacheSize;

private:

	void WriteTestResults(const char* fileName);

	const char* _fileName;
	int         _flushCount;

	struct STimerEvent
	{
		timer_t TimerValues;
		int     Steps;
		int     Count;

		struct SAxis
		{
			int Multiplier;
			int MoveAxis;
			int Distance;
		}       Axis[NUM_AXIS_MVC];

		char MSCInfo[MOVEMENTINFOSIZE];
	};

	int _TotalSteps;

	void InitCache()
	{
		_eventIdx = 0;
		memset(_TimerEvents, 0, sizeof(STimerEvent) * CacheSize);
	}

	int          _exportIdx;
	int          _eventIdx;
	STimerEvent* _TimerEvents;
	int          _oldCacheSize;

	int     _sumTime[NUM_AXIS_MVC];
	int     _count[NUM_AXIS_MVC];
	int     _total[NUM_AXIS_MVC];
	char    _speed[NUM_AXIS_MVC][20];
	int64_t _totalTime;
	int     _lastTimer;
	int     _refMoveStart;

	bool    _isReferenceMove;
	uint8_t _isReferenceId;
	int     _referenceMoveSteps;

	void DoISR();

public:

	void HandleIdle();
};
