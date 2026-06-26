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

////////////////////////////////////////////////////////

#include "Parser.h"
#include "GCodeTools.h"
#include "MotionControlBase.h"
#include "Control.h"

////////////////////////////////////////////////////////

#ifdef REDUCED_SIZE
typedef uint8_t mcode_t;
#else
typedef unsigned int mcode_t;
#endif

typedef uint8_t gcode_t;

////////////////////////////////////////////////////////


#define FEEDRATE_MIN_ALLOWED	STEPRATETOFEEDRATE(1)		// use VMAX => min is 1Steps/Sec because of CStepper
#define FEEDRATE_MAX_ALLOWED	feedrate_t(9999999)			// 

#define FEEDRATE_DEFAULT_G0		(-feedrate_t(526518))		// VMAXTOFEEDRATE(((SPEED_MULTIPLIER_4)-5))
#define FEEDRATE_DEFAULT_G1		feedrate_t(100000)			// 100mm/min

#define FEEDRATE_DEFAULT_MAX_G1	feedrate_t(500000)			// 500mm/min is STEPRATETOFEEDRATE(26667) with 3200Steps/rotation(16Steps) 

////////////////////////////////////////////////////////

class CGCodeParserBase : public CParser
{
private:
	typedef CParser super;

public:
	CGCodeParserBase(CStreamReader* reader, Stream* output) : super(reader, output) { }

	static void     SetG0FeedRate(feedrate_t feedrate) { _modalState.G0FeedRate = feedrate; }
	static void     SetG1FeedRate(feedrate_t feedrate) { _modalState.G1FeedRate = feedrate; }
	static void     SetG1MaxFeedRate(feedrate_t feedrate) { _modalState.G1MaxFeedRate = feedrate; }
	static mm1000_t GetG92PosPreset(axis_t axis) { return _modalState.G92Pospreset[axis]; }

	static feedrate_t GetG0FeedRate() { return _modalState.G0FeedRate; }
	static feedrate_t GetG1FeedRate() { return _modalState.G1FeedRate; }

	static bool IsMm1000() { return _modalState.UnitisMm; }
	static bool IsInch(axis_t axis) { return !IsMm1000() && IsBitSet(_modalState.UnitConvert, axis); }

	static bool IsSpindleOn() { return _modalState.SpindleOn; }

	static bool    IsCutMove() { return _modalState.CutMove; }
	static uint16_t GetSpindleSpeed() { return _modalState.SpindleSpeed; }

	static void Init()
	{
		super::Init();
		_modalState.Init();
		_modlessstate.Init();
	}

	static void SetFeedRate(feedrate_t feedrateG0, feedrate_t feedrateG1, feedrate_t feedrateG1max)
	{
		SetG0FeedRate(feedrateG0);
		SetG1FeedRate(feedrateG1);
		SetG1MaxFeedRate(feedrateG1max);
	}

	static void InitAndSetFeedRate(feedrate_t feedrateG0, feedrate_t feedrateG1, feedrate_t feedrateG1max)
	{
		Init();
		SetG0FeedRate(feedrateG0);
		SetG1FeedRate(feedrateG1);
		SetG1MaxFeedRate(feedrateG1max);
	}

protected:
	virtual void Parse() override;
	virtual bool InitParse() override;
	virtual void CleanupParse() override;

	virtual bool GCommand(gcode_t gcode);
	virtual bool MCommand(mcode_t mcode);
	virtual bool Command(char ch);

	virtual bool ParseLineNumber();
	virtual char SkipSpacesOrComment() override;

	virtual mm1000_t CalcAllPreset(axis_t axis);
	virtual void     CommentMessage(char*) { }

	bool IsCommentStart(char);

protected:
	typedef void (CGCodeParserBase::*LastCommandCB)();

	enum ESpindleMode
	{
		NormalOnOff = 0,
		CutMoveOnOff			// set on/off while starting a move g0=>off, g1=>on
	};

	////////////////////////////////////////////////////////
	// Modal State

	struct SModalState
	{
		uint32_t Clock;					// clock start time

		#ifdef REDUCED_SIZE
		uint16_t		LineNumber;
#else
		int32_t ReceivedLineNumber;
		int32_t LineNumber;
#endif

		uint8_t Plane_axis_0;			// x
		uint8_t Plane_axis_1;			// y 

		uint8_t Plane_axis_2;			// z
		uint8_t UnitConvert;			// bit array convert between inch and mm (a b c is Grad) 			

		bool UnitisMm;				// g20,g21
		bool FeedRatePerUnit;		//feedrate per Unit(mm,inch) per min, or per revolution /g94/95

		bool ConstantVelocity;		// G61 G64
		bool IsAbsolut;

		feedrate_t G0FeedRate;
		feedrate_t G1FeedRate;
		feedrate_t G1MaxFeedRate;

		uint16_t SpindleSpeed;			// see SpindleOnCW

		bool SpindleOn;
		bool SpindleOnCW;
		uint8_t SpindleMode;			// see ESpindleMode
		bool CutMove;					// G00 is no cut move

		bool ProbeOnValue;
		bool Dummy;

		CGCodeParserBase::LastCommandCB LastCommand;

		mm1000_t G92Pospreset[NUM_AXIS];

		void Init()
		{
			*this = SModalState();		// POD .. Plane Old Data Type => no Constructor => init with default value = 0
			//POD		LineNumber = 0;
			//POD		LastCommand = NULL;
			UnitisMm         = true;
			FeedRatePerUnit  = true;
			ConstantVelocity = true;
			SpindleSpeed     = 255;			// max of uint8_t (analog out)
			G0FeedRate       = FEEDRATE_DEFAULT_G0;
			G1FeedRate       = FEEDRATE_DEFAULT_G1;
			G1MaxFeedRate    = FEEDRATE_DEFAULT_MAX_G1;
			IsAbsolut        = true;
			Plane_axis_0     = X_AXIS;
			Plane_axis_1     = Y_AXIS;
			Plane_axis_2     = Z_AXIS;
			UnitConvert      = 1 + 2 + 4 + 64 + 128;				// inch to mm 
			//POD		for (uint8_t i = 0; i < NUM_AXIS; i++) G92PosPreset[i] = 0;
		}
	};

	static SModalState _modalState;

	////////////////////////////////////////////////////////
	// Modeless State

	struct SModelessState
	{
		//		uint8_t	ZeroPresetIdx;				// 0:g53-, 1:G54-
		void Init()
		{
			*this = SModelessState();		// POD .. Plane Old Data Type => no Constructor => init with default value = 0
			//			ZeroPresetIdx = _modalState.ZeroPresetIdx;
		}
	};

	static SModelessState _modlessstate;

	////////////////////////////////////////////////////////
	// Parser structure

	struct SAxisMove
	{
		uint8_t axes;		// plural, each bit for axis
		union
		{
			struct
			{
				uint8_t I : 1;			// must be bit 0	=> see getIJK();
				uint8_t J : 1;			// must be bit 1
				uint8_t K : 1;			// must be bit 2
				uint8_t F : 1;
				uint8_t R : 1;
				uint8_t Q : 1;
				uint8_t P : 1;
				uint8_t L : 1;
			} bit;

			uint8_t all;
		}           bitfield;

		mm1000_t newpos[NUM_AXIS];

		uint8_t GetIJK() const { return bitfield.all & 7; }

		SAxisMove(bool getCurrentPosition)
		{
			axes         = 0;
			bitfield.all = 0;
			if (getCurrentPosition)
			{
				CMotionControlBase::GetInstance()->GetPositions(newpos);
			}
			else
			{
				for (int32_t& newpo : newpos)
				{
					newpo = 0;
				}
			}
		}
	};

	////////////////////////////////////////////////////////

	void Sync();									// WaitBusy, sync movement with realtime
	void Wait(uint32_t ms);							// add "wait" in movement queue
	void WaitClock(uint32_t clock);					// "wait" until this clock time (see Clock in stepper.h)
	void SkipCommentNested();

	void ConstantVelocity();

	virtual bool GetParamOrExpression(mm1000_t*, bool) { return false; }
	mm1000_t     ParseCoordinate(bool convertUnits);
	mm1000_t     ParseCoordinateAxis(axis_t axis);

	uint32_t GetUint32OrParam(uint32_t max);
	uint32_t GetUint32OrParam() { return GetUint32OrParam(0xffffffffl); }
	uint16_t GetUint16OrParam() { return uint16_t(GetUint32OrParam(65535)); }
	uint8_t  GetUint8OrParam() { return uint8_t(GetUint32OrParam(255)); }

	//mm1000_t GetRelativePosition(mm1000_t pos, axis_t axis)	{ return pos - CalcAllPreset(axis); }
	//mm1000_t GetRelativePosition(axis_t axis)				{ return GetRelativePosition(CMotionControlBase::GetInstance()->GetPosition(axis), axis); }

	bool   CheckAxisSpecified(axis_t axis, uint8_t& axes);
	axis_t CharToAxis(char axis);
	axis_t CharToAxisOffset(char axis);

	bool GetAxisNo(char axis, axis_t max, axis_t& axes);
	bool GetAxisNoOffset(char axis, axis_t max, axis_t& axes);

	uint8_t GetSubCode();

	enum EAxisPosType
	{
		AbsolutWithZeroShiftPosition,
		AbsolutPosition,
		RelativPosition
	};

	mm1000_t ParseCoordinate(axis_t axis, mm1000_t relpos, EnumAsByte(EAxisPosType) posType);

	void GetUint8(uint8_t& value, uint8_t& specified, uint8_t bit);

	void GetFeedrate(SAxisMove& move);
	void GetAxis(axis_t axis, SAxisMove& move, EnumAsByte(EAxisPosType) posType);

	void InfoNotImplemented() { Info(MESSAGE_GCODE_NotImplemented); }

	uint32_t GetDweel(bool defaultIsMs);

	void SetClock();
	uint32_t GetClock();

	void GetRadius(SAxisMove& move, mm1000_t& radius);

	void CallIOControl(uint8_t io, uint16_t value);
	bool GetSpindleSpeedCommand();	
	void SpindleCallIOControl() { CallIOControl(_modalState.SpindleOnCW ? CControl::SpindleCW : CControl::SpindleCCW, _modalState.SpindleSpeed); }

	void MoveStart(bool cutMove, bool needSpindleCallIo);

	void G31Command(bool probevalue);
	bool ProbeCommand(SAxisMove& move, bool probevalue);

	static void PrintInfo();
	static void PrintPosition(mm1000_t (*GetPos)(axis_t axis));
	static void PrintPosition(mm1000_t pos);

private:
	void GetIJK(axis_t axis, SAxisMove& move, mm1000_t offset[2]);

	void GetG92Axis(axis_t axis, uint8_t& axes);

	static bool G31TestProbe(uintptr_t);

	bool LastCommand();

	void G00Command() { G0001Command(true); }
	void G01Command() { G0001Command(false); }
	void G0001Command(bool isG00);
	void G02Command() { G0203Command(true); }
	void G03Command() { G0203Command(false); }
	void G0203Command(bool isG02);
	void G04Command();
	void G171819Command(axis_t axis0, axis_t axis1, axis_t axis2);
	void G20Command() { _modalState.UnitisMm = false; }
	void G21Command() { _modalState.UnitisMm = true; }
	void G28Command();
	void G61Command() { _modalState.ConstantVelocity = false; }
	void G64Command() { _modalState.ConstantVelocity = true; }
	void G90Command() { _modalState.IsAbsolut = true; }
	void G91Command();
	void G92Command();

	void M0304Command(bool m3);					// spindle on CW/CCW
	void M05Command()
	{
		_modalState.SpindleOn = false;
		CallIOControl(CControl::SpindleCW, 0);
	} //Spindle off

	void M07Command() { CallIOControl(CControl::Coolant, CControl::CoolantOn); }
	void M09Command() { CallIOControl(CControl::Coolant, CControl::CoolantOff); }
	void M75Command() { SetClock(); }

	/////////////////

#ifdef REDUCED_SIZE
	mcode_t GetMCode()							{ return GetUInt8(); }
#else
	mcode_t GetMCode() { return GetUInt16(); }
#endif

	gcode_t GetGCode() { return GetUInt8(); }

	/////////////////

#ifdef _MSC_VER
public:
	static bool _exit;
#endif
};

////////////////////////////////////////////////////////
