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

#include "GCodeParserBase.h"
#include "GCodeTools.h"

////////////////////////////////////////////////////////

typedef uint16_t param_t;

// see: http://linuxcnc.org/docs/html/gcode/overview.html#_numbered_parameters_a_id_sub_numbered_parameters_a

#define PARAMSTART_G28HOME		5161		// 5161-5169 - G28 Home for (X Y Z A B C U V W)
#define PARAMSTART_G92OFFSET	5211		// 5211-5219 - G92 offset (X Y Z A B C U V W) 
#define PARAMSTART_G54OFFSET	5221		// 5221-5230 - Coordinate System 1, G54 (X Y Z A B C U V W R) - R denotes the XY rotation angle around the Z axis 
#define PARAMSTART_G54FF_OFFSET	  20		// 5241-5250 - Coordinate System 2, G55 (X Y Z A B C U V W R) - R denotes the XY rotation angle around the Z axis 
#define PARAMSTART_CURRENTPOS	5420		// 5420-5428 - Current Position including offsets in current program units (X Y Z A B C U V W)

#define PARAMSTART_PROBEPOS		5061		// After successful probing, parameters 5061 to 5069 will be set to the coordinates of X, Y, Z, A, B, C, U, V, W of the location of the controlled point at the time the probe changed state
#define PARAMSTART_PROBEOK		5070		// Parameter 5070 is set to 1 if the probe succeeded and 0 if the probe failed

// extent
#define PARAMSTART_CURRENTABSPOS	6010	// Current absolute machine position in current program units (X Y Z A B C U V W)
#define PARAMSTART_BACKLASH			6031	// Backlash in current units(e.g. mm) (X Y Z A B C U V W)
#define PARAMSTART_BACKLASH_FEEDRATE 6049	// Feedrate for backlash (0 if disabled)


#define PARAMSTART_MAX				6051	// Acc (X Y Z A B C U V W)
#define PARAMSTART_MIN				6071	// Acc (X Y Z A B C U V W)
#define PARAMSTART_ACC				6091	// Acc (X Y Z A B C U V W)
#define PARAMSTART_DEC				6111	// Dec (X Y Z A B C U V W)
#define PARAMSTART_JERK				6131	// Jerk (X Y Z A B C U V W)

#define PARAMSTART_CONTROLLERFAN	6900	// ControllerFan if not idle (0 if disabled, 255 max)
#define PARAMSTART_RAPIDMOVEFEED	6901	// RapidMove Feedrate

// name, name to internal number

#define PARAMSTART_INTERNAL			14000
#define PARAMSTART_FEEDRATE			(PARAMSTART_INTERNAL+0)	// Feedrate


// g73 retraction
#define G73RETRACTION			200			// mm1000_t => 0.2mm

////////////////////////////////////////////////////////

class CGCodeParser : public CGCodeParserBase
{
private:

	typedef CGCodeParserBase super;
	friend class CGCodeExpressionParser;

public:

	CGCodeParser(CStreamReader* reader, Stream* output) : super(reader, output) { };

	static mm1000_t GetG54PosPreset(axis_t        axis);
	static mm1000_t GetToolHeightPosPreset(axis_t axis) { return axis == super::_modalState.Plane_axis_2 ? _modalState.ToolHeigtCompensation : 0; }
	static void     SetG54PosPreset(axis_t        axis, mm1000_t pos) { _modalState.G54PosPreset[0][axis] = pos; }
	static uint8_t  GetZeroPresetIdx() { return _modalState.ZeroPresetIdx; }
	static void     SetZeroPresetIdx(uint8_t idx) { _modalState.ZeroPresetIdx = idx; }

	static bool IsG53Present() { return _modelessState.ZeroPresetIdx == 0; }

	static mm1000_t GetAllPreset(axis_t axis) { return GetG92PosPreset(axis) + GetG54PosPreset(axis) + GetToolHeightPosPreset(axis); }

	static void Init()
	{
		super::Init();
		_modalState.Init();
		_modelessState.Init();
	}

	static void InitAndSetFeedRate(feedrate_t feedrateG0, feedrate_t feedrateG1, feedrate_t feedrateG1max)
	{
		Init();
		super::InitAndSetFeedRate(feedrateG0, feedrateG1, feedrateG1max);
	}

protected:

	virtual bool InitParse() override;
	virtual void CleanupParse() override;

	virtual bool GCommand(gcode_t        gcode) override;	// check for GCode extension => return true if command is parsed, false to do default
	virtual bool MCommand(mcode_t        mcode) override;
	virtual bool SetParamCommand(param_t pramNo);
	virtual bool Command(char            ch) override;

	void ToolSelectCommand();
	void ParameterCommand();

	virtual void     CommentMessage(char* ) override;
	virtual mm1000_t CalcAllPreset(axis_t axis) override;

protected:

	////////////////////////////////////////////////////////
	// Modal State

	struct SModalState
	{
		enum ECutterRadiusCompensation
		{
			CutterRadiusOff = 0,
			CutterRadiusLeft,
			CutterRadiusRight
		};

		EnumAsByte(ECutterRadiusCompensation) CutterRadiusCompensation;
		bool                                  EvenSize;

		uint8_t ZeroPresetIdx;				// 0:g53-, 1:G54-
		bool    IsG98;						// G98 or G99	( Return To R or return to init Z) 

		uint8_t _debugLevel;
		bool    IsProbeOK;

		toolnr_t ToolSelected;

		mm1000_t G8xQ;
		mm1000_t G8xPlane2;
		mm1000_t G8xR;
		mm1000_t G8xP;

		mm1000_t G54PosPreset[G54ARRAYSIZE][NUM_AXIS];	// 54-59
		mm1000_t G38ProbePos[NUM_AXIS];
		mm1000_t ToolHeigtCompensation;

		float   Parameter[NUM_PARAMETER];		// this is a expression, mm or inch
		uint8_t ParamNoToIdx[NUM_PARAMETER];

		void Init()
		{
			*this         = SModalState();		// POD .. Plane Old Data Type => no Constructor => init with default value = 0
			ZeroPresetIdx = 1;						// always 54
			//POD		G8xQ = G8xR = G8xPlane2 = G8xP = 0;
			CutterRadiusCompensation = CutterRadiusOff;
			ToolSelected             = 1;
			IsG98                    = true;
			//POD		_debugLevel = 0;
			//POD		for (uint8_t i = 0; i < NUM_AXIS; i++) G54PosPreset[i] = 0;
			//POD		for (uint8_t i = 0; i < NUM_PARAMETER; i++) Parameter[i] = 0;
			//POD		ToolHeightCompensation = 0;
		}
	};

	static SModalState _modalState;

	////////////////////////////////////////////////////////
	// Modeless State

	struct SModelessState
	{
		uint8_t ZeroPresetIdx;				// 0:g53-, 1:G54-
		void    Init()
		{
			// *this = SModelessState();		// POD .. Plane Old Data Type => no Constructor => init with default value = 0
			ZeroPresetIdx = _modalState.ZeroPresetIdx;
		}
	};

	static SModelessState _modelessState;

	////////////////////////////////////////////////////////
	// Parser structure

	bool CutterRadiusIsOn()
	{
		if (_modalState.CutterRadiusCompensation)
		{
			Info(MESSAGE_GCODE_G41G43AreNotAllowedWithThisCommand);
			return true;
		}
		else return false;
	}

	virtual bool GetParamOrExpression(mm1000_t*, bool convertToInch) override;
	mm1000_t     ParseParameter(bool           convertToInch);
	param_t      ParseParamNo();

	mm1000_t GetParamValue(param_t paramNo, bool convertToInch);
	void     SetParamValue(param_t paramNo);

	static uint8_t ParamNoToParamIdx(param_t paramNo);

	static mm1000_t GetParamAsMm1000(mm1000_t   posMm100, axis_t     ) { return posMm100; }
	static mm1000_t GetParamAsPosition(mm1000_t posInMachine, axis_t axis) { return CMotionControlBase::GetInstance()->ToMm1000(axis, posInMachine); }
	static mm1000_t GetParamAsMachine(mm1000_t  posInmm1000, axis_t  axis) { return CMotionControlBase::GetInstance()->ToMachine(axis, posInmm1000); }
	static mm1000_t GetParamAsFeedrate(mm1000_t feedrate, axis_t     axis) { return CMotionControlBase::GetInstance()->ToMachine(axis, feedrate / 60); }

	mm1000_t GetRelativePosition(mm1000_t pos, axis_t axis) { return pos - GetG92PosPreset(axis) - GetG54PosPreset(axis); }
	mm1000_t GetRelativePosition(axis_t   axis) { return GetRelativePosition(CMotionControlBase::GetInstance()->GetPosition(axis), axis); }

	void GetG68IJK(axis_t axis, SAxisMove& move, mm1000_t offset[NUM_AXISXYZ]);

private:

	void GetR81(SAxisMove&    move);
	void GetP81(SAxisMove&    move);
	void GetQ81(SAxisMove&    move);
	void GetL81(SAxisMove&    move, uint8_t&  l);
	void GetAngleR(SAxisMove& move, mm1000_t& angle);		// get angle (with R Parameter)

	void G10Command();
	void G38Command();
	void G40Command() { _modalState.CutterRadiusCompensation = SModalState::CutterRadiusOff; }
	void G41Command();		// Cutter Radius Compensation left
	void G42Command();		// Cutter Radius Compensation right
	void G43Command();		// Tool Height Compensation 
	void G49Command() { _modalState.ToolHeigtCompensation = 0; }
	void G53Command();
	void G68Command();
	void G68CommandDefault();
	void G68Ext10Command();
	void G68Ext11Command();
	void G68Ext12Command();
	void G68ExtXXCommand(axis_t rotaxis);
	void G69Command();
	void G5xCommand(uint8_t    idx);
	void G8xCommand(SAxisMove& move, bool useP, bool useQ, bool useMinQ);
	void G73Command();		// High-speed Peck Drilling for Shallow Holes
	void G81Command();		// Basic drilling canned cycle
	void G82Command();		// Spot Drilling Cycle
	void G83Command();		// Peck Drilling for Deeper Holes
	void G98Command() { _modalState.IsG98 = true; };
	void G99Command() { _modalState.IsG98 = false; };

	void M00Command();		// Compulsory stop
	void M01Command();		// Optional stop
	void M02Command();		// End of program
	void M06Command();		// Automatic tool change => not supported
	void M08Command();		// Coolant on (flood)
	void M10Command();		// vacuum on
	void M11Command();		// vacuum off
	void M31Command();      // Print time since start
	void M110Command();
	void M111Command();		// Set debug level
	void M114Command();		// Report Position

	void M220Command();		// Set Speed override
	void M300Command();		// Play Song

	void G38CenterProbe(bool           probevalue);
	bool CenterProbeCommand(SAxisMove& move, bool probevalue, axis_t axis);

	/////////////////

	void CommandEscape();

	void CNCLibCommandExtensions();
	/////////////////
	// OK Message

protected:

	static void PrintAbsPosition();
	static void PrintRelPosition();

	static void PrintInfoAllPreset();

	void SetPositionAfterG68G69() { CMotionControlBase::GetInstance()->SetPositionFromMachine(); }


protected:

	struct SParamInfo
	{
	public:
		param_t     _paramNo;		// base param address
		const char* _text;
		bool        _allowaxisofs;

		enum EValueType
		{
			IsInt,
			IsMm1000
		};

		uint8_t _valueType;
	public:

		const char* GetText() const { return (const char*)pgm_read_ptr(&this->_text); }
		param_t     GetParamNo() const { return pgm_read_word(&this->_paramNo); }
		bool        GetAllowAxisOfs() const { return pgm_read_byte(&this->_allowaxisofs) != 0; }
		uint8_t     GetValueType() const { return uint8_t(pgm_read_byte(&this->_valueType)); }
	};

	void PrintAllParam();
	void PrintParam(const SParamInfo*      item, axis_t axis);
	void PrintParamValue(const SParamInfo* item, axis_t ofs);
	void PrintParamValue(param_t           paramNo);

	static const struct SParamInfo _paramDef[] PROGMEM;

	static const SParamInfo* FindParamInfo(uintptr_t         param, bool (*check)(const SParamInfo*, uintptr_t));
	static const SParamInfo* FindParamInfoByText(const char* text);
	static const SParamInfo* FindParamInfoByParamNo(param_t  paramNo);
};

////////////////////////////////////////////////////////
