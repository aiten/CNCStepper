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

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <Arduino.h>
#include <ctype.h>

#include "Stepper.h"
#include "PushValue.h"
#include "UtilitiesStepperLib.h"

////////////////////////////////////////////////////////

CStepper::CStepper()
{
	_numAxes = NUM_AXIS;
}

////////////////////////////////////////////////////////

template <> CStepper* CSingleton<CStepper>::_instance = nullptr;

uint8_t          CStepper::_mySteps[NUM_AXIS];
volatile uint8_t CStepper::_setState;
uint8_t          CStepper::_myCnt;

////////////////////////////////////////////////////////

void CStepper::InitMemVar()
{
	axis_t i;

	_pod = POD(); //POD init object with 0

	// look to ASM => more for() are faster an smaller

#if USESLIP
	for (i=0;i<NUM_AXIS;i++)	_slipSum[i]=0;
	for (i=0;i<NUM_AXIS;i++)	_slip[i]=0;
#endif

#ifndef REDUCED_SIZE
	// must be set in CMyControl
	for (i = 0; i < NUM_AXIS; i++) _pod._limitMax[i] = 0x00ffffff;
#endif

	for (i = 0; i < NUM_REFERENCE; i++) { _pod._referenceHitValue[i] = 255; }

	_pod._checkReference = true;
	_pod._timerBacklash  = timer_t(-1);

	_pod._limitCheck = true;
	_pod._idleLevel  = LevelOff;

	_pod._speedOverride    = SpeedOverride100P;
	_pod._timeOutEnableAll = TIMEOUTSETIDLE_DEFAULT;

	//	SetUsual(28000);	=> reduce size => hard coded
	SetDefaultMaxSpeed(28000, 350, 380);
	for (i = 0; i < NUM_AXIS; i++) { SetJerkSpeed(i, 1000); }

	for (i = 0; i < MOVEMENTBUFFERSIZE; i++) _movements._queue.Buffer[i]._state = SMovement::StateDone;

#ifdef _MSC_VER
	MSCInfo = "";
#endif
}

////////////////////////////////////////////////////////

void CStepper::SetUsual(steprate_t vMax)
{
	// with ProxxonMF70
	// maxSteprate ca. 28000
	// acc & dec = 350
	// JerkSpeed = 1000

	const steprate_t defSpeed = 28000;
	const steprate_t defAcc   = 350;
	const steprate_t defDec   = 380;
	const steprate_t defJerk  = 1000;

	auto     jerk = steprate_t(MulDivU32(vMax, defJerk, defSpeed));
	uint32_t sqrt = _ulsqrt_round(vMax * 10000l / defSpeed);

	auto acc = steprate_t(sqrt * defAcc / 100l);
	auto dec = steprate_t(sqrt * defDec / 100l);

	// acc and dec must not be y 62 => this is to slow
	if (dec < 62) dec = 62;
	if (acc < 62) acc = 62;

	SetDefaultMaxSpeed(vMax, acc, dec);
	for (axis_t i = 0; i < NUM_AXIS; i++) { SetJerkSpeed(i, jerk); }
}

////////////////////////////////////////////////////////

void CStepper::Init()
{
	InitMemVar();
	InitTimer();
#if defined(__SAM3X8E__) || defined(__SAMD21G18A__) 
	CHAL::InitBackground(HandleBackground);
#endif

	GoIdle();

	for (uint8_t i = 0; i < NUM_AXIS; i++)
	{
		SetEnableSafe(i, LevelOff, true);
	}
}

////////////////////////////////////////////////////////

void CStepper::AddEvent(StepperEvent event, uintptr_t eventParam, SEvent& oldEvent)
{
	oldEvent           = _event;
	_event._event      = event;
	_event._eventParam = eventParam;
}

////////////////////////////////////////////////////////

void CStepper::StartTimer(timer_t timer)
{
	_pod._timerRunning = true;
	CHAL::StartTimer1OneShot(timer);
}

////////////////////////////////////////////////////////

void CStepper::SetIdleTimer()
{
	CHAL::StartTimer1OneShot(IDLETIMER1VALUE);
	_pod._timerRunning = false;
}

////////////////////////////////////////////////////////

void CStepper::QueueMove(const mdist_t dist[NUM_AXIS], const bool directionUp[NUM_AXIS], timer_t timerMax, uint8_t stepMult)
{
	//DumpArray<mdist_t,NUM_AXIS>(F("QueueMove"),dist,false);
	//DumpArray<bool,NUM_AXIS>(F("Dir"),directionUp,false);
	//DumpType<timer_t>(F("tm"),timerMax,true);

	mdist_t steps = 0;

	axisArray_t directionMask = 0;
	axisArray_t direction     = 0;
	axisArray_t mask          = 1;

	for (uint8_t i = 0; i < NUM_AXIS; i++)
	{
		if (dist[i])
		{
			directionMask += mask;
			if (directionUp[i])
			{
				direction += mask;
			}
			if (steps < dist[i])
			{
				steps = dist[i];
			}
		}
		mask *= 2;
	}

	if (steps == 0) // nothing to do
	{
#ifndef REDUCED_SIZE
		Info(MESSAGE_STEPPER_EmptyMoveSkipped);
#endif
		return;
	}

	steps *= stepMult;

	if (IsSetBacklash())
	{
		if ((_pod._lastDirection & directionMask) != direction)
		{
			mdist_t backlashDist[NUM_AXIS] = { 0 };

			mdist_t backlashSteps = 0;
			mask                  = 1;
			for (uint8_t i = 0; i < NUM_AXIS; i++)
			{
				if ((_pod._lastDirection & directionMask & mask) != (direction & mask) && dist[i] && _pod._backlash[i])
				{
					backlashDist[i] = _pod._backlash[i];
					if (backlashDist[i] > backlashSteps)
					{
						backlashSteps = backlashDist[i];
					}
				}
				mask *= 2;
			}

			if (backlashSteps)
			{
				// need backlash
#ifndef REDUCED_SIZE
				Info(MESSAGE_STEPPER_Backlash);
#endif
				WaitUntilCanQueue();

				_movements._queue.NextTail().InitMove(this, GetPrevMovement(_movements._queue.GetNextTailPos()), backlashSteps, backlashDist, directionUp, _pod._timerBacklash);
				_movements._queue.NextTail().SetBacklash();

				EnqueueAndStartTimer(false);
			}
		}
	}

	// set all bits in lastDirection where axes moves
	_pod._lastDirection &= ~directionMask;
	_pod._lastDirection += direction;

	// wait until free movement buffer

	WaitUntilCanQueue();

	_movements._queue.NextTail().InitMove(this, GetPrevMovement(_movements._queue.GetNextTailPos()), steps, dist, directionUp, timerMax);

	EnqueueAndStartTimer(true);
}

////////////////////////////////////////////////////////

void CStepper::QueueWait(const mdist_t dist, timer_t timerMax, uint32_t clock, bool checkWaitConditional)
{
	WaitUntilCanQueue();
	_movements._queue.NextTail().InitWait(this, dist, timerMax, clock, checkWaitConditional);

	EnqueueAndStartTimer(true);
}

////////////////////////////////////////////////////////

void CStepper::WaitUntilCanQueue()
{
	while (_movements._queue.IsFull())
	{
		OnWait(MovementQueueFull);
	}
}

////////////////////////////////////////////////////////

bool CStepper::StartMovement()
{
	_movements._queue.Head().CalcNextSteps(false);
	if (_movements._queue.Head().IsFinished())
	{
		_movements._queue.Dequeue();
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////

void CStepper::EnqueueAndStartTimer(bool waitFinish)
{
	_movements._queue.Enqueue();

	OptimizeMovementQueue(false);

	if (_pod._timerRunning)
	{
		// situation: wait for last interrupt, need recalculate next step for stepBuffer
		CCriticalRegion criticalRegion;
		if (_movements._queue.Count() == 1 && _stepBuffer.IsEmpty())
		{
			StartMovement();
		}
	}
	else
	{
#ifndef REDUCED_SIZE
		_pod._timerLastCheckEnable =
#endif
			_pod._timerStartOrOnIdle = millis();

		for (axis_t i = 0; i < NUM_AXIS; i++)
		{
#ifndef REDUCED_SIZE
			_pod._timeEnable[i] = GetEnableTimeout(i);
#endif
			SetEnableSafe(i, CStepper::LevelMax, true);
		}

		OnStart();

		if (StartMovement())
		{
			// fill step-buffer with interrupts enabled, otherwise "Zero" will cause an usb failure if interrupts are blocked to long
			FillStepBuffer();
			CCriticalRegion criticalRegion;
			StepRequest(false);
		}
		else
		{
			// empty move => startup failed
			GoIdle();
		}
	}

	if (waitFinish && IsWaitFinishMove())
	{
		WaitBusy();
	}
}

////////////////////////////////////////////////////////

void CStepper::SMovement::InitMove(CStepper* stepper, SMovement* mvPrev, mdist_t steps, const mdist_t dist[NUM_AXIS], const bool directionUp[NUM_AXIS], timer_t timerMax)
{
	axis_t i;

	// memset(this, 0, sizeof(SMovement)); => set all mem-vars!!!

	_stepper             = stepper;
	_pod._move._timerMax = timerMax;

	_backlash = false;

	_steps = steps;
	memcpy(_distance_, dist, sizeof(_distance_));

#ifdef _MSC_VER
	strcpy_s(_mvMSCInfo, _stepper->MSCInfo);
#endif

	// calculate relative speed for axis => limit speed for axis

	for (i = 0; i < NUM_AXIS; i++)
	{
		mdist_t d = dist[i];
		if (d)
		{
			uint32_t axisTimer = MulDivU32(_pod._move._timerMax, _steps, d);
			if (axisTimer < uint32_t(stepper->_pod._timerMax[i]))
			{
				timerMax             = timer_t(MulDivU32(stepper->_pod._timerMax[i], d, _steps));
				_pod._move._timerMax = max(timerMax, _pod._move._timerMax);
			}
		}
	}

	// and acc/dec values

	_pod._move._timerAcc = 0;
	_pod._move._timerDec = 0;

	for (i = 0; i < NUM_AXIS; i++)
	{
		mdist_t d = dist[i];
		if (d)
		{
			auto accDec = timer_t(MulDivU32(stepper->_pod._timerAcc[i], d, _steps));
			if (accDec > _pod._move._timerAcc)
			{
				_pod._move._timerAcc = accDec;
			}

			accDec = timer_t(MulDivU32(stepper->_pod._timerDec[i], d, _steps));
			if (accDec > _pod._move._timerDec)
			{
				_pod._move._timerDec = accDec;
			}
		}
	}

	// calculate StepMultiplier and adjust distance

	uint8_t maxMultiplier = CStepper::GetStepMultiplier(_pod._move._timerMax);
	_lastStepDirCount     = 0;
	_dirCount             = 0;

	if (maxMultiplier > 1)
	{
		mdist_t calcFullSteps = _steps / maxMultiplier;
		if (_steps % maxMultiplier == 0)
		{
			calcFullSteps--;
		}

		for (i = NUM_AXIS - 1;; i--)
		{
			uint8_t multiplier = maxMultiplier;
			uint8_t axisDiff   = 0;
			if (_distance_[i])
			{
				if (calcFullSteps)
				{
					multiplier = uint8_t(_distance_[i] / calcFullSteps); // should fit in unsigned char!
					if (multiplier > maxMultiplier)
					{
						multiplier = maxMultiplier;
					}
					if ((_distance_[i] % calcFullSteps) != 0 && multiplier < maxMultiplier)
					{
						multiplier++;
					}

					if (multiplier != maxMultiplier)
					{
						_distance_[i] = mdist_t(MulDivU32(_distance_[i], maxMultiplier, multiplier));
					}
					else if (_distance_[i] != _steps)
					{
						// round up => avoid rounding problems
						if (_distance_[i] % multiplier)
						{
							_distance_[i] = ((_distance_[i] / multiplier) + 1) * multiplier;
						}
					}
#
					auto distInit = uint32_t(_steps / maxMultiplier / 2);
					auto distSum  = uintXX_t(_distance_[i]) * uintXX_t(calcFullSteps);
					auto s        = mdist_t(((distInit + distSum) / uintXX_t(_steps) * multiplier));

					axisDiff = uint8_t(dist[i] - s); // must be in range 0..7
				}
				else
				{
					axisDiff = uint8_t(dist[i]); // must be in range 0..7
				}
			}

			_lastStepDirCount *= 16;
			_lastStepDirCount += axisDiff;
			_dirCount *= 16;
			_dirCount += multiplier;

			if (directionUp[i])
			{
				_lastStepDirCount += 8;
				_dirCount += 8;
			}

			if (i == 0)
			{
				break;
			}
		}
	}
	else
	{
		for (i = NUM_AXIS - 1;; i--)
		{
			_dirCount *= 16;
			_dirCount += 1;
			if (directionUp[i])
			{
				_dirCount += 8;
			}

			if (i == 0)
			{
				break;
			}
		}
		_lastStepDirCount = _dirCount;
	}

	_pod._move._ramp._timerStart = GetUpTimerAcc();
	_pod._move._ramp._timerStop  = GetUpTimerDec();
	_pod._move._timerRun         = _pod._move._timerMax;

	if (_pod._move._timerRun > _pod._move._ramp._timerStart)
		_pod._move._ramp._timerStart = _pod._move._timerRun;

	if (_pod._move._timerRun > _pod._move._ramp._timerStop)
		_pod._move._ramp._timerStop = _pod._move._timerRun;

	_state = StateReadyMove;

	_pod._move._timerJunctionToPrev = timer_t(-1); // force optimization

	bool prevIsMove = mvPrev && mvPrev->IsActiveMove();
	if (prevIsMove)
	{
		CalcMaxJunctionSpeed(mvPrev);
		_pod._move._timerEndPossible = timer_t(-1);
	}
	else
	{
		_pod._move._timerEndPossible = _stepper->GetTimer(_steps, GetUpTimerAcc());
	}

	_pod._move._ramp.RampUp(this, _pod._move._timerRun, timer_t(-1));
	_pod._move._ramp.RampDown(this, timer_t(-1));
	_pod._move._ramp.RampRun(this);

	//stepper->Dump(DumpAll);
}

////////////////////////////////////////////////////////

void CStepper::SMovement::InitStop(SMovement* mvPrev, timer_t timer, timer_t decTimer)
{
	// must be a copy off current (executing) move
	*this = *mvPrev;

	mvPrev->_steps = _stepper->_movementState._n; // stop now

	_pod._move._timerDec = decTimer;

	mdist_t downSteps = CStepper::GetDecSteps(timer, decTimer);

	for (uint8_t i = 0; i < NUM_AXIS; i++)
	{
		_distance_[i] = mdist_t(RoundMulDivUInt(_distance_[i], downSteps, _steps));
	}

	_state = SMovement::StateReadyMove;

	_steps = downSteps;

	_pod._move._ramp._timerRun = timer;

	_pod._move._ramp.RampUp(this, timer, timer);
	_pod._move._ramp.RampDown(this, timer_t(-1));
}

////////////////////////////////////////////////////////

void CStepper::SMovement::InitWait(CStepper* stepper, mdist_t steps, timer_t timer, uint32_t clock, bool checkWaitConditional)
{
	//this is no POD because of methods => *this = SMovement();		
	memset(this, 0, sizeof(SMovement)); // init with 0

	_stepper                         = stepper;
	_steps                           = steps;
	_pod._wait._timer                = timer;
	_pod._wait._checkWaitConditional = checkWaitConditional;
	_pod._wait._endTime              = clock;

	_state = StateReadyWait;
}

void CStepper::SMovement::InitIoControl(CStepper* stepper, uint8_t tool, uint16_t level)
{
	//this is no POD because of methods => *this = SMovement();		
	memset(this, 0, sizeof(SMovement)); // init with 0

	_stepper        = stepper;
	_pod._io._tool  = tool;
	_pod._io._level = level;
	_state          = StateReadyIo;
}

////////////////////////////////////////////////////////

mdist_t CStepper::SMovement::GetDistance(axis_t axis) const
{
	if (_distance_[axis])
	{
		uint8_t multiplier    = GetStepMultiplier(axis);
		uint8_t maxMultiplier = GetMaxStepMultiplier();
		if (multiplier != maxMultiplier)
		{
			return mdist_t(MulDivU32(_distance_[axis], multiplier, maxMultiplier));
		}
	}
	return _distance_[axis];
}

////////////////////////////////////////////////////////

uint8_t CStepper::SMovement::GetMaxStepMultiplier() const
{
	DirCount_t count         = _dirCount;
	uint8_t    maxMultiplier = 0;

	for (uint8_t i = 0;; i++)
	{
		maxMultiplier = max(maxMultiplier, uint8_t(uint8_t(count) % 8));
		if (i == NUM_AXIS - 1)
		{
			break;
		}

		count /= 16;
	}
	return maxMultiplier;
}

////////////////////////////////////////////////////////

void CStepper::SMovement::SRamp::RampUp(SMovement* movement, timer_t timerRun, timer_t timerJunction)
{
	_timerRun           = timerRun;
	timer_t timerAccDec = movement->GetUpTimerAcc();

	if (timerJunction >= timerAccDec) // check from v0=0
	{
		_timerStart = max(timerAccDec, _timerRun);
		_nUpOffset  = 0;
		_upSteps    = GetAccSteps(_timerRun, timerAccDec);
	}
	else
	{
		// start from v0 != 0
		_timerStart = timerJunction;
		if (_timerStart >= _timerRun) // acc while start
		{
			_nUpOffset = GetAccSteps(_timerStart, timerAccDec);
			_upSteps   = GetAccSteps(_timerRun, timerAccDec) - _nUpOffset;
		}
		else
		{
			_upSteps    = CStepper::GetAccSteps(_timerRun, timerAccDec);
			timerAccDec = movement->GetUpTimerDec();
			_nUpOffset  = GetDecSteps(_timerStart, timerAccDec);
			_upSteps    = _nUpOffset - GetDecSteps(_timerRun, timerAccDec);
		}
	}
}

////////////////////////////////////////////////////////

void CStepper::SMovement::SRamp::RampDown(SMovement* movement, timer_t timerJunction)
{
	mdist_t steps       = movement->_steps;
	timer_t timerAccDec = movement->GetDownTimerDec();
	if (timerJunction >= timerAccDec)
	{
		_timerStop   = max(timerAccDec, _timerRun); // to v=0
		_downSteps   = CStepper::GetDecSteps(_timerRun, timerAccDec);
		_downStartAt = steps - _downSteps;
		_nDownOffset = 0;
	}
	else
	{
		_timerStop = timerJunction;

		if (_timerStop >= _timerRun) // dec while stop
		{
			// shift down phase with _nDownOffset steps
			_nDownOffset = CStepper::GetDecSteps(_timerStop, timerAccDec);
			_downSteps   = CStepper::GetDecSteps(_timerRun, timerAccDec);
			_downStartAt = steps - _downSteps + _nDownOffset;
		}
		else
		{
			timerAccDec  = movement->GetDownTimerAcc();
			_nDownOffset = CStepper::GetAccSteps(_timerStop, timerAccDec);
			//_downStartAt = _steps - 2 - (_nDownOffset - CStepper::GetAccSteps(_timerRun,timerAccDec));
			_downStartAt = steps - (_nDownOffset - CStepper::GetAccSteps(_timerRun, timerAccDec));
		}
		_downSteps = steps - _downStartAt;
	}
}

////////////////////////////////////////////////////////

void CStepper::SMovement::SRamp::RampRun(SMovement* movement)
{
	mdist_t steps = movement->_steps;

	if (_upSteps > steps || steps - _upSteps < _downSteps)
	{
		// we cant reach vMax for this movement, cut plateau.
		// leave relation between up and down squared => constant a
		mdist_t toMany = _downSteps + _upSteps - steps;
		mdist_t subUp;

		if (_downSteps == 0)
		{
			subUp = toMany;
		}
		else if (_upSteps == 0)
		{
			subUp = 0;
		}
		else
		{
			timer_t upTimer   = movement->GetUpTimer(_timerStart > _timerRun);
			timer_t downTimer = movement->GetUpTimer(_timerStop < _timerRun);

			uint32_t sqUp   = uint32_t(upTimer) * uint32_t(upTimer);
			uint32_t sqDown = uint32_t(downTimer) * uint32_t(downTimer);

			sqUp /= 0x1000; // may overrun => divide by 0x1000
			sqDown /= 0x1000;

			uint32_t sum = sqUp + sqDown;

			subUp = mdist_t(RoundMulDivU32(toMany, sqUp, sum)); // round
		}

		if (subUp > _upSteps || (toMany - subUp) > _downSteps)
		{
			subUp = _upSteps;
			// return false;	=> do not return in case of error, assume "valid" values!
		}

		_upSteps -= subUp;
		_downSteps -= toMany - subUp;

		if (_downSteps > steps)
		{
			_downSteps = steps;
			// return false;	=> do not return in case of error, assume "valid" values!
		}

		_downStartAt = steps - _downSteps;
	}
}

////////////////////////////////////////////////////////

bool CStepper::SMovement::Ramp(SMovement* mvNext)
{
#ifdef _MSC_VER
	assert(IsActiveMove());
	assert(mvNext == NULL || mvNext->IsActiveMove());
#endif
	if (!IsDownMove())
	{
		SRamp tmpRamp = _pod._move._ramp;
		tmpRamp.RampUp(this, _pod._move._timerRun, _pod._move._timerJunctionToPrev);
		tmpRamp.RampDown(this, mvNext ? mvNext->_pod._move._timerJunctionToPrev : GetDownTimerDec());
		tmpRamp.RampRun(this);

		CCriticalRegion criticalRegion;

		if (IsReadyForMove() ||
			(IsUpMove() && _stepper->_movementState._n < tmpRamp._upSteps) ||    // in acc
			(IsRunMove() && _stepper->_movementState._n < tmpRamp._downStartAt)) // in run
		{
			_pod._move._ramp = tmpRamp;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////
// drill down the junction speed if speed at junction point is not possible

void CStepper::SMovement::AdjustJunctionSpeedH2T(SMovement* mvPrev, SMovement* mvNext)
{
	if (!IsActiveMove()) return; // Move became inactive by ISR or "WaitState"/"IoControl"

	if (mvPrev == nullptr || IsRunOrDownMove()) // no prev or processing (can be if the ISR has switched to the next move)
	{
		// first "now" executing move
		if (IsRunOrUpMove())
		{
			if (IsRunMove())
			{
				// Option(TODO): we could accelerate with "down" move, 
				_pod._move._timerEndPossible = _pod._move._ramp._timerRun;
			}
			else
			{
				// just continue accelerate to the end of the move
				_pod._move._timerEndPossible = _stepper->GetTimerAccelerating(_steps, _pod._move._ramp._timerStart, GetUpTimerAcc());
			}
		}
		else
			_pod._move._timerEndPossible = _pod._move._ramp._timerStop;
	}
	else
	{
		_pod._move._timerEndPossible = _stepper->GetTimerAccelerating(_steps, mvPrev->IsActiveMove() ? (mvPrev->IsProcessingMove() ? mvPrev->_pod._move._ramp._timerStop : mvPrev->_pod._move._timerEndPossible) : -1, GetUpTimerAcc());

		if (_pod._move._timerEndPossible > _pod._move._timerMax)
		{
			// not faster as required
			_pod._move._timerRun = max(_pod._move._timerEndPossible, _pod._move._timerRun);
		}
	}

	if (mvNext != nullptr)
	{
#ifdef _MSC_VER
		assert(mvNext->IsActiveMove());
#endif
		// next element available, calculate junction speed
		mvNext->_pod._move._timerJunctionToPrev = max(mvNext->_pod._move._timerMaxJunction, max(_pod._move._timerEndPossible, mvNext->_pod._move._timerJunctionToPrev));
		_pod._move._timerEndPossible            = mvNext->_pod._move._timerJunctionToPrev;
	}

	if (!Ramp(mvNext))
	{
		// modify of ramp failed => do not modify _pod._move._timerEndPossible
		_pod._move._timerEndPossible = _pod._move._ramp._timerStop;
		if (mvNext != nullptr) mvNext->_pod._move._timerJunctionToPrev = _pod._move._ramp._timerStop;
	}
}

////////////////////////////////////////////////////////
// drill down the junction speed if speed at junction point is not possible
// return (Tail to Head) if entry has changed, if not, all previous (to head) no change is required

bool CStepper::SMovement::AdjustJunctionSpeedT2H(SMovement* mvPrev, SMovement* mvNext)
{
	if (!IsActiveMove()) return !IsSkipForOptimizing(); // Move became inactive by ISR or "wait" move or ignore "IOControl

	if (mvNext == nullptr)
	{
		// last element in queue, v(end) = 0, we have to stop
		_stepper->_movements._timerStartPossible = _stepper->GetTimer(_steps, GetDownTimerDec());
	}
	else
	{
		// calculate new speed at start of move
		// assume _timerStartPossible (of next move) at end
		_stepper->_movements._timerStartPossible = _stepper->GetTimerAccelerating(_steps, _stepper->_movements._timerStartPossible, GetDownTimerDec());
	}

	if (mvPrev != nullptr)
	{
		_pod._move._timerRun = _pod._move._timerMax;

		if (!mvPrev->IsActiveMove())
			return true; // waitState => no optimize, break here

		// prev element available, calculate junction speed
		timer_t junctionToPrev = max(_pod._move._timerMaxJunction, _stepper->_movements._timerStartPossible);
		if (junctionToPrev == _pod._move._timerJunctionToPrev)
			return true; // nothing changed (prev movements do not change)

		_pod._move._timerJunctionToPrev          = junctionToPrev;
		_stepper->_movements._timerStartPossible = _pod._move._timerJunctionToPrev;
	}

	return false;
}

////////////////////////////////////////////////////////
// calculate the max junction between two movements- consider jerk - speed is maxspeed - only calculated once (at setup time of movement)

void CStepper::SMovement::CalcMaxJunctionSpeed(SMovement* mvPrev)
{
#ifdef _MSC_VER
	assert(IsActiveMove());
	assert(mvPrev==NULL || mvPrev->IsActiveMove());
#endif

	// .1 => prev
	// .2 => next(this)

	// default => fastest move (no jerk)
	_pod._move._timerMaxJunction = min(mvPrev->_pod._move._timerMax, _pod._move._timerMax);
	timer_t timerMaxJunction;
	timer_t timerMaxJunctionAcc = mvPrev->GetUpTimerAcc();

	mdist_t s1 = mvPrev->_steps;
	mdist_t s2 = _steps;

	axis_t mainAxis; // max moving axis	=> no jerk 

	for (mainAxis = 0; mainAxis < NUM_AXIS; mainAxis++)
	{
		if (s1 == mvPrev->GetDistance(mainAxis) && s2 == GetDistance(mainAxis) && mvPrev->GetDirectionUp(mainAxis) == GetDirectionUp(mainAxis))
		{
			_pod._move._timerMaxJunction = (int32_t(mvPrev->_pod._move._timerMax) + int32_t(_pod._move._timerMax)) / 2;
			break;
		}
	}

	timer_t moveTimerMaxJunction = _pod._move._timerMaxJunction;
	/*
		printf("H: s1=%i s2=%i\n", (int)s1, (int)s2);
		for (int ii = 0; ii < NUM_AXIS; ii++)
		{
		printf("%i: Prev:%i%c\tThis:%i%c\n", ii, (int)mvPrev->GetDistance(ii), mvPrev->GetDirectionUp(ii) ? '+' : '-', (int)GetDistance(ii), GetDirectionUp(ii) ? '+' : '-');
		}
		printf("");

		if (mainAxis >= NUM_AXIS)
		{
		//_pod._move._timerMaxJunction = (int32_t(mvPrev->_timerMax) + int32_t(_timerMax)) / 2;
		//return;
		}
		*/
	for (axis_t i = 0; i < NUM_AXIS; i++)
	{
		if (i != mainAxis)
		{
			mdist_t d1 = mvPrev->GetDistance(i);
			mdist_t d2 = GetDistance(i);

			steprate_t v1 = _stepper->TimerToSpeed(mvPrev->_pod._move._timerMax);
			steprate_t v2 = _stepper->TimerToSpeed(_pod._move._timerMax);

			if (d1 != s1)
			{
				v1 = steprate_t(RoundMulDivUInt(v1, d1, s1));
			}
			if (d2 != s2)
			{
				v2 = steprate_t(RoundMulDivUInt(v2, d2, s2));
			}

			int32_t vDiff;

			if (v1 == 0 || v2 == 0 || mvPrev->GetDirectionUp(i) == GetDirectionUp(i))
			{
				// same direction (v1 and v2 not 0)
				vDiff = v1 > v2 ? v1 - v2 : v2 - v1;

				if (vDiff > int32_t(_stepper->_pod._maxJerkSpeed[i]))
				{
					// reduce total speed by ratio maxJerk <=> current jerk
					timerMaxJunction             = _stepper->SpeedToTimer(steprate_t(RoundMulDivUInt(_stepper->TimerToSpeed(moveTimerMaxJunction), _stepper->_pod._maxJerkSpeed[i], steprate_t(vDiff))));
					_pod._move._timerMaxJunction = max(_pod._move._timerMaxJunction, min(timerMaxJunction, timerMaxJunctionAcc));
				}
			}
			else
			{
				// different direction, add speed
				vDiff = int32_t(v1) + int32_t(v2);

				if (mainAxis >= NUM_AXIS)
				{
					_pod._move._timerMaxJunction = timerMaxJunctionAcc; //Stop and go
				}
				else
				{
					if (vDiff > int32_t(_stepper->_pod._maxJerkSpeed[i]))
					{
						// reduce total speed by ratio maxJerk <=> current jerk
						timerMaxJunction             = _stepper->SpeedToTimer(steprate_t(RoundMulDivUInt(_stepper->TimerToSpeed(moveTimerMaxJunction), _stepper->_pod._maxJerkSpeed[i], steprate_t(vDiff))));
						_pod._move._timerMaxJunction = max(_pod._move._timerMaxJunction, min(timerMaxJunction, timerMaxJunctionAcc));
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////

CStepper::SMovement* CStepper::GetNextMovement(uint8_t idx)
{
	// get next movement which can be optimized (no IOControl)

	if (_movements._queue.IsEmpty())
	{
		return nullptr;
	}
	while (true)
	{
		idx = _movements._queue.H2TInc(idx);
		if (!_movements._queue.H2TTest(idx))
		{
			return nullptr;
		}

		if (!_movements._queue.Buffer[idx].IsSkipForOptimizing())
		{
			return &_movements._queue.Buffer[idx];
		}
	}
	// not reachable;
}

////////////////////////////////////////////////////////

CStepper::SMovement* CStepper::GetPrevMovement(uint8_t idx)
{
	// get previous movement which can be optimized (no IOControl)

	if (_movements._queue.IsEmpty())
	{
		return nullptr;
	}
	while (true)
	{
		idx = _movements._queue.T2HInc(idx);
		if (!_movements._queue.T2HTest(idx))
		{
			return nullptr;
		}

		if (!_movements._queue.Buffer[idx].IsActiveIo())
		{
			return &_movements._queue.Buffer[idx];
		}
	}
}

////////////////////////////////////////////////////////

void CStepper::OptimizeMovementQueue(bool /* force */)
{
	if (_movements._queue.IsEmpty() || _movements._queue.Count() < 2)
	{
		return;
	}

	uint8_t idx;
	uint8_t idxNoChange = _movements._queue.H2TInit();

	////////////////////////////////////
	// calculate junction (max) speed!

	for (idx = _movements._queue.T2HInit(); _movements._queue.T2HTest(idx); idx = _movements._queue.T2HInc(idx))
	{
		if (_movements._queue.Buffer[idx].AdjustJunctionSpeedT2H(GetPrevMovement(idx), GetNextMovement(idx)))
		{
			idxNoChange = idx;
			break;
		}
	}

	////////////////////////////////////
	// calculate junction (max) speed!
	// and calculate trapezoid ramp (as soon as possible)

	for (idx = idxNoChange; _movements._queue.H2TTest(idx); idx = _movements._queue.H2TInc(idx))
	{
		_movements._queue.Buffer[idx].AdjustJunctionSpeedH2T(GetPrevMovement(idx), GetNextMovement(idx));
	}
}

////////////////////////////////////////////////////////

void CStepper::OnIdle(uint32_t idleTime)
{
	CallEvent(OnIdleEvent);
	if (_pod._timeOutEnableAll != 0 && idleTime > GetEnableTimeoutInMs())
	{
		bool setEnable = false;
		for (uint8_t i = 0; i < NUM_AXIS; i++)
		{
			setEnable |= SetEnableSafe(i, _pod._idleLevel, true);
		}
		if (setEnable)
		{
			CallEvent(OnDisableEvent);
		}
	}
}

////////////////////////////////////////////////////////

void CStepper::OnStart()
{
	CallEvent(OnStartEvent);
}

////////////////////////////////////////////////////////

void CStepper::OnWait(EnumAsByte(EWaitType) wait)
{
	CallEvent(OnWaitEvent, wait);
}

////////////////////////////////////////////////////////

void CStepper::OnError(cncerror_t error)
{
	CallEvent(OnErrorEvent, uintptr_t(error));
}

////////////////////////////////////////////////////////

void CStepper::OnWarning(cncerror_t warning)
{
	CallEvent(OnWarningEvent, uintptr_t(warning));
	StepperSerial.print(MESSAGE_WARNING);
	StepperSerial.println(warning);
}

////////////////////////////////////////////////////////

void CStepper::OnInfo(cncerror_t info)
{
	CallEvent(OnInfoEvent, uintptr_t(info));
	StepperSerial.print((MESSAGE_INFO));
	StepperSerial.println(info);
}

////////////////////////////////////////////////////////

void CStepper::WaitBusy()
{
	while (IsBusy())
	{
		// wait until finish all movements
		OnWait(WaitBusyCall);
	}
}

////////////////////////////////////////////////////////

uint32_t CStepper::GetAccelerationFromTimer(mdist_t timerV0)
{
	// original a = v/t, => a=s/t^2
	// for first step we need c0 => convert to sec, a = 1/t^2
	// a = (F/timer)^2

	uint32_t x = TIMER1FREQUENCE / timerV0;
	return x * x;

	//range: v0=100 => 10000
	//range: v0=400 => 159201
}

////////////////////////////////////////////////////////
// reverse calc n from timerValue

timer_t CStepper::GetTimer(mdist_t steps, timer_t timerStart)
{
	// original v = sqrt(v0^2 + 2 a d)
	// because of int estimation: use factor
	//
	// v = sqrt((v0^2 + 2 a d) / factor^2)
	// v0 = 0

	if (steps > MAXACCDECSTEPS)
	{
		steps = MAXACCDECSTEPS;
	}

	uint32_t a2 = 2 * GetAccelerationFromTimer(timerStart);

	if (ToPrecisionU2(a2) + ToPrecisionU2(steps) > 31)
	{
		return TIMER1VALUEMAXSPEED;
	}

	uint32_t ad = a2 * steps;
	auto     v  = steprate_t((_ulsqrt(((ad) / 93) * 85)));

	return SpeedToTimer(v) + 1; // +1 	empiric tested to get better results
}

////////////////////////////////////////////////////////
// reverse calc n from timerValue

timer_t CStepper::GetTimerAccelerating(mdist_t steps, timer_t timerV0, timer_t timerStart)
{
	// original v = sqrt(v0^2 + 2 a d)
	// because of int estimation: use factor
	//
	// v = sqrt((v0^2 + 2 a d) / factor^2)

	if (steps > MAXACCDECSTEPS)
	{
		steps = MAXACCDECSTEPS;
	}

	uint32_t sqv0 = TimerToSpeed(timerV0);
	sqv0 *= sqv0;

	uint32_t a2 = 2 * GetAccelerationFromTimer(timerStart);

	if (ToPrecisionU2(a2) + ToPrecisionU2(steps) > 31)
	{
		return TIMER1VALUEMAXSPEED;
	}

	uint32_t ad = a2 * steps;
	auto     v  = steprate_t((_ulsqrt(((sqv0 + ad) / 93) * 85)));

	return SpeedToTimer(v) + 1; // +1 	empiric tested to get better results
}

////////////////////////////////////////////////////////
// reverse calc n from timerValue

timer_t CStepper::GetTimerDecelerating(mdist_t steps, timer_t timerV, timer_t timerStart)
{
	// original v = sqrt(v0^2 + 2 a d)
	// because of int estimation: use factor
	//
	// v = sqrt((v0^2 + 2 a d) / factor^2)

	if (steps > MAXACCDECSTEPS)
	{
		steps = MAXACCDECSTEPS;
	}

	uint32_t sqv0 = TimerToSpeed(timerV);
	sqv0 *= sqv0;

	uint32_t a2 = 2 * GetAccelerationFromTimer(timerStart);

	if (ToPrecisionU2(a2) + ToPrecisionU2(steps) > 31)
	{
		return TIMER1VALUEMAXSPEED;
	}

	uint32_t ad = a2 * steps;
	if (sqv0 < ad)
	{
		return timer_t(-1);
	}

	auto v = steprate_t((_ulsqrt(((sqv0 - ad) / 93) * 85)));

	return SpeedToTimer(v) + 1; // +1 	empiric tested to get better results
}

////////////////////////////////////////////////////////
// reverse calc n from timerValue

mdist_t CStepper::GetAccSteps(timer_t timer, timer_t timerStart)
{
	// original: d = v^2 / v0^2
	// tested with excel to fit to timer calculation with cn = cn-1 + (2*cn-1) / (4n+1) and use of "integer"

	uint32_t sqA2 = uint32_t(timer) * uint32_t(timer - 1) * 2;
	uint32_t sqB  = uint32_t(timerStart) * uint32_t(timerStart);

	// factor => tested with excel => timer *= 1.046 => timer^2 *= 1.0941 ( 1.046^2)
	// use int and not float => 93/85 = 1,094117647058824 

	// timerStart*timerStart * 93 must not overrun => sqrt(65536*95536*93) = 6795
	if (timerStart > 6795) // avoid overrun
	{
		sqB /= 128;
		sqB = MulDivU32(sqB, 93, 85);
		sqB *= 128;
	}
	else
	{
		sqB = MulDivU32(sqB, 93, 85);
	}

	return mdist_t(sqB / sqA2); // ceil
}

////////////////////////////////////////////////////////

mdist_t CStepper::GetAccSteps(timer_t timer1, timer_t timer2, timer_t timerStart)
{
	// timer1 = v1 (slower) => timer1 greater
	// timer2 = v2 (faster)

	if (timer1 < timer2) // swap if wrong
	{
		timer_t tmp = timer1;
		timer1      = timer2;
		timer2      = tmp;
	}

	return GetAccSteps(timer2, timerStart) - GetAccSteps(timer1, timerStart);
}

////////////////////////////////////////////////////////

mdist_t CStepper::GetSteps(timer_t timer1, timer_t timer2, timer_t timerStart, timer_t timerStop)
{
	if (timer1 > timer2)
	{
		return GetAccSteps(timer1, timer2, timerStart);
	}
	return GetDecSteps(timer1, timer2, timerStop);
}

////////////////////////////////////////////////////////

void CStepper::StopMove(steprate_t v0Dec)
{
	if (_movements._queue.Count() > 0)
	{
		SMovement& mv = _movements._queue.Head();

		if (mv.IsActiveWait())
		{
			CCriticalRegion critical;
			_movementState._n = mv._steps;
		}
		else
		{
			timer_t decTimer = v0Dec != 0 ? SpeedToTimer(v0Dec) : mv.GetDownTimerDec();

			{
				CCriticalRegion critical;

				// do nothing if move is about to finish
				if (mv.IsDownMove())
				{
					return;
				}

				// remove all not executed moves and create a new one for dec
				// start downRamp now

				SubTotalSteps();

				_movements._queue.RemoveTail(_movements._queue.GetHeadPos());
				_movements._queue.NextTail().InitStop(&mv, _movementState._timer, decTimer);
				_movements._queue.Enqueue();
			}

			WaitBusy();
			memcpy(_pod._calculatedPos, _pod._current, sizeof(_pod._calculatedPos));
		}
	}
}

////////////////////////////////////////////////////////

void CStepper::AbortMove()
{
	CCriticalRegion critical;

	SubTotalSteps();

	_stepBuffer.Clear();
	_movements._queue.Clear();

	memcpy(_pod._calculatedPos, _pod._current, sizeof(_pod._calculatedPos));

	GoIdle();
}

////////////////////////////////////////////////////////

void CStepper::PauseMove()
{
	if (!_pod._pause)
	{
		_pod._pause = true;

#ifndef REDUCED_SIZE

		// insert into queue (where jerk speed to stop move will not exceed) 

		for (uint8_t idx = _movements._queue.H2TInit(); _movements._queue.H2TTest(idx); idx = _movements._queue.H2TInc(idx))
		{
			SMovement& mv = _movements._queue.Buffer[idx];

			bool canInsertAfter = !mv.IsActiveMove();

			if (!canInsertAfter)
			{
				// check jerk speed to stop

				steprate_t speedStop = TimerToSpeed(mv._pod._move._ramp._timerStop);
				mdist_t    s         = mv.GetSteps();

				canInsertAfter = true;

				for (axis_t x = 0; canInsertAfter && x < NUM_AXIS; x++)
				{
					mdist_t    d = mv.GetDistance(x);
					steprate_t v = speedStop;

					if (d != s)
					{
						v = steprate_t(RoundMulDivUInt(v, d, s));
					}

					canInsertAfter = _pod._maxJerkSpeed[x] > v;
				}
			}

			if (canInsertAfter)
			{
				WaitUntilCanQueue();
				_movements._queue.InsertTail(_movements._queue.NextIndex(idx))->InitWait(this, 0xffff, WAITTIMER1VALUE, 0, true);
				return;
			}
		}

#endif

		// just queue a stop at end of queue
		QueueWait(0xffff, WAITTIMER1VALUE, 0, true);
	}
}

////////////////////////////////////////////////////////

void CStepper::ContinueMove()
{
	_pod._pause = false;
}

////////////////////////////////////////////////////////

void CStepper::SubTotalSteps()
{
#ifndef REDUCED_SIZE

	// sub all pending steps to _totalSteps

	for (uint8_t idx = _movements._queue.T2HInit(); _movements._queue.T2HTest(idx); idx = _movements._queue.T2HInc(idx))
	{
		SMovement& mv = _movements._queue.Buffer[idx];
		if (mv.IsActiveMove())
		{
			_pod._totalSteps -= mv._steps;
			if (mv.IsProcessingMove())
			{
				_pod._totalSteps += _movementState._n;
			}
		}
	}

#endif
}

////////////////////////////////////////////////////////

void CStepper::EmergencyStopResurrect()
{
	AbortMove(); // make sure nothing is running
	_pod._emergencyStop = false;
	_pod._fatalError    = nullptr;
}

////////////////////////////////////////////////////////

uint8_t CStepper::GetStepMultiplier(timer_t timerMax)
{
	if (timerMax >= TIMER1VALUE(SPEED_MULTIPLIER_2)) return 1;
	if (timerMax >= TIMER1VALUE(SPEED_MULTIPLIER_3)) return 2;
	if (timerMax >= TIMER1VALUE(SPEED_MULTIPLIER_4)) return 3;
	if (timerMax >= TIMER1VALUE(SPEED_MULTIPLIER_5)) return 4;
	if (timerMax >= TIMER1VALUE(SPEED_MULTIPLIER_6)) return 5;
	if (timerMax >= TIMER1VALUE(SPEED_MULTIPLIER_7)) return 6;
	return 7;
}

////////////////////////////////////////////////////////

inline void CStepper::StepOut()
{
	// called in interrupt => must be "fast"
	// "Out" the Step to the stepper 

	// calculate all axes and set PINS parallel - DRV 8225 requires 1.9us * 2 per step => sequential is too slow 

	DirCount_t dir_count;

	{
		auto stepBuffer = &_stepBuffer.Head();
		StartTimer(stepBuffer->Timer - TIMEROVERHEAD);
		dir_count = stepBuffer->DirStepCount;
	}

#ifdef _MSC_VER
	StepBegin(&_stepBuffer.Head());
#endif
	// AVR: div with 256 is faster than 16 (loop shift)

	stepperstatic_avr uint8_t axesCount[NUM_AXIS];
	axisArray_t               directionUp = 0;

	uint8_t byteDirCount;
	bool    countIt = true;
	if (reinterpret_cast<DirCountByte_t*>(&dir_count)->byte.byteInfo.nocount != 0)
	{
		countIt = false;
	}

	for (uint8_t i = 0;; i++)
	{
#if defined (__AVR_ARCH__)
		if (i % 2 == 1)
		{
			byteDirCount = byteDirCount / 16;
		}
		else
		{
			byteDirCount = uint8_t(dir_count); //  &255;
			dir_count /= 256;
		}
#else
		byteDirCount = dir_count & 15;
		dir_count /= 16;
#endif

		axesCount[i] = byteDirCount & 7;
		directionUp /= 2;

		if (axesCount[i])
		{
			if ((byteDirCount & 8) != 0)
			{
				directionUp += (1 << (NUM_AXIS - 1));
				if (countIt)
				{
					_pod._current[i] += axesCount[i];
				}
			}
			else
			{
				if (countIt)
				{
					_pod._current[i] -= axesCount[i];
				}
			}
		}

		if (i == NUM_AXIS - 1)
		{
			break;
		}
	}

	directionUp = directionUp ^ _pod._invertDirection;
	Step(axesCount, directionUp, _pod._lastDirectionUp == directionUp);
	_pod._lastDirectionUp = directionUp;

	_stepBuffer.Dequeue();
}

////////////////////////////////////////////////////////

static volatile bool _backgroundActive = false;

void CStepper::StartBackground()
{
#if defined(__SAM3X8E__) || defined(__SAMD21G18A__) 
	// sam3x cannot call timer interrupt nested.
	// we use an other ISR (CAN) as Tail-chaining with lower (priority value is higher) priority and exit the Timer ISR

	if (_backgroundActive)
	{
		_pod._timerISRBusy++;
	}
	else
	{
		CHAL::BackgroundRequest();
	}

#else

	static volatile uint8_t reenterCount = 0;

	reenterCount = reenterCount+1;

	if (reenterCount != 1)
	{
		// other ISR is calculating!
#ifndef REDUCED_SIZE
		_pod._timerISRBusy++;
#endif
		reenterCount = reenterCount - 1;
		return;
	}

	// Reenable nested interrupts	=> usual EnableInterrupts
	CHAL::EnableInterrupts();

	// calculate next steps until buffer is full or nothing to do!
	// other timerIRQs may occur
	FillStepBuffer();

	CHAL::DisableInterrupts();
	reenterCount = reenterCount-1;

#endif
}

////////////////////////////////////////////////////////

void CStepper::FillStepBuffer()
{
	// calculate next steps until buffer is full or nothing to do!
	while (!_movements._queue.IsEmpty())
	{
		if (!_movements._queue.Head().CalcNextSteps(true)) // buffer full => wait (and leave ISR)
		{
			break;
		}

		if (_movements._queue.Head().IsFinished())
		{
			_movements._queue.Dequeue();
		}
	}

#ifndef REDUCED_SIZE

	// check if turn off stepper

	auto ms      = millis();
	auto diffSec = uint8_t((ms - _pod._timerLastCheckEnable) / 1024); // div 1024 is faster as 1000

	if (diffSec > 0)
	{
		_pod._timerLastCheckEnable = ms;

		for (axis_t i = 0; i < NUM_AXIS; i++)
		{
			if (_pod._timeEnable[i] != 0)
			{
				if (_pod._timeEnable[i] < diffSec) // may overrun
				{
					_pod._timeEnable[i] = 0;
				}
				else
				{
					_pod._timeEnable[i] -= diffSec;
				}

				if (_pod._timeEnable[i] == 0)
				{
					SetEnableSafe(i, _pod._idleLevel, true);
				}
			}
		}
	}

#endif
}

////////////////////////////////////////////////////////
// called as Tail-chaining on due (after Step() )
// due: do not check reenter => ISR on due can be only called once (not nested)

void CStepper::Background()
{
	_backgroundActive = true;
	FillStepBuffer();
	_backgroundActive = false;
}

////////////////////////////////////////////////////////

void CStepper::GoIdle()
{
	// start idle timer
	_pod._timerStartOrOnIdle = millis();
	SetIdleTimer();
	OnIdle(0);
}

////////////////////////////////////////////////////////

void CStepper::ContinueIdle()
{
	SetIdleTimer();
	OnIdle(millis() - _pod._timerStartOrOnIdle);
}

////////////////////////////////////////////////////////

void CStepper::StepRequest(bool isr)
{
	// called in interrupt => must be "fast"
	// first send commands to stepper driver
	// afterwards calculate the next steps in background

	// AVR:	 interrupts are disabled (ISR) or disable: see OnStart
	// SAM3X:no nested call of ISR 

	if (isr && !_pod._timerRunning)
	{
		ContinueIdle();
		return;
	}

	if (_stepBuffer.IsEmpty())
	{
		GoIdle();
		return;
	}

	if (_pod._emergencyStop)
	{
		AbortMove();
		return;
	}

	StepOut();

	if ((_pod._checkReference && IsAnyReference()))
	{
		FatalError(MESSAGE(MESSAGE_STEPPER_IsAnyReference));
		EmergencyStop();
		return;
	}

	// calculate next step 

	StartBackground();
}

////////////////////////////////////////////////////////

#if defined (stepperstatic_)

CStepper::SMovementState CStepper::_movementState;
CRingBufferQueue<CStepper::SStepBuffer, STEPBUFFERSIZE> CStepper::_stepBuffer;
CStepper::SMovements CStepper::_movements;
CStepper* CStepper::SMovement::_stepper;

#endif

////////////////////////////////////////////////////////

void CStepper::SMovementState::Init(SMovement* movement)
{
	mdist_t steps = movement->_steps;

	if (movement->_state == SMovement::StateReadyMove)
	{
		_count = movement->GetMaxStepMultiplier();
		_timer = movement->_pod._move._ramp._timerStart;
	}
	else
	{
		_count = 1;
		_timer = movement->_pod._wait._timer;
	}

	steps = (steps / _count) >> 1;

	for (axis_t i = 0; i < NUM_AXIS; i++)
	{
		_add[i] = steps;
	}

	_n    = 0;
	_rest = 0;
#ifndef REDUCED_SIZE
	_sumTimer = 0;
#endif
}

////////////////////////////////////////////////////////

bool CStepper::SMovementState::CalcTimerAcc(timer_t maxtimer, mdist_t n, uint8_t cnt)
{
	// use for float: Cn = Cn-1 - 2*Cn-1 / (4*N + 1)
	// use for INTEGER:
	// In = ((2*In-1)+Rn-1) / (4*N + 1)		=> quot
	// Rn = ((2*In-1)+Rn-1) % (4*N + 1)		=> remainder of division
	// Cn = Cn-1 - In

	if (maxtimer < _timer)
	{
		mudiv_t udivRemainder = mudiv(_timer * (2 * cnt) + _rest, n * 4 + 2 - cnt);

		_rest  = udivRemainder.rem;
		_timer = _timer - udivRemainder.quot;
		if (maxtimer >= _timer)
		{
			_timer = maxtimer;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////

bool CStepper::SMovementState::CalcTimerDec(timer_t minTimer, mdist_t n, uint8_t cnt)
{
	// use for float: Cn = Cn-1 + 2*Cn-1 / (4*N - 1)
	// use for INTEGER:
	// In = ((2*In-1)+Rn-1) / (4*N - 1)		=> quot
	// Rn = ((2*In-1)+Rn-1) % (4*N - 1)		=> remainder of division
	// Cn = Cn-1 - In

	if (minTimer > _timer)
	{
		if (n <= 1)
		{
			_timer = minTimer;
			return true;
		}
		mudiv_t udivRemainder = mudiv(_timer * (2 * cnt) + _rest, n * 4 - 1 - cnt);

		_rest  = udivRemainder.rem;
		_timer = _timer + udivRemainder.quot;
		if (minTimer <= _timer)
		{
			_timer = minTimer;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////

bool CStepper::SMovement::IsEndWait() const
{
	if (_pod._wait._checkWaitConditional)
	{
		// wait only if Stepper is "checkWaitConditional"
		if (!_stepper->IsPauseMove() && !_stepper->IsWaitConditional())
		{
			return true;
		}
	}
	if (_pod._wait._endTime != 0)
	{
		if (millis() >= _pod._wait._endTime)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////
// called in interrupt => must be "fast"

bool CStepper::SMovement::CalcNextSteps(bool continues)
{
	// return false if buffer full and nothing calculated.

	axis_t i;
	do
	{
		auto stepper = _stepper;
		auto mvState = &stepper->_movementState;

		if (
			stepper->_stepBuffer.IsFull() ||
			((_state == SMovement::StateReadyWait || _state == SMovement::StateReadyIo) && stepper->_stepBuffer.Count() > SYNC_STEPBUFFERCOUNT)
			)
		{
			// cannot add to queue
			return false;
		}

		if (_state == SMovement::StateReadyMove || _state == SMovement::StateReadyWait || _state == SMovement::StateReadyIo)
		{
			// Start of move/wait

			mvState->Init(this);

			for (i = 0; i < NUM_AXIS; i++)
			{
				if (_distance_[i] != 0)
				{
#ifndef REDUCED_SIZE
					stepper->_pod._timeEnable[i] = 0;
#endif
					stepper->SetEnableSafe(i, CStepper::LevelMax, false);
				}
			}

			if (_state == SMovement::StateReadyWait)
			{
				_state = StateWait;

				if (IsEndWait())
				{
					mvState->_n = _steps;
				}
			}
			if (_state == SMovement::StateReadyIo)
			{
				stepper->CallEvent(OnIoEvent, uintptr_t(&_pod._io));
				// pState->_n = _steps; => done by Init()
				// this will end move immediately
			}
		}

		mdist_t n     = mvState->_n;
		uint8_t count = mvState->_count;

		if (_steps <= n)
		{
			// End of move/wait/io

#ifndef REDUCED_SIZE
			for (i = 0; i < NUM_AXIS; i++)
			{
				if (_distance_[i] != 0)
				{
					stepper->_pod._timeEnable[i] = stepper->GetEnableTimeout(i);
				}
			}
#endif

			_state = StateDone;
			return true;
		}

		{
			// calculate f for step-buffer

			if (count > 1 && _steps - n <= count)
			{
				// last step with multiplier
				stepper->_stepBuffer.NextTail().Init(_lastStepDirCount);
				count = uint8_t(_steps - n); // must fit in unsigned char
			}
			else
			{
				DirCount_t stepCount = 0;
				DirCount_t mask      = 15;

				if (_backlash)
				{
					// ((DirCountByte_t*)&stepCount)->byteInfo.noCount = 1;	=> this force stepCount to be not in register
					DirCountByte_t x        = DirCountByte_t(); //POD
					x.byte.byteInfo.nocount = 1;
					stepCount += x.all;
				}

				for (i = 0;; i++)
				{
					// Check overflow!
					mdist_t oldAdd = mvState->_add[i];
					mvState->_add[i] += _distance_[i];
					if (mvState->_add[i] >= _steps || mvState->_add[i] < oldAdd)
					{
						mvState->_add[i] -= _steps;
						stepCount += mask & _dirCount;
					}
					if (i == NUM_AXIS - 1)
					{
						break;
					}
					mask *= 16;
				}
				stepper->_stepBuffer.NextTail().Init(stepCount);
			}
		}

		////////////////////////////////////
		// calc new timer

		if (_state == StateReadyMove)
		{
			if (mvState->_timer == _pod._move._ramp._timerRun)
			{
				_state = StateRun;
			}
			else
			{
				_state = mvState->_timer > _pod._move._ramp._timerRun ? StateUpAcc : StateUpDec;
				if (mvState->_count > 1 && _pod._move._ramp._nUpOffset == 0)
				{
					static const uint16_t corrTab[][2] PROGMEM =
					{
						{ 1300, 1402 },
						{ 611, 709 },
						{ 322, 400 },
						{ 307, 405 },
						{ 289, 403 }
					};
					uint16_t mul    = pgm_read_word(&corrTab[mvState->_count - 2][0]);
					uint16_t div    = pgm_read_word(&corrTab[mvState->_count - 2][1]);
					mvState->_timer = timer_t(MulDivU32(mvState->_timer, mul, div));
				}
				else if (mvState->_count <= 1 && _pod._move._ramp._nUpOffset == 0 && _state == StateUpDec)
				{
					// mdist_t is unsigned and _pod._move._ramp._nUpOffset -n cause an under run
					_pod._move._ramp._nUpOffset = 1;
				}
			}
		}
		else if (_state == StateWait)
		{
			if (IsEndWait())
			{
				n = _steps;
			}
		}
		else
		{
			if (_state <= StateRun)
			{
				if (n >= _pod._move._ramp._downStartAt)
				{
					mvState->_rest = 0;
					_state         = _pod._move._ramp._timerStop > mvState->_timer ? StateDownDec : StateDownAcc;
				}
			}

			switch (_state)
			{
				case StateUpAcc:
				{
					if (mvState->CalcTimerAcc(_pod._move._ramp._timerRun, n + _pod._move._ramp._nUpOffset, count))
					{
						_state = StateRun;
					}
					break;
				}

				case StateUpDec:
				{
					if (mvState->CalcTimerDec(_pod._move._ramp._timerRun, _pod._move._ramp._nUpOffset - n, count))
					{
						_state = StateRun;
					}
					break;
				}

				case StateDownDec:
				{
					mvState->CalcTimerDec(_pod._move._ramp._timerStop, _steps - n + _pod._move._ramp._nDownOffset, count);
					break;
				}

				case StateDownAcc:
				{
					mvState->CalcTimerAcc(_pod._move._ramp._timerStop, _pod._move._ramp._nDownOffset - (_steps - n - 1), count);
					break;
				}
				default: break;
			}
		}

		timer_t t = mvState->_timer * count;

#ifndef REDUCED_SIZE
		if (stepper->GetSpeedOverride() != CStepper::SpeedOverride100P)
		{
			// slower => increase timer
			uint32_t tl = RoundMulDivU32(t, CStepper::SpeedOverride100P, stepper->GetSpeedOverride());
			if (tl >= TIMER1MAX)
			{
				t = TIMER1MAX; // to slow
			}
			else if (tl < TIMER1MIN)
			{
				t = TIMER1MIN; // to fast
			}
			else
			{
				t = timer_t(tl);
			}
		}

		mvState->_sumTimer += t;
#endif

		stepper->_stepBuffer.NextTail().Timer = t;

		n += count;
		if (count > n)
		{
			// overrun
			n = _steps;
		}
		mvState->_n = n;

#ifdef _MSC_VER
		{
			SStepBuffer& stepBuffer = stepper->_stepBuffer.NextTail();
			memcpy(stepBuffer._distance, _distance_, sizeof(stepBuffer._distance));
			stepBuffer._steps = _steps;
			stepBuffer._state = _state;
			stepBuffer._n     = mvState->_n;
			stepBuffer._count = count;
			strcpy_s(stepBuffer._spMSCInfo, _mvMSCInfo);
		}
#endif

		stepper->_stepBuffer.Enqueue();
	}
	while (continues);

	return true;
}

////////////////////////////////////////////////////////

void CStepper::QueueAndSplitStep(const udist_t dist[NUM_AXIS], const bool directionUp[NUM_AXIS], steprate_t vMax)
{
	_pod._error = nullptr;
	axis_t i;

#if USESLIP
	signed char slip;
#endif

	for (i = 0; i < NUM_AXIS; i++)
	{
		int32_t newC = CalcNextPos(_pod._calculatedPos[i], dist[i], directionUp[i]);
		if (_pod._limitCheck)
		{
			// check limit
			if (newC > int32_t(GetLimitMax(i)) || newC < int32_t(GetLimitMin(i)))
			{
				Error(MESSAGE(MESSAGE_STEPPER_RangeLimit));
				//				StepperSerial.print(F("Error: range limit")); StepperSerial.print(_limitMin[i]); StepperSerial.print(F("<")); StepperSerial.print(newC);; StepperSerial.print(F("<")); StepperSerial.print(_limitMax[i]);
				return;
			}
		}

#if USESLIP
		// calc slip
		if (_refMove == SMNoRefMove)
		{
			if ((_slip[i]>0 && d[i]>0) || (_slip[i]<0 && d[i]<0))
			{
				_slipSum[i] += abs(d[i]);
				slip = (int)_slipSum[i] / _slip[i];
				if (slip)
				{
					_slipSum[i] -= slip*_slip[i];
					d[i] += slip;
					_currentpos[i] -= slip;
					_StepOffset[i] = (_StepOffset[i] + slip) & 0xf;
				}
			}
		}
#endif
	}

	// now move must not fail => we can calculate next position

	for (i = 0; i < NUM_AXIS; i++)
	{
		_pod._calculatedPos[i] = CalcNextPos(_pod._calculatedPos[i], dist[i], directionUp[i]);
	}

	uint8_t stepMult = 1;

	timer_t timerMax = vMax == 0 ? _pod._timerMaxDefault : SpeedToTimer(vMax);

	while (timerMax == timer_t(-1))
	{
		stepMult++;
		timerMax = SpeedToTimer(vMax * stepMult);
	}

	if (timerMax < _pod._timerMaxDefault)
		timerMax = _pod._timerMaxDefault;

	mdist_t d[NUM_AXIS];
	udist_t steps = 0;

	for (i = 0; i < NUM_AXIS; i++)
	{
		if (dist[i] > steps)
		{
			steps = dist[i];
		}
	}

#ifndef REDUCED_SIZE
	_pod._totalSteps += steps;
#endif

	uint16_t moveCount     = 1;
	udist_t  pos[NUM_AXIS] = { 0 };

	steps *= stepMult;

	if (steps > MAXSTEPSPERMOVE)
	{
		moveCount = uint16_t(steps / MAXSTEPSPERMOVE);
		if ((steps % MAXSTEPSPERMOVE) != 0)
		{
			moveCount++;
		}
	}

	for (uint16_t j = 1; j < moveCount; j++)
	{
		for (i = 0; i < NUM_AXIS; i++)
		{
			udist_t nextPos = RoundMulDivU32(dist[i], j, moveCount);

			d[i]   = mdist_t(nextPos - pos[i]);
			pos[i] = nextPos;
		}

		QueueMove(d, directionUp, timerMax, stepMult);
		if (IsError())
		{
			return;
		}
	}

	for (i = 0; i < NUM_AXIS; i++)
	{
		d[i] = mdist_t(dist[i] - pos[i]);
	}

	QueueMove(d, directionUp, timerMax, stepMult);
}

////////////////////////////////////////////////////////

bool CStepper::MoveUntil(TestContinueMove testContinue, uintptr_t param)
{
	while (IsBusy())
	{
		if (!testContinue(param))
		{
			AbortMove();
			return true;
		}
		OnWait(WaitReference);
	}
	return false;
}

////////////////////////////////////////////////////////

bool CStepper::MoveUntil(uint8_t referenceId, bool referenceValue, uint16_t stableTime)
{
	uint32_t time = 0;

	while (IsBusy())
	{
		if (IsReferenceTest(referenceId) == referenceValue)
		{
			if (time == 0)
			{
				time = millis() + stableTime; // allow stableTime == 0
			}
			if (millis() >= time)
			{
				AbortMove();
				return true;
			}
		}
		else
		{
			time = 0;
		}
		OnWait(WaitReference);
	}
	return false;
}

////////////////////////////////////////////////////////

bool CStepper::MoveAwayFromReference(axis_t axis, uint8_t referenceId, sdist_t dist, steprate_t vMax)
{
	if (IsReferenceTest(referenceId))
	{
		Info(MESSAGE_STEPPER_IsReferenceIsOn);
		CPushValue<bool> OldCheckForReference(&_pod._checkReference, false);
		MoveAwayFromReference(axis, dist, vMax);

		if (!MoveUntil(referenceId, false, REFERENCESTABLETIME))
		{
			return false;
		}
	}

	return !IsReferenceTest(referenceId);
}

////////////////////////////////////////////////////////

bool CStepper::MoveReference(axis_t axis, uint8_t referenceId, bool toMin, steprate_t vMax, sdist_t maxDist, sdist_t distToRef, sdist_t distIfRefIsOn)
{
	WaitBusy();

	CPushValue<bool>    OldLimitCheck(&_pod._limitCheck, false);
	CPushValue<bool>    OldWaitFinishMove(&_pod._waitFinishMove, false);
	CPushValue<bool>    OldCheckForReference(&_pod._checkReference, false);
	CPushValue<timer_t> OldBacklashEnabled(&_pod._timerBacklash, (timer_t(-1)));

	if (vMax == 0)
	{
		vMax = TimerToSpeed(_pod._timerMaxDefault);
	}
#ifdef use16bit
	if (maxDist == 0)
	{
		maxDist = min(GetLimitSize(axis), 0xfffel * MOVEMENTBUFFERSIZE);	// do not queue
	}
#else
	if (maxDist == 0)
	{
		maxDist = (GetLimitSize(axis) * 11) / 10; // add 10%
	}
#endif

	if (distIfRefIsOn == 0)
	{
		distIfRefIsOn = maxDist / 8;
	}

	// check direction of move (assume to min)
	if (maxDist > 0)
	{
		maxDist = -maxDist;
	}
	if (distToRef < 0)
	{
		distToRef = -distToRef;
	}
	if (distIfRefIsOn < 0)
	{
		distIfRefIsOn = -distIfRefIsOn;
	}

	if (!toMin)
	{
		// move to max
		distToRef     = -distToRef;
		distIfRefIsOn = -distIfRefIsOn;
		maxDist       = -maxDist;
	}

	bool ret = false;

	if (MoveAwayFromReference(axis, referenceId, distIfRefIsOn, vMax))
	{
		// move to reference
		MoveRel(axis, maxDist, vMax);
		if (MoveUntil(referenceId, true, REFERENCESTABLETIME))
		{
			// ref reached => move away
			MoveRel(axis, distIfRefIsOn, vMax);
			if (MoveUntil(referenceId, false, REFERENCESTABLETIME))
			{
				// move distToRef from change
				if (distToRef)
				{
					MoveRel(axis, distToRef, vMax);
					WaitBusy();
				}
				ret = true;
			}
		}
	}
	else
	{
		Error(MESSAGE(MESSAGE_STEPPER_MoveReferenceFailed));
	}

	// calling this method always sets position, independent of the result!!!!
	SetPosition(axis, toMin ? GetLimitMin(axis) : GetLimitMax(axis));

	return ret;
}

////////////////////////////////////////////////////////

bool CStepper::IsAnyReference()
{
	// slow version of IsAnyReference => override and do not call base

	for (axis_t ref = 0; ref < NUM_REFERENCE; ref++)
	{
		if (_pod._referenceHitValue[ref] == GetReferenceValue(ref))
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////

void CStepper::Wait(unsigned int sec100)
{
	QueueWait(mdist_t(sec100), WAITTIMER1VALUE, 0, false);
}

////////////////////////////////////////////////////////

void CStepper::WaitConditional(unsigned int sec100)
{
	QueueWait(mdist_t(sec100), WAITTIMER1VALUE, 0, true);
}

////////////////////////////////////////////////////////

void CStepper::WaitClock(uint32_t clock)
{
	QueueWait(0xffff, WAITTIMER1VALUE, clock, false);
}

////////////////////////////////////////////////////////

void CStepper::IoControl(uint8_t tool, uint16_t level)
{
	WaitUntilCanQueue();
	_movements._queue.NextTail().InitIoControl(this, tool, level);

	EnqueueAndStartTimer(true);
}

////////////////////////////////////////////////////////

void CStepper::MoveAbs(const udist_t d[NUM_AXIS], steprate_t vMax)
{
	udist_t dist[NUM_AXIS];
	bool    directionUp[NUM_AXIS];

	for (axis_t i = 0; i < NUM_AXIS; ++i)
	{
		directionUp[i] = d[i] >= _pod._calculatedPos[i];
		if (directionUp[i])
		{
			dist[i] = d[i] - _pod._calculatedPos[i];
		}
		else
		{
			dist[i] = _pod._calculatedPos[i] - d[i];
		}
	}
	QueueAndSplitStep(dist, directionUp, vMax);
}

////////////////////////////////////////////////////////

void CStepper::MoveRel(const sdist_t d[NUM_AXIS], steprate_t vMax)
{
	udist_t dist[NUM_AXIS];
	bool    directionUp[NUM_AXIS];

	for (axis_t i = 0; i < NUM_AXIS; ++i)
	{
		directionUp[i] = d[i] >= 0;
		dist[i]        = abs(d[i]);
	}
	QueueAndSplitStep(dist, directionUp, vMax);
}

////////////////////////////////////////////////////////

void CStepper::MoveAbs(axis_t axis, udist_t d, steprate_t vMax)
{
	udist_t D[NUM_AXIS] = { 0 };
	memcpy(D, _pod._calculatedPos, sizeof(_pod._calculatedPos));
	D[axis] = d;
	MoveAbs(D, vMax);
}

////////////////////////////////////////////////////////

void CStepper::MoveRel(axis_t axis, sdist_t d, steprate_t vMax)
{
	udist_t dist[NUM_AXIS]        = { 0 };
	bool    directionUp[NUM_AXIS] = { false };
	dist[axis]                    = abs(d);
	directionUp[axis]             = d > 0;
	QueueAndSplitStep(dist, directionUp, vMax);
}

////////////////////////////////////////////////////////
// repeat axis and d until axis not in 0 .. NUM_AXIS

void CStepper::MoveAbsEx(steprate_t vMax, uint16_t axis, udist_t d, ...)
{
	udist_t D[NUM_AXIS] = { 0 };
	memcpy(D, _pod._calculatedPos, sizeof(_pod._calculatedPos));

	va_list arglist;
	va_start(arglist, d);

	while (axis < NUM_AXIS)
	{
		D[axis] = d;

#ifdef _MSC_VER
		axis = va_arg(arglist, uint16_t);
		d    = va_arg(arglist, udist_t);
#else
		axis = va_arg(arglist, unsigned int);		// only "int" supported on arduino
		d    = va_arg(arglist, uint32_t);	
#endif
	}

	va_end(arglist);

	MoveAbs(D, vMax);
}

////////////////////////////////////////////////////////
// repeat axis and d until axis not in 0 .. NUM_AXIS

void CStepper::MoveRelEx(steprate_t vMax, uint16_t axis, sdist_t d, ...)
{
	sdist_t dist[NUM_AXIS] = { 0 };

	va_list arglist;
	va_start(arglist, d);

	while (axis < NUM_AXIS)
	{
		dist[axis] = d;

#ifdef _MSC_VER
		axis = va_arg(arglist, uint16_t);
		d    = va_arg(arglist, sdist_t);
#else
		axis = va_arg(arglist, unsigned int);		// only "int" supported on arduino
		d    = va_arg(arglist, uint32_t);	
#endif
	}

	va_end(arglist);

	MoveRel(dist, vMax);
}

////////////////////////////////////////////////////////

void CStepper::SetPosition(axis_t axis, udist_t pos)
{
	WaitBusy();
#ifdef USESLIP
	_slipSum[axis] = 0;
#endif
	_pod._current[axis]       = pos;
	_pod._calculatedPos[axis] = pos;
}

////////////////////////////////////////////////////////

#ifdef USESLIP

void CStepper::SetSlip(int d[NUM_AXIS])
{
	memcpy(_slip,d,sizeof(d[0])*NUM_AXIS);
}

#endif

////////////////////////////////////////////////////////

timer_t CStepper::SpeedToTimer(steprate_t speed) const
{
	if (speed == 0)
	{
		return timer_t(-1);
	}

	uint32_t timer = TIMER1FREQUENCE / speed;
	if (timer > (timer_t(-1)))
	{
		return timer_t(-1);
	}

	return timer_t(timer);
}

////////////////////////////////////////////////////////

steprate_t CStepper::TimerToSpeed(timer_t timer) const
{
	return SpeedToTimer(timer);
}

////////////////////////////////////////////////////////

bool CStepper::SetEnableSafe(axis_t i, uint8_t level, bool force)
{
	CCriticalRegion criticalRegion;
	if (GetEnable(i) != level)
	{
		SetEnable(i, level, force);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////

//static void DumpTypeBool(FLSTR head, bool value, bool newline)	{ DumpType<bool>(head, value, newline); }
//void DumpArray_udist_t(FLSTR head, const udist_t pos[NUM_AXIS], bool newline) { DumpArray<udist_t, NUM_AXIS>(head,pos,newline); }

void CStepper::Dump(uint8_t options)
{
#ifdef _NO_DUMP
	(void) options;
#else
	uint8_t i;

	if (options & DumpPos)
	{
		DumpArray<udist_t, NUM_AXIS>(F("pos"), _pod._current, false);
		DumpArray<udist_t, NUM_AXIS>(F("cur"), _pod._calculatedPos, !(options & DumpState));
	}

	if (options & DumpState)
	{
		for (i = 0; i < sizeof(_pod._referenceHitValue); i++)
		{
			StepperSerial.print(i == 0 ? F("ES") : F(":ES"));
			StepperSerial.print(i);
			StepperSerial.print(F("="));
			StepperSerial.print(GetReferenceValue(i));
		}
		StepperSerial.print(F(":ANY="));
		StepperSerial.print(IsAnyReference());
		DumpArray<uint8_t, NUM_AXIS * 2>(F(":UseReference"), _pod._referenceHitValue, false);

		for (i = 0; i < NUM_AXIS; i++)
		{
			StepperSerial.print(i == 0 ? F("L") : F(":L"));
			StepperSerial.print(i);
			StepperSerial.print(F("="));
			StepperSerial.print(int(GetEnable(i)));
		}
		StepperSerial.println();

		DumpType<uint32_t>(F("TotalSteps"), _pod._totalSteps, false);
		DumpType<unsigned int>(F("TimerISRBusy"), _pod._timerISRBusy, false);

		DumpType<bool>(F("TimerRunning"), _pod._timerRunning, false);
		DumpType<bool>(F("CheckReference"), _pod._checkReference, false);
		DumpType<bool>(F("WaitFinishMove"), _pod._waitFinishMove, false);

		DumpType<bool>(F("LimitCheck"), _pod._limitCheck, false);
		DumpArray<udist_t, NUM_AXIS>(F("Min"), _pod._limitMin, false);
		DumpArray<udist_t, NUM_AXIS>(F("Max"), _pod._limitMax, false);

		//		DumpArray<EnumAsByte(EStepMode), NUM_AXIS>(F("StepMode"), _stepMode, true);

		DumpType<timer_t>(F("TimerMaxDefault"), _pod._timerMaxDefault, false);

		DumpArray<steprate_t, NUM_AXIS>(F("MaxJerkSpeed"), _pod._maxJerkSpeed, false);
		DumpArray<steprate_t, NUM_AXIS>(F("TimerMax"), _pod._timerMax, false);
		DumpArray<steprate_t, NUM_AXIS>(F("TimerAcc"), _pod._timerAcc, false);
		DumpArray<steprate_t, NUM_AXIS>(F("TimerDec"), _pod._timerDec, true);
	}

	if (options & DumpMovements)
	{
		uint8_t idxNoChange = _movements._queue.H2TInit();

		i = 0;
		for (uint8_t idx = idxNoChange; _movements._queue.H2TTest(idx); idx = _movements._queue.H2TInc(idx))
		{
			_movements._queue.Buffer[idx].Dump(i++, options);
		}
	}
#endif
}

////////////////////////////////////////////////////////

void CStepper::SMovement::Dump(uint8_t idx, uint8_t options)
{
#ifdef _NO_DUMP
	(void) idx; (void) options;
#else
	DumpType<uint8_t>(F("Idx"), idx, false);
	if (idx == 0)
	{
		_stepper->_movementState.Dump(options);
	}
	DumpType<udist_t>(F("Steps"), _steps, false);
	DumpType<udist_t>(F("State"), _state, false);

	if (IsActiveWait())
	{
		DumpType<mdist_t>(F("Conditional"), _pod._wait._checkWaitConditional, false);
		DumpType<mdist_t>(F("EndTime"), _pod._wait._endTime, false);
	}
	else if (IsActiveIo())
	{
		DumpType<mdist_t>(F("tool"), _pod._io._tool, false);
		DumpType<mdist_t>(F("level"), _pod._io._level, false);
	}
	else
	{
		DumpType<DirCount_t>(F("DirCount"), _dirCount, false);
		DumpType<DirCount_t>(F("LastDirCount"), _lastStepDirCount, false);
		DumpArray<mdist_t, NUM_AXIS>(F("Dist"), _distance_, false);

		DumpType<mdist_t>(F("UpSteps"), _pod._move._ramp._upSteps, false);
		DumpType<mdist_t>(F("DownSteps"), _pod._move._ramp._downSteps, false);
		DumpType<mdist_t>(F("DownStartAt"), _pod._move._ramp._downStartAt, false);
		DumpType<mdist_t>(F("UpOffset"), _pod._move._ramp._nUpOffset, false);
		DumpType<mdist_t>(F("DownOffset"), _pod._move._ramp._nDownOffset, false);

		DumpType<timer_t>(F("tMax"), _pod._move._timerMax, false);
		DumpType<timer_t>(F("tRun"), _pod._move._ramp._timerRun, false);
		DumpType<timer_t>(F("tStart"), _pod._move._ramp._timerStart, false);
		DumpType<timer_t>(F("tStop"), _pod._move._ramp._timerStop, false);
		DumpType<timer_t>(F("tEndPossible"), _pod._move._timerEndPossible, false);
		DumpType<timer_t>(F("tJunctionToPrev"), _pod._move._timerJunctionToPrev, false);
		DumpType<timer_t>(F("tMaxJunction"), _pod._move._timerMaxJunction, false);

		if (options & DumpDetails)
		{
			DumpType<timer_t>(F("TimerAcc"), _pod._move._timerAcc, false);
			DumpType<timer_t>(F("TimerDec"), _pod._move._timerDec, false);
		}
	}

	StepperSerial.println();
#endif
}

////////////////////////////////////////////////////////

void CStepper::SMovementState::Dump(uint8_t /* options */)
{
#ifndef _NO_DUMP
	DumpType<mdist_t>(F("n"), _n, false);
	DumpType<timer_t>(F("t"), _timer, false);
	DumpType<timer_t>(F("r"), _rest, false);
	DumpType<uint32_t>(F("sum"), _sumTimer, false);
	DumpArray<mdist_t, NUM_AXIS>(F("a"), _add, false);
#endif
}
