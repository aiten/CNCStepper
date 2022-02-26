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

#include "ConfigurationStepperLib.h"
#include "HAL.h"
#include "RingBuffer.h"
#include "Singleton.h"
#include "UtilitiesStepperLib.h"

////////////////////////////////////////////////////////
//
// assume step <==> mm
// a  ... acceleration
// d  ... distance in steps 
// v  ... velocity in steps/sec
// v0 ... velocity at start <=> a
// F  ... TimerFrequency in Hz (Arduino 1/8 CPU => 2MHZ)
// c  ... timer value (for step) in multiply of timerFrequency (NOT sec)
// v  ... (1 / F) * c
//
////////////////////////////////////////////////////////

class CStepper : public CSingleton<CStepper>
{
public:
	/////////////////////

	enum EWaitType
	{
		MovementQueueFull,
		WaitBusyCall,

		WaitTimeCritical,										// TimeCritical wait types start here
		WaitReference
	};

	enum EStepperEvent
	{
		OnStartEvent,											// Stepper, Io and Wait
		OnIdleEvent,
		OnDisableEvent,											// Disable stepper if inactive
		OnWaitEvent,
		OnErrorEvent,
		OnWarningEvent,
		OnInfoEvent,
		OnIoEvent,

		LastStepperEvent = OnIoEvent
	};

#define LevelToPercent(a) ((a)*100/255)
#define PercentToLevel(a) ((a)*255/100)

	enum ELevel
	{
		LevelMax=255,
		Level20P = PercentToLevel(20),
		Level60P = PercentToLevel(60),
		LevelOff = 0
	};

	enum ESpeedOverride
	{
		SpeedOverrideMax = 255,
		SpeedOverride100P = 128,
		SpeedOverrideMin = 1
	};

	enum EDumpOptions		// use bit
	{
		DumpAll = 0xff,
		DumpPos = 1,
		DumpState = 2,
		DumpMovements = 8,
		DumpDetails = 128									// detail of each option
	};

	typedef bool (*StepperEvent)(CStepper*     stepper, uintptr_t param, EnumAsByte(EStepperEvent) eventType, uintptr_t addInfo);
	typedef bool (*TestContinueMove)(uintptr_t param);

	struct SEvent
	{
		SEvent()
		{
			_event      = nullptr;
			_eventParam = 0;
		}

		StepperEvent _event;
		uintptr_t    _eventParam;

		bool Call(CStepper* stepper, EnumAsByte(CStepper::EStepperEvent) eventType, uintptr_t addInfo) const
		{
			if (_event)
			{
				return _event(stepper, _eventParam, eventType, addInfo);
			}
			return true;
		}
	};

	struct SIoControl
	{
		uint8_t  _tool;
		uint16_t _level;
	};

	/////////////////////

protected:

	CStepper();

public:

	virtual void Init();

	bool    IsError() const { return _pod._error != nullptr; };
	error_t GetError() const { return _pod._error; }
	void    ClearError() { _pod._error = nullptr; }

	bool    IsFatalError() const { return _pod._fatalError != nullptr; };
	error_t GetFatalError() const { return _pod._fatalError; }
	void    ClearFatalError() { _pod._fatalError = nullptr; }


	void SetMaxSpeed(axis_t axis, steprate_t vMax) { _pod._timerMax[axis]  = SpeedToTimer(vMax); }
	void SetAcc(axis_t      axis, steprate_t v0Acc) { _pod._timerAcc[axis] = SpeedToTimer(v0Acc); }
	void SetDec(axis_t      axis, steprate_t v0Dec) { _pod._timerDec[axis] = SpeedToTimer(v0Dec); }

	void SetAccDec(axis_t axis, steprate_t v0Acc, steprate_t v0Dec)
	{
		SetAcc(axis, v0Acc);
		SetDec(axis, v0Dec);
	}

	void SetDefaultMaxSpeed(steprate_t vMax) { _pod._timerMaxDefault = SpeedToTimer(vMax); }

	void SetDefaultMaxSpeed(steprate_t vMax, steprate_t v0Acc, steprate_t v0Dec)
	{
		SetDefaultMaxSpeed(vMax);
		for (axis_t i = 0; i < NUM_AXIS; i++)
		{
			SetAcc(i, v0Acc);
			SetDec(i, v0Dec);
		}
	}

	void SetDefaultMaxSpeed(steprate_t vMax, axis_t axis, steprate_t v0Acc, steprate_t v0Dec)
	{
		SetDefaultMaxSpeed(vMax);
		SetAccDec(axis, v0Acc, v0Dec);
	}

	void SetDefaultMaxSpeed(steprate_t vMax, steprate_t v0Acc, steprate_t v0Dec, steprate_t vMaxJerk)
	{
		SetDefaultMaxSpeed(vMax);
		for (axis_t i = 0; i < NUM_AXIS; i++)
		{
			SetAcc(i, v0Acc);
			SetDec(i, v0Dec);
			SetJerkSpeed(i, vMaxJerk);
		}
	}

	void                       SetSpeedOverride(EnumAsByte(ESpeedOverride) speed) { _pod._speedOverride = speed; }
	EnumAsByte(ESpeedOverride) GetSpeedOverride() const { return _pod._speedOverride; }

	static uint8_t                    SpeedOverrideToP(EnumAsByte(ESpeedOverride) speed) { return RoundMulDivU8(uint8_t(speed), 100, SpeedOverride100P); }
	static EnumAsByte(ESpeedOverride) PToSpeedOverride(uint8_t                    speedP) { return EnumAsByte(ESpeedOverride)(RoundMulDivU8(speedP, SpeedOverride100P, 100)); }

	void SetUsual(steprate_t vMax);

	void SetJerkSpeed(axis_t axis, steprate_t vMaxJerk) { _pod._maxJerkSpeed[axis] = vMaxJerk; }

	void SetWaitFinishMove(bool wait) { _pod._waitFinishMove = wait; };
	bool IsWaitFinishMove() const { return _pod._waitFinishMove; }

	void SetCheckForReference(bool check) { _pod._checkReference = check; };
	bool IsCheckForReference() const { return _pod._checkReference; }

	void       SetBacklash(steprate_t speed) { _pod._timerBacklash = SpeedToTimer(speed); };
	bool       IsSetBacklash() const { return timer_t(-1) != _pod._timerBacklash; }
	steprate_t GetBacklash() const { return TimerToSpeed(_pod._timerBacklash); };

	bool IsBusy() const { return _pod._timerRunning; };
	void WaitBusy();

	bool    CanQueueMovement() const { return !_movements._queue.IsFull(); }
	uint8_t QueuedMovements() const { return _movements._queue.Count(); }

	uint16_t GetEnableTimeout() const { return _pod._timeOutEnableAll; }
	void    SetEnableTimeout(uint16_t sec) { _pod._timeOutEnableAll = sec; }

#ifndef REDUCED_SIZE
	uint8_t GetEnableTimeout(axis_t axis) const { return _pod._timeOutEnable[axis]; }
	void    SetEnableTimeout(axis_t axis, uint8_t sec) { _pod._timeOutEnable[axis] = sec; }
#endif

	void SetDirection(axisArray_t direction) { _pod._invertDirection = direction; }

#ifdef USESLIP
	void SetSlip(int dist[NUM_AXIS]);
#endif

	void SetLimitMax(axis_t axis, udist_t limit) { _pod._limitMax[axis] = limit; };
#ifndef REDUCED_SIZE
	void SetLimitMin(axis_t axis, udist_t limit) { _pod._limitMin[axis] = limit; };
#endif

	void SetBacklash(axis_t axis, mdist_t dist) { _pod._backlash[axis] = dist; }

	void StopMove(steprate_t v0Dec = 0);						// Stop all pendinge/current moves, WITH dec ramp, clear buffer
	void AbortMove();											// Abort all pendinge/current moves, NO dec ramp, clear buffer

	void PauseMove();											// Finish current move but do not continue with next (WITH dec ramp)
	void ContinueMove();										// continue after pause
	bool IsPauseMove() const { return _pod._pause; }

	void EmergencyStop()
	{
		_pod._emergencyStop = true;
		AbortMove();
	}

	bool IsEmergencyStop() const { return _pod._emergencyStop; }
	void EmergencyStopResurrect();

	bool IsWaitConditional() const { return _pod._isWaitConditional; }
	void SetWaitConditional(bool conditionalWait) { _pod._isWaitConditional = conditionalWait; }

	void SetReferenceHitValue(uint8_t referenceId, uint8_t valueHit) { _pod._referenceHitValue[referenceId] = valueHit; }
	bool IsUseReference(uint8_t       referenceId) const { return _pod._referenceHitValue[referenceId] != 255; }
	bool IsUseReference(axis_t        axis, bool toMin) const { return IsUseReference(ToReferenceId(axis, toMin)); }

	debugvirtual bool MoveReference(axis_t axis, uint8_t referenceId, bool toMin, steprate_t vMax, sdist_t maxDist = 0, sdist_t distToRef = 0, sdist_t distIfRefIsOn = 0);
	void              SetPosition(axis_t   axis, udist_t pos);

	//////////////////////////////

	void MoveAbs(const udist_t d[NUM_AXIS], steprate_t vMax = 0);
	void MoveRel(const sdist_t d[NUM_AXIS], steprate_t vMax = 0);

	void MoveAbs(axis_t axis, udist_t d, steprate_t vMax = 0);
	void MoveRel(axis_t axis, sdist_t d, steprate_t vMax = 0);

	void MoveAbsEx(steprate_t         vMax, uint16_t axis, udist_t d, ...);	// repeat axis and d until axis not in 0 .. NUM_AXIS-1
	void MoveRelEx(steprate_t         vMax, uint16_t axis, sdist_t d, ...);	// repeat axis and d until axis not in 0 .. NUM_AXIS-1
	void Wait(unsigned int            sec100);								// unconditional wait
	void WaitConditional(unsigned int sec100);								// conditional wait 
	void IoControl(uint8_t            tool, uint16_t level);

	bool MoveUntil(TestContinueMove testContinue, uintptr_t param);

	//////////////////////////////

	const udist_t* GetPositions() const { return _pod._calculatedPos; }
	void           GetPositions(udist_t pos[NUM_AXIS]) const { memcpy(pos, _pod._calculatedPos, sizeof(_pod._calculatedPos)); }
	udist_t        GetPosition(axis_t   axis) const { return _pod._calculatedPos[axis]; }

	void GetCurrentPositions(udist_t pos[NUM_AXIS]) const
	{
		CCriticalRegion criticalRegion;
		memcpy(pos, _pod._current, sizeof(_pod._current));
	}

	udist_t GetCurrentPosition(axis_t axis) const
	{
		CCriticalRegion criticalRegion;
		return (*const_cast<volatile udist_t*>(&_pod._current[axis]));
	}

	udist_t GetLimitMax(axis_t axis) const { return _pod._limitMax[axis]; }
#ifdef REDUCED_SIZE
	udist_t GetLimitMin(axis_t ) const							{ return 0; }
#else
	udist_t GetLimitMin(axis_t axis) const { return _pod._limitMin[axis]; }
#endif
	udist_t GetLimitSize(axis_t axis) const { return GetLimitMax(axis)- GetLimitMin(axis); }

	mdist_t     GetBacklash(axis_t axis) const { return _pod._backlash[axis]; }
	axisArray_t GetLastDirection() const { return _pod._lastDirection; }		// check for backlash

	steprate_t GetDefaultVmax() const { return TimerToSpeed(_pod._timerMaxDefault); }
	steprate_t GetMaxSpeed(axis_t  axis) const { return min(GetDefaultVmax(),TimerToSpeed(_pod._timerMax[axis])); }
	steprate_t GetAcc(axis_t       axis) const { return TimerToSpeed(_pod._timerAcc[axis]); }
	steprate_t GetDec(axis_t       axis) const { return TimerToSpeed(_pod._timerDec[axis]); }
	steprate_t GetJerkSpeed(axis_t axis) const { return _pod._maxJerkSpeed[axis]; }

#ifndef REDUCED_SIZE
	uint32_t     GetTotalSteps() const { return _pod._totalSteps; }
	unsigned int GetTimerISRBuys() const { return _pod._timerISRBusy; }
#endif
	uint32_t IdleTime() const { return _pod._timerStartOrOnIdle; }

	void AddEvent(StepperEvent event, uintptr_t eventParam, SEvent& oldEvent);

	static uint8_t ToReferenceId(axis_t axis, bool minRef) { return axis * 2 + (minRef ? 0 : 1); }

	virtual bool    IsAnyReference() = 0;
	virtual uint8_t GetReferenceValue(uint8_t referenceId) = 0;
	bool            IsReferenceTest(uint8_t   referenceId) { return GetReferenceValue(referenceId) == _pod._referenceHitValue[referenceId]; }

	virtual void    SetEnable(axis_t     axis, uint8_t level, bool force) = 0;
	virtual uint8_t GetEnable(axis_t     axis) = 0;

	static uint8_t ConvertLevel(bool enable) { return enable ? uint8_t(LevelMax) : uint8_t(LevelOff); }

	void Dump(uint8_t options);							// options ==> EDumpOptions with bits

	////////////////////////////////////////////////////////

private:

	bool SetEnableSafe(axis_t i, uint8_t level, bool force);

	void QueueMove(const mdist_t dist[NUM_AXIS], const bool directionUp[NUM_AXIS], timer_t timerMax, uint8_t stepMult);
	void QueueWait(const mdist_t dist, timer_t              timerMax, bool                 checkWaitConditional);

	void EnqueueAndStartTimer(bool waitFinish);
	void WaitUntilCanQueue();
	bool StartMovement();

	static int32_t CalcNextPos(udist_t current, udist_t dist, bool directionUp)
	{
		if (directionUp)
		{
			return sdist_t(current) + sdist_t(dist);
		}
		return sdist_t(current) - sdist_t(dist);
	}

	inline void StepOut();
	inline void StartBackground();
	inline void FillStepBuffer();
	void        Background();

	void GoIdle();
	void ContinueIdle();

	void CallEvent(EnumAsByte(EStepperEvent) eventType, uintptr_t addInfo = 0) { _event.Call(this, eventType, addInfo); }

	void SubTotalSteps();

protected:

	bool MoveUntil(uint8_t referenceId, bool referenceValue, uint16_t stableTime);

	void QueueAndSplitStep(const udist_t dist[NUM_AXIS], const bool directionUp[NUM_AXIS], steprate_t vMax);

	debugvirtual void StepRequest(bool           isr);
	debugvirtual void OptimizeMovementQueue(bool force);

	////////////////////////////////////////////////////////

	timer_t GetTimer(mdist_t             steps, timer_t timerStart);										// calc "speed" after steps with constant a (from v0 = 0)
	timer_t GetTimerAccelerating(mdist_t steps, timer_t timerV0, timer_t timerStart);			// calc "speed" after steps accelerating with constant a 
	timer_t GetTimerDecelerating(mdist_t steps, timer_t timerV, timer_t  timerStart);			// calc "speed" after steps decelerating with constant a 

	static mdist_t GetAccSteps(timer_t timer, timer_t timerStart);										// from v=0
	static mdist_t GetDecSteps(timer_t timer, timer_t timerStop) { return GetAccSteps(timer, timerStop); } // to v=0

	static mdist_t GetAccSteps(timer_t timer1, timer_t timer2, timer_t timerStart);				// from v1 to v2 (v1<v2)
	static mdist_t GetDecSteps(timer_t timer1, timer_t timer2, timer_t timerStop) { return GetAccSteps(timer2, timer1, timerStop); }

	static mdist_t GetSteps(timer_t timer1, timer_t timer2, timer_t timerStart, timer_t timerStop);		// from v1 to v2 (v1<v2 uses acc, dec otherwise)

	uint32_t GetAccelerationFromTimer(mdist_t    timerV0);
	uint32_t GetAccelerationFromSpeed(steprate_t speedV0) { return GetAccelerationFromTimer(SpeedToTimer(speedV0)); }

	timer_t    SpeedToTimer(steprate_t speed) const;
	steprate_t TimerToSpeed(timer_t    timer) const;

	static uint8_t GetStepMultiplier(timer_t timerMax);

protected:

	//////////////////////////////////////////
	// often accessed members first => is faster
	// even size of struct and 2byte alignment

	struct POD												// POD .. Plane Old Data Type => no Constructor => init with default value = 0
	{
		volatile bool _timerRunning;
		bool          _checkReference;						// check for "IsReference" in ISR (while normal move)

		bool _emergencyStop;
		bool _isWaitConditional;							// wait on "Wait"

		bool _waitFinishMove;
		bool _limitCheck;

		timer_t _timerBacklash;								// -1 or 0 for temporary enable/disable backlash without setting _backlash to 0

#ifndef REDUCED_SIZE
		uint32_t     _totalSteps;							// total steps since start
		unsigned int _timerISRBusy;							// ISR while in ISR
#endif

		timer_t _timerMaxDefault;							// timerValue of vMax (if vMax = 0)

		udist_t _current[NUM_AXIS];							// update in ISR
		udist_t _calculatedPos[NUM_AXIS];					// calculated in advanced (use movement queue)

		uint8_t _referenceHitValue[NUM_REFERENCE];			// each axis min and max - used in ISR LOW,HIGH, 255(not used)

		steprate_t _maxJerkSpeed[NUM_AXIS];					// immediate change of speed without ramp (in junction)

		timer_t _timerMax[NUM_AXIS];						// maximum speed of axis
		timer_t _timerAcc[NUM_AXIS];						// acc timer start
		timer_t _timerDec[NUM_AXIS];						// dec timer start

#ifndef REDUCED_SIZE
		udist_t _limitMin[NUM_AXIS];
#endif
		udist_t _limitMax[NUM_AXIS];

		mdist_t _backlash[NUM_AXIS];						// backlash of each axis (signed mdist_t/2)

		uint32_t _timerStartOrOnIdle;						// timerValue if library start move or goes to Idle
#ifndef REDUCED_SIZE
		uint32_t _timerLastCheckEnable;						// timerValue
#endif

		uint8_t                             _idleLevel;											// level if idle (0..100)
		volatile EnumAsByte(ESpeedOverride) _speedOverride;		// Speed override, 128 => 100% (change in irq possible)

		axisArray_t _lastDirection;							// for backlash
		axisArray_t _invertDirection;						// invert direction

		error_t _error;
		error_t _fatalError;

#ifndef REDUCED_SIZE
		uint8_t _timeOutEnable[NUM_AXIS];					// enableTimeout in sec if no step (0.. disable, always enabled)
		uint8_t _timeEnable[NUM_AXIS];						// 0: active(during move), do not turn off, else time to turn off
#endif

		uint16_t _timeOutEnableAll;							// timeout for enable/disable all steppers (after move)

#ifdef USESLIP
		unsigned int _slipSum[NUM_AXIS];
		int _slip[NUM_AXIS];
#endif

		bool        _pause;									// PauseMove is called
		axisArray_t _lastDirectionUp;						// last parameter value of Step()
	}               _pod;

	SEvent _event ALIGN_WORD;								// no POS => Constructor
	axis_t _numAxes;										// actual axis (3 e.g. SMC800)

	struct SMovementState;

	/////////////////////////////////////////////////////////////////////
	// internal ringBuffer for movement optimization

	struct SMovement
	{
	protected:
		friend class CStepper;

		enum EMovementState
		{
			StateReadyMove = 1,							// ready for travel (not executing)
			StateReadyWait = 2,							// ready for none "travel" move (wait move) (not executing)
			StateReadyIo = 3,							// ready for none "travel" move (set io) (not executing)

			StateUpAcc = 11,							// in start phase accelerate
			StateUpDec = 12,							// in start phase decelerate to vMax
			StateRun = 13,								// running (no acc and dec)
			StateDownDec = 14,							// in stop phase decelerate
			StateDownAcc = 15,							// in stop phase accelerate

			StateWait = 21,								// executing wait (do no step)

			StateDone = 0,								// finished
		};

		mdist_t _steps;									// total movement steps (=distance)

		EnumAsByte(EMovementState) _state;				// enums are 16 bit in gcc => force byte
		bool                       _backlash;			// move is backlash

		DirCount_t _dirCount;
		DirCount_t _lastStepDirCount;

		mdist_t _distance_[NUM_AXIS];					// distance adjusted with stepMultiplier => use GetDistance(axis)

		struct SRamp									// only modify in CCriticalRegion
		{
			timer_t _timerStart;						// start ramp with speed (timerValue)
			timer_t _timerRun;
			timer_t _timerStop;							// stop  ramp with speed (timerValue)

			mdist_t _upSteps;							// steps needed for accelerating from v0
			mdist_t _downSteps;							// steps needed for decelerating to v0
			mdist_t _downStartAt;						// index of step to start with deceleration

			mdist_t _nUpOffset;							// offset of n ramp calculation(acc) 
			mdist_t _nDownOffset;						// offset of n ramp calculation(dec)

			void RampUp(SMovement*   movement, timer_t timerRun, timer_t timerJunction);
			void RampDown(SMovement* movement, timer_t timerJunction);
			void RampRun(SMovement*  movement);
		};

		union
		{
			struct SMove
			{
				timer_t _timerMax;								// timer for max requested speed
				timer_t _timerRun;								// copy of _ramp. => modify during ramp calc

				timer_t _timerEndPossible;						// timer possible at end of last movement
				timer_t _timerJunctionToPrev;					// used to calculate junction speed, stored in "next" step
				timer_t _timerMaxJunction;						// max possible junction speed, stored in "next" step

				struct SRamp _ramp;								// only modify in CCriticalRegion

				timer_t _timerAcc;								// timer for calc of acceleration while "up" state - depend on axis
				timer_t _timerDec;								// timer for calc of decelerating while "down" state - depend on axis
			}           _move;

			struct SWait
			{
				timer_t _timer;
				bool    _checkWaitConditional;					// wait only if Stepper.SetConditionalWait is set
			}           _wait;

			struct SIoControl _io;
		}                     _pod;

		stepperstatic CStepper* _stepper;						// give access to stepper (not static if multi instance)  

		timer_t GetUpTimerAcc() const { return _pod._move._timerAcc; }
		timer_t GetUpTimerDec() const { return _pod._move._timerDec; }

		timer_t GetDownTimerAcc() const { return _pod._move._timerAcc; }
		timer_t GetDownTimerDec() const { return _pod._move._timerDec; }

		timer_t GetUpTimer(bool   acc) const { return acc ? GetUpTimerAcc() : GetUpTimerDec(); }
		timer_t GetDownTimer(bool acc) const { return acc ? GetDownTimerAcc() : GetDownTimerDec(); }

		mdist_t GetDistance(axis_t       axis) const;
		uint8_t GetStepMultiplier(axis_t axis) const { return (_dirCount >> (axis * 4)) % 8; }
		bool    GetDirectionUp(axis_t    axis) const { return ((_dirCount >> (axis * 4)) & 8) != 0; }
		uint8_t GetMaxStepMultiplier() const;

		bool Ramp(SMovement* mvNext);

		void CalcMaxJunctionSpeed(SMovement* mvPrev);

		bool AdjustJunctionSpeedT2H(SMovement* mvPrev, SMovement* mvNext);
		void AdjustJunctionSpeedH2T(SMovement* mvPrev, SMovement* mvNext);

		bool CalcNextSteps(bool continues);

	private:

		bool IsEndWait() const;									// immediately end wait 

	public:

		mdist_t GetSteps() const { return _steps; }

		bool IsActiveIo() const { return _state == StateReadyIo; }								// Ready from Io
		bool IsActiveWait() const { return _state == StateReadyWait || _state == StateWait; }	// Ready from wait or waiting
		bool IsActiveMove() const { return IsReadyForMove() || IsProcessingMove(); }			// Ready from move or moving
		bool IsReadyForMove() const { return _state == StateReadyMove; }						// Ready for move but not started
		bool IsProcessingMove() const { return _state >= StateUpAcc && _state <= StateDownAcc; }// Move is currently processed (in acc,run or dec)
		bool IsUpMove() const { return IsProcessingMove() && _state < StateRun; }				// Move in ramp acc state
		bool IsRunOrUpMove() const { return IsProcessingMove() && _state <= StateRun; }			// Move in ramp run state
		bool IsRunMove() const { return _state == StateRun; }									// Move in ramp run state
		bool IsRunOrDownMove() const { return IsProcessingMove() && _state >= StateRun; }		// Move in ramp run state
		bool IsDownMove() const { return IsProcessingMove() && _state > StateRun; }				// Move in ramp dec state
		bool IsFinished() const { return _state == StateDone; }									// Move finished 

		bool IsSkipForOptimizing() const { return IsActiveIo(); }									// skip the entry when optimizing queue

		void InitMove(CStepper*      stepper, SMovement* mvPrev, mdist_t steps, const mdist_t dist[NUM_AXIS], const bool directionUp[NUM_AXIS], timer_t timerMax);
		void InitWait(CStepper*      stepper, mdist_t    steps, timer_t  timer, bool          checkWaitConditional);
		void InitIoControl(CStepper* stepper, uint8_t    tool, uint16_t  level);

		void InitStop(SMovement* mvPrev, timer_t timer, timer_t decTimer);

		void SetBacklash() { _backlash = true; }

		void Dump(uint8_t idx, uint8_t options);

#ifdef _MSC_VER
		char _mvMSCInfo[MOVEMENTINFOSIZE];
#endif
	};

	struct SMovements
	{
		timer_t                                         _timerStartPossible;					// timer for fastest possible start (break at the end)
		CRingBufferQueue<SMovement, MOVEMENTBUFFERSIZE> _queue;
	};

	stepperstatic struct SMovements _movements;

	/////////////////////////////////////////////////////////////////////
	// internal state of move (used in ISR)

	struct SMovementState
	{
	protected:
		friend class CStepper;

		// state for calculating steps (moving)
		// static for performance on arduino => only one instance allowed

		mdist_t _n;				// step within movement (1-_steps)
		timer_t _timer;			// current timer
		timer_t _rest;			// rest of ramp calculation

		uint8_t _count;			// increment of _n
		char    _dummyAlignment;

#ifndef REDUCED_SIZE
		uint32_t _sumTimer;		// for debug
#endif

		mdist_t _add[NUM_AXIS];

		void Init(SMovement* movement);

		bool CalcTimerAcc(timer_t maxTimer, mdist_t n, uint8_t cnt);
		bool CalcTimerDec(timer_t minTimer, mdist_t n, uint8_t cnt);

	public:

		void Dump(uint8_t options);
	};

	stepperstatic SMovementState _movementState;

	/////////////////////////////////////////////////////////////////////
	// internal ringBuffer for steps (after calculating acc and dec)

	struct SStepBuffer
	{
	public:
		DirCount_t DirStepCount;								// direction and count
		timer_t    Timer;
#ifdef _MSC_VER
		mdist_t                   _distance[NUM_AXIS];			// to calculate relative speed
		mdist_t                   _steps;
		SMovement::EMovementState _state;
		mdist_t                   _n;
		uint8_t                   _count;
		char                      _spMSCInfo[MOVEMENTINFOSIZE];
#endif
		void Init(DirCount_t dirCount)
		{
			Timer        = 0;
			DirStepCount = dirCount;
		};
	};

	stepperstatic CRingBufferQueue<SStepBuffer, STEPBUFFERSIZE> _steps;

public:

#ifdef _MSC_VER
	const char* MSCInfo;
#endif

	//////////////////////////////////////////

private:

	SMovement* GetNextMovement(uint8_t idx);
	SMovement* GetPrevMovement(uint8_t idx);


protected:

	debugvirtual void OnIdle(uint32_t              idleTime);		// called in ISR
	debugvirtual void OnWait(EnumAsByte(EWaitType) wait);			// wait for finish move or movementQueue full
	debugvirtual void OnStart();									// startup of movement

	debugvirtual void OnError(error_t   error);
	debugvirtual void OnWarning(error_t warning);
	debugvirtual void OnInfo(error_t    info);

	void FatalError(error_t error) { _pod._fatalError = error; }

	void Error(error_t error)
	{
		_pod._error = error;
		OnError(error);
	}

	void Info(error_t    info) { OnInfo(info); }
	void Warning(error_t warning) { OnWarning(warning); }

	void Error() NEVER_INLINE_AVR { Error(MESSAGE_UNKNOWNERROR); }
	void FatalError() { FatalError(MESSAGE_UNKNOWNERROR); }

protected:

	bool         MoveAwayFromReference(axis_t axis, uint8_t referenceId, sdist_t dist, steprate_t vMax);
	virtual void MoveAwayFromReference(axis_t axis, sdist_t dist, steprate_t     vMax) { MoveRel(axis, dist, vMax); };

#ifdef _MSC_VER
	virtual void StepBegin(const SStepBuffer* /* step */) { };
	virtual void StepEnd() {};
#endif

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) = 0;

	bool IsSameDirectionUp(axisArray_t directionUp) { return directionUp == _pod._lastDirectionUp; }

private:

	void InitMemVar();

	////////////////////////////////////////////////////////
	// HAL

protected:

	debugvirtual void InitTimer() { CHAL::InitTimer1OneShot(HandleInterrupt); }
	debugvirtual void RemoveTimer() { CHAL::RemoveTimer1(); }

	debugvirtual void StartTimer(timer_t timer);
	debugvirtual void SetIdleTimer();

	static void HandleInterrupt() { GetInstance()->StepRequest(true); }
	static void HandleBackground() { GetInstance()->Background(); }


	////////////////////////////////////////////////////////
	// timer supports pin for step / dir (A4998)
protected:

	static uint8_t          _mySteps[NUM_AXIS];
	static volatile uint8_t _setState;
	static uint8_t          _myCnt;

	enum ESetPinState
	{
		NextIsDone=0,
		NextIsSetPin,
		NextIsClearPin,
		NextIsClearDonePin,
	};

	static void InitStepDirTimer(const uint8_t steps[NUM_AXIS])
	{
		memcpy(_mySteps, steps, sizeof(_mySteps));
		_myCnt    = 0;
		_setState = NextIsSetPin;
	}
};
