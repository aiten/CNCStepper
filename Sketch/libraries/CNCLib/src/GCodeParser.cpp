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
#include <StepperLib.h>

#include "Control.h"
#include "MotionControlBase.h"
#include "MotionControl.h"
#include "ExpressionParser.h"

#include "GCodeParser.h"
#include "GCodeExpressionParser.h"
#include "GCodeBuilder.h"
#include "DecimalAsInt.h"

////////////////////////////////////////////////////////////

#ifndef REDUCED_SIZE

////////////////////////////////////////////////////////////

struct CGCodeParser::SModalState    CGCodeParser::_modalState;
struct CGCodeParser::SModelessState CGCodeParser::_modelessState;

////////////////////////////////////////////////////////////

bool CGCodeParser::SetParamCommand(param_t /* paramNo */) { return false; }

////////////////////////////////////////////////////////////

bool CGCodeParser::InitParse()
{
	if (!super::InitParse())
	{
		return false;
	}

	_modelessState.Init();
	return true;				// continue
}

////////////////////////////////////////////////////////////

void CGCodeParser::CleanupParse()
{
	_modelessState.Init();		// state for no command
	super::CleanupParse();
}

////////////////////////////////////////////////////////////

bool CGCodeParser::GetParamOrExpression(mm1000_t* value, bool convertToInch)
{
	if (_reader->GetChar() == '#')
	{
		_reader->GetNextChar();
		*value = ParseParameter(convertToInch); // do not convert if already mm1000
		return true;
	}
	if (_reader->GetChar() == '[')
	{
		const char* start = _reader->GetBuffer();
		char        ch    = _reader->GetNextChar();
		uint8_t     count = 1;

		while (!_reader->IsEOC(ch))
		{
			if (ch == '[')
			{
				count++;
			}
			else if (ch == ']')
			{
				count--;
				if (count == 0)
				{
					_reader->GetNextChar();
					CStreamReader::CSetTemporary terminate(_reader->GetBuffer());
					_reader->ResetBuffer(start);

					CGCodeExpressionParser expressionParser(this);
					expressionParser.Parse();
					if (expressionParser.IsError())
					{
						Error(expressionParser.GetError());
					}
					else
					{
						*value = CMm1000::ConvertFrom(expressionParser.Answer);
					}
					return true;
				}
			}
			ch = _reader->GetNextChar();
		}
		Error(MESSAGE_EXPR_MISSINGRPARENTHESIS);
		return true;
	}

	return super::GetParamOrExpression(value, convertToInch);
}

////////////////////////////////////////////////////////////

void CGCodeParser::CommentMessage(char* start)
{
	bool isMsg = TryToken(start, F("(MSG,"), false, true);
	if (isMsg)
	{
		start += 5;
		while (start + 1 < _reader->GetBuffer())
		{
			StepperSerial.print(*(start++));
		}
		StepperSerial.println();
	}
	else
	{
		//see: http://linuxcnc.org/docs/html/gcode/overview.html#gcode:print
		isMsg = TryToken(start, F("(PRINT,"), false, true);
		if (isMsg)
		{
			start += 7;
			const char* current = _reader->GetBuffer();
			while (start + 1 < current)
			{
				char ch = *(start++);
				if (ch == '#')
				{
					_reader->ResetBuffer(start);

					param_t paramNo = ParseParamNo();
					auto    item    = FindParamInfoByParamNo(paramNo);

					if (!IsError())
					{
						if (item)
						{
							PrintParamValue(item, axis_t(paramNo - item->GetParamNo()));
						}
						else
						{
							PrintParamValue(paramNo);
						}
					}

					start = const_cast<char*>(_reader->GetBuffer());
				}
				else
				{
					StepperSerial.print(ch);
				}
			}
			_reader->ResetBuffer(current);
			StepperSerial.println();
		}
	}
}

////////////////////////////////////////////////////////////

param_t CGCodeParser::ParseParamNo()
{
	if (_reader->SkipSpacesToUpper() == '<')		// named parameter
	{
		// format: <_varName : axis >
		// e.g.    <_home:x>
		_reader->GetNextChar();
		char        ch    = _reader->SkipSpacesToUpper();
		const char* start = _reader->GetBuffer();
		const char* colon = nullptr;

		if (!CStreamReader::IsAlpha(ch))
		{
			Error(MESSAGE_GCODE_VaribaleMustStartWithAlpha);
			return 0;
		}

		while (CStreamReader::IsAlpha(ch) || CStreamReader::IsDigit(ch) || ch == ':')
		{
			if (ch == ':')
			{
				if (colon != nullptr)
				{
					Error(MESSAGE_GCODE_NoValidVaribaleName);
					return 0;
				}
				colon = _reader->GetBuffer();
			}
			ch = _reader->GetNextChar();
		}
		const char* end = _reader->GetBuffer();

		if (CStreamReader::IsSpaceOrEnd(ch))
			ch = _reader->SkipSpacesToUpper();

		if (ch != '>')
		{
			Error(MESSAGE_GCODE_NoValidVaribaleName);
			return 0;
		}

		_reader->GetNextChar();

		CStreamReader::CSetTemporary terminate(end);
		axis_t                       a = X_AXIS;

		if (colon != nullptr)
		{
			if (colon[2] != 0 || (a = CharToAxis(CStreamReader::Toupper(colon[1]))) >= NUM_AXIS)
			{
				Error(MESSAGE_GCODE_NoValidVaribaleName);
				return 0;
			}
		}

		CStreamReader::CSetTemporary terminateColon(colon ? colon : end);

		const SParamInfo* param = FindParamInfoByText(start);

		if (param != nullptr)
		{
			if (!param->GetAllowAxisOfs() && colon != nullptr)
			{
				Error(MESSAGE_GCODE_NoValidVaribaleName);
				return 0;
			}
			return param->GetParamNo() + a;
		}

		Error(MESSAGE_GCODE_ParameterDoesntExist);
		return 0;
	}

	if (IsUInt(_reader->GetChar()))
	{
		return GetUInt16();
	}

	Error(MESSAGE_GCODE_NoValidVaribaleName);;
	return 0;
}

////////////////////////////////////////////////////////////

mm1000_t CGCodeParser::ParseParameter(bool convertToInch)
{
	param_t paramNo = ParseParamNo();
	if (paramNo)
	{
		return GetParamValue(paramNo, convertToInch);
	}

	Error(MESSAGE_GCODE_ParameterNotFound);
	return 0;
}

////////////////////////////////////////////////////////////

static bool IsModifyParam(param_t paramNo)
{
	return paramNo >= 1 && paramNo <= NUM_PARAMETERRANGE;
}

////////////////////////////////////////////////////////////

uint8_t CGCodeParser::ParamNoToParamIdx(param_t paramNo)
{
	for (uint8_t idx = 0; idx < NUM_PARAMETER; idx++)
	{
		if (uint8_t(paramNo) == _modalState.ParamNoToIdx[idx])
		{
			return idx;
		}
	}

	// return 255 of not found
	return 255;
}

// 5161-5169 - G28 Home for (X Y Z A B C U V W)
// 5221-5230 - Coordinate System 1, G54 (X Y Z A B C U V W R) - R denotes the XY rotation angle around the Z axis 
// 5420-5428 - Current Position including offsets in current program units (X Y Z A B C U V W)
// customized extension

mm1000_t CGCodeParser::GetParamValue(param_t paramNo, bool convertToInch)
{
	if (IsModifyParam(paramNo))
	{
		uint8_t paramIdx   = ParamNoToParamIdx(paramNo);
		float   paramValue = paramIdx != 255 ? _modalState.Parameter[paramIdx] : 0.0f;

		if (convertToInch)
		{
			paramValue *= 25.4f;
		}

		return CMm1000::ConvertFrom(paramValue);
	}

	const SParamInfo* param = FindParamInfoByParamNo(paramNo);

	if (param != nullptr)
	{
		auto axis = axis_t(paramNo - param->GetParamNo());
		switch (param->GetParamNo())
		{
			case PARAMSTART_G28HOME:
			{
				mm1000_t pos = CStepper::GetInstance()->GetLimitMin(axis);
				if (CStepper::GetInstance()->IsUseReference(CStepper::GetInstance()->ToReferenceId(axis, false)))	// max refmove
				{
					pos = CStepper::GetInstance()->GetLimitMax(axis);
				}
				return GetParamAsPosition(pos, axis);
			}
			case PARAMSTART_G92OFFSET: return GetParamAsMm1000(super::_modalState.G92Pospreset[axis], axis);
			case PARAMSTART_CURRENTPOS: return GetParamAsMm1000(GetRelativePosition(axis), axis);
			case PARAMSTART_CURRENTABSPOS: return GetParamAsMm1000(CMotionControlBase::GetInstance()->GetPosition(axis), axis);
			case PARAMSTART_PROBEPOS: return GetParamAsMm1000(GetRelativePosition(_modalState.G38ProbePos[axis], axis), axis);
			case PARAMSTART_PROBEOK: return _modalState.IsProbeOK ? 1 : 0;
			case PARAMSTART_BACKLASH: return GetParamAsPosition(CStepper::GetInstance()->GetBacklash(axis), axis);
			case PARAMSTART_BACKLASH_FEEDRATE: return CMotionControlBase::GetInstance()->ToMm1000(0, CStepper::GetInstance()->GetBacklash()) * 60;
			case PARAMSTART_MAX: return GetParamAsPosition(CStepper::GetInstance()->GetLimitMax(axis), axis);
			case PARAMSTART_MIN: return GetParamAsPosition(CStepper::GetInstance()->GetLimitMin(axis), axis);
			case PARAMSTART_ACC: return CStepper::GetInstance()->GetAcc(axis);
			case PARAMSTART_DEC: return CStepper::GetInstance()->GetDec(axis);
			case PARAMSTART_JERK: return CStepper::GetInstance()->GetJerkSpeed(axis);

			case PARAMSTART_G54OFFSET + 0 * PARAMSTART_G54FF_OFFSET:
			case PARAMSTART_G54OFFSET + 1 * PARAMSTART_G54FF_OFFSET:
			case PARAMSTART_G54OFFSET + 2 * PARAMSTART_G54FF_OFFSET:
			case PARAMSTART_G54OFFSET + 3 * PARAMSTART_G54FF_OFFSET:
			case PARAMSTART_G54OFFSET + 4 * PARAMSTART_G54FF_OFFSET:
			case PARAMSTART_G54OFFSET + 5 * PARAMSTART_G54FF_OFFSET:
			{
				auto idx = uint8_t((param->GetParamNo() - PARAMSTART_G54OFFSET) / PARAMSTART_G54FF_OFFSET);
				if (idx < G54ARRAYSIZE)
				{
					return GetParamAsMm1000(_modalState.G54PosPreset[idx][axis], axis);
				}
				break;
			}
			case PARAMSTART_FEEDRATE: return GetG1FeedRate();
			case PARAMSTART_CONTROLLERFAN: return CControl::GetInstance()->IOControl(CControl::ControllerFan);
			case PARAMSTART_RAPIDMOVEFEED: return -GetG0FeedRate();
			default: break;
		}
	}

	Error(MESSAGE_GCODE_ParameterNotFound);
	return 0;
}

////////////////////////////////////////////////////////////

void CGCodeParser::SetParamValue(param_t paramNo)
{
	CGCodeExpressionParser expressionParser(this);
	expressionParser.Parse();

	if (expressionParser.IsError())
	{
		Error(expressionParser.GetError());
	}
	else
	{
		mm1000_t mm1000      = CMm1000::ConvertFrom(expressionParser.Answer);
		uint32_t answerAsInt = expressionParser.Answer;
		auto     param       = FindParamInfoByParamNo(paramNo);

		if (IsModifyParam(paramNo))
		{
			uint8_t paramIdx = ParamNoToParamIdx(paramNo);

			if (paramIdx == 255)
			{
				if (expressionParser.Answer != 0.0)
				{
					uint8_t idx;
					for (idx = 0; idx < NUM_PARAMETER; idx++)
					{
						if (_modalState.ParamNoToIdx[idx] == 0)
						{
							_modalState.ParamNoToIdx[idx] = uint8_t(paramNo);
							_modalState.Parameter[idx]    = expressionParser.Answer;
							break;
						}
					}
					if (idx >= NUM_PARAMETER)
					{
						Error(MESSAGE_GCODE_NoParamSlotAvailable);
					}
				}
			}
			else if (expressionParser.Answer == 0.0)
			{
				// free slot
				_modalState.ParamNoToIdx[paramIdx] = 0;
			}
			else
			{
				_modalState.Parameter[paramIdx] = expressionParser.Answer;
			}
		}
		else if (param != nullptr)
		{
			auto axis = axis_t(paramNo - param->GetParamNo());
			switch (param->GetParamNo())
			{
				case PARAMSTART_BACKLASH:
				{
					CStepper::GetInstance()->SetBacklash(axis, mdist_t(GetParamAsMachine(mm1000, axis)));
					break;
				}
				case PARAMSTART_BACKLASH_FEEDRATE:
				{
					CStepper::GetInstance()->SetBacklash(steprate_t(GetParamAsFeedrate(mm1000, axis)));
					break;
				}
				case PARAMSTART_CONTROLLERFAN:
				{
					CControl::GetInstance()->IOControl(CControl::ControllerFan, uint16_t(answerAsInt));
					break;
				}
				case PARAMSTART_RAPIDMOVEFEED:
				{
					SetG0FeedRate(-CFeedrate1000::ConvertFrom(expressionParser.Answer));
					break;
				}
				case PARAMSTART_MAX:
				{
					CStepper::GetInstance()->SetLimitMax(axis, GetParamAsMachine(mm1000, axis));
					break;
				}
				case PARAMSTART_MIN:
				{
					CStepper::GetInstance()->SetLimitMin(axis, GetParamAsMachine(mm1000, axis));
					break;
				}
				case PARAMSTART_ACC:
				{
					CStepper::GetInstance()->SetAcc(axis, steprate_t(answerAsInt));
					break;
				}
				case PARAMSTART_DEC:
				{
					CStepper::GetInstance()->SetDec(axis, steprate_t(answerAsInt));
					break;
				}
				case PARAMSTART_JERK:
				{
					CStepper::GetInstance()->SetJerkSpeed(axis, steprate_t(answerAsInt));
					break;
				}

				case PARAMSTART_G54OFFSET + 0 * PARAMSTART_G54FF_OFFSET:
				case PARAMSTART_G54OFFSET + 1 * PARAMSTART_G54FF_OFFSET:
				case PARAMSTART_G54OFFSET + 2 * PARAMSTART_G54FF_OFFSET:
				case PARAMSTART_G54OFFSET + 3 * PARAMSTART_G54FF_OFFSET:
				case PARAMSTART_G54OFFSET + 4 * PARAMSTART_G54FF_OFFSET:
				case PARAMSTART_G54OFFSET + 5 * PARAMSTART_G54FF_OFFSET:
				{
					auto idx = uint8_t((param->GetParamNo() - PARAMSTART_G54OFFSET) / PARAMSTART_G54FF_OFFSET);
					if (idx < G54ARRAYSIZE)
					{
						_modalState.G54PosPreset[idx][axis] = mm1000;
					}
					break;
				}

				default:
				{
					Error(MESSAGE_GCODE_ParameterReadOnly);
					return;
				}
			}
		}
		else
		{
			Error(MESSAGE_GCODE_UnspportedParameterNumber);
			return;
		}

		// rest of line only comment allowed!
		ExpectEndOfCommand();
	}
}
////////////////////////////////////////////////////////////

const CGCodeParser::SParamInfo* CGCodeParser::FindParamInfo(uintptr_t param, bool (*check)(const SParamInfo*, uintptr_t))
{
	auto item = &_paramDef[0];
	while (item->GetParamNo() != 0)
	{
		if (check(item, param))
		{
			return item;
		}
		item++;
	}

	return nullptr;
}

////////////////////////////////////////////////////////////

const CGCodeParser::SParamInfo* CGCodeParser::FindParamInfoByText(const char* text)
{
	return FindParamInfo(uintptr_t(text), [](const SParamInfo* p, uintptr_t x) -> bool
	{
		const char* text = p->GetText();
		return text != nullptr && strcasecmp_P((const char*)x, text) == 0;
	});
}

////////////////////////////////////////////////////////////

const CGCodeParser::SParamInfo* CGCodeParser::FindParamInfoByParamNo(param_t paramNo)
{
	return FindParamInfo(uintptr_t(paramNo), [](const SParamInfo* p, uintptr_t x) -> bool
	{
		auto findParamNo = param_t(x);
		return p->GetParamNo() == findParamNo ||	// exact same paramNo
			(p->GetAllowAxisOfs() && p->GetParamNo() <= findParamNo && p->GetParamNo() + NUM_AXIS > findParamNo);	// diff with axis
	});
}

////////////////////////////////////////////////////////////

static const char _feedrate[] PROGMEM      = "_feedrate";
static const char _g28home[] PROGMEM       = "_g28home";
static const char _g92home[] PROGMEM       = "_g92home";
static const char _g54home[] PROGMEM       = "_g54home";
static const char _g55home[] PROGMEM       = "_g55home";
static const char _g56home[] PROGMEM       = "_g56home";
static const char _g57home[] PROGMEM       = "_g57home";
static const char _g58home[] PROGMEM       = "_g58home";
static const char _g59home[] PROGMEM       = "_g59home";
static const char _currentPos[] PROGMEM    = "_current";
static const char _currentAbsPos[] PROGMEM = "_currentAbs";
static const char _probePos[] PROGMEM      = "_probePos";
static const char _probeOk[] PROGMEM       = "_probeOK";
static const char _backlash[] PROGMEM      = "_backlash";
static const char _backlashfeed[] PROGMEM  = "_backlashfeed";
static const char _minPos[] PROGMEM        = "_minPos";
static const char _maxPos[] PROGMEM        = "_maxPos";
static const char _acc[] PROGMEM           = "_acc";
static const char _dec[] PROGMEM           = "_dec";
static const char _jerk[] PROGMEM          = "_jerk";
static const char _fan[] PROGMEM           = "_fan";
static const char _g0feedrate[] PROGMEM    = "_g0feedrate";
static const char _xPos[] PROGMEM          = "_x";
static const char _yPos[] PROGMEM          = "_y";
static const char _zPos[] PROGMEM          = "_z";
static const char _aPos[] PROGMEM          = "_a";
static const char _bPos[] PROGMEM          = "_b";
static const char _cPos[] PROGMEM          = "_c";

const CGCodeParser::SParamInfo CGCodeParser::_paramDef[] PROGMEM =
{
	{ PARAMSTART_G28HOME, _g28home, true, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_G92OFFSET, _g92home, true, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_CURRENTPOS, _currentPos, true, CGCodeParser::SParamInfo::IsMm1000 },

	{ PARAMSTART_CURRENTPOS, _xPos, false, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_CURRENTPOS + 1, _yPos, false, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_CURRENTPOS + 2, _zPos, false, CGCodeParser::SParamInfo::IsMm1000 },
#if NUM_AXIS > 3
	{ PARAMSTART_CURRENTPOS+3,	_aPos,      false,	CGCodeParser::SParamInfo::IsMm1000 },
#if NUM_AXIS > 4
	{ PARAMSTART_CURRENTPOS+4,	_bPos,      false,	CGCodeParser::SParamInfo::IsMm1000 },
#if NUM_AXIS > 5
	{ PARAMSTART_CURRENTPOS+5,	_cPos,      false,	CGCodeParser::SParamInfo::IsMm1000 },
#endif
#endif
#endif

	{ PARAMSTART_CURRENTABSPOS, _currentAbsPos, true, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_PROBEPOS, _probePos, true, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_PROBEOK, _probeOk, false, CGCodeParser::SParamInfo::IsInt },
	{ PARAMSTART_BACKLASH, _backlash, true, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_BACKLASH_FEEDRATE, _backlashfeed, false, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_MAX, _maxPos, true, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_MIN, _minPos, true, CGCodeParser::SParamInfo::IsInt },
	{ PARAMSTART_ACC, _acc, true, CGCodeParser::SParamInfo::IsInt },
	{ PARAMSTART_DEC, _dec, true, CGCodeParser::SParamInfo::IsInt },
	{ PARAMSTART_JERK, _jerk, true, CGCodeParser::SParamInfo::IsInt },
	{ PARAMSTART_CONTROLLERFAN, _fan, false, CGCodeParser::SParamInfo::IsInt },
	{ PARAMSTART_RAPIDMOVEFEED, _g0feedrate, false, CGCodeParser::SParamInfo::IsMm1000 },

	{ PARAMSTART_G54OFFSET + 0 * PARAMSTART_G54FF_OFFSET, _g54home, true, CGCodeParser::SParamInfo::IsMm1000 },
	{ PARAMSTART_G54OFFSET + 1 * PARAMSTART_G54FF_OFFSET, _g55home, true, CGCodeParser::SParamInfo::IsMm1000 },
#if G54ARRAYSIZE > 2
	{ PARAMSTART_G54OFFSET + 2 * PARAMSTART_G54FF_OFFSET, _g56home, true, CGCodeParser::SParamInfo::IsMm1000 },
#if G54ARRAYSIZE > 3
	{ PARAMSTART_G54OFFSET + 3 * PARAMSTART_G54FF_OFFSET, _g57home, true, CGCodeParser::SParamInfo::IsMm1000 },
#if G54ARRAYSIZE > 4
	{ PARAMSTART_G54OFFSET + 4 * PARAMSTART_G54FF_OFFSET, _g58home, true, CGCodeParser::SParamInfo::IsMm1000 },
#if G54ARRAYSIZE > 5
	{ PARAMSTART_G54OFFSET + 5 * PARAMSTART_G54FF_OFFSET, _g59home, true, CGCodeParser::SParamInfo::IsMm1000 },
#endif
#endif
#endif
#endif

	{ PARAMSTART_FEEDRATE, _feedrate, false, CGCodeParser::SParamInfo::IsMm1000 },
	{ 0, nullptr, false , 0 }
};

////////////////////////////////////////////////////////////

void CGCodeParser::PrintParam(const CGCodeParser::SParamInfo* item, axis_t axis)
{
	const char* paramName = item->GetText();
	StepperSerial.print('#');
	if (paramName != nullptr)
	{
		StepperSerial.print('<');
		StepperSerial.print(paramName);
		axis_t ofs = 0;
		if (axis != 255)
		{
			StepperSerial.print(':');
			StepperSerial.print(CGCodeBuilder::AxisToChar(axis));
			ofs = axis;
		}
		StepperSerial.print(F(">="));

		PrintParamValue(item, ofs);
		StepperSerial.print(F("\t\t;"));
		StepperSerial.print(item->GetParamNo() + ofs);
		StepperSerial.println();
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::PrintParamValue(const CGCodeParser::SParamInfo* item, axis_t ofs)
{
	mm1000_t paramValue = GetParamValue(item->GetParamNo() + ofs, false);
	switch (item->GetValueType())
	{
		default:
		case SParamInfo::EValueType::IsInt:
			StepperSerial.print(paramValue);
			break;
		case SParamInfo::EValueType::IsMm1000:
		{
			char tmp[16];
			StepperSerial.print(CMm1000::ToString(paramValue, tmp, 3));
			break;
		}
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::PrintParamValue(param_t paramNo)
{
	if (IsModifyParam(paramNo))
	{
		uint8_t  paramIdx   = ParamNoToParamIdx(paramNo);
		mm1000_t paramValue = paramIdx != 255 ? GetParamValue(_modalState.ParamNoToIdx[paramIdx], false) : 0;
		char     tmp[16];
		StepperSerial.println(CMm1000::ToString(paramValue, tmp, 3));
	}
}
	
////////////////////////////////////////////////////////////

void CGCodeParser::PrintAllParam()
{
	for (uint8_t paramNo : _modalState.ParamNoToIdx)
	{
		if (paramNo != 0)
		{
			StepperSerial.print('#');
			StepperSerial.print(uint16_t(paramNo));
			StepperSerial.print('=');
			PrintParamValue(paramNo);
		}
	}
	const SParamInfo* item = &_paramDef[0];
	while (item->GetParamNo() != 0)
	{
		if (item->GetAllowAxisOfs())
		{
			for (axis_t i = 0; i < NUM_AXIS; i++)
			{
				PrintParam(item, i);
			}
		}
		else
		{
			PrintParam(item, 255);
		}
		item++;
	}
}

////////////////////////////////////////////////////////////

mm1000_t CGCodeParser::CalcAllPreset(axis_t axis)
{
	return GetG54PosPreset(axis) + (IsG53Present() ? 0 : super::GetG92PosPreset(axis) + GetToolHeightPosPreset(axis));
}

////////////////////////////////////////////////////////////

mm1000_t CGCodeParser::GetG54PosPreset(axis_t axis)
{
	if (_modelessState.ZeroPresetIdx > 0)
	{
		return _modalState.G54PosPreset[_modelessState.ZeroPresetIdx - 1][axis];
	}
	// no preset
	return 0;
}

////////////////////////////////////////////////////////////

bool CGCodeParser::Command(char ch)
{
	switch (ch)
	{
		case 'S':		// spindle speed
		{
			_reader->GetNextChar();
			SpindleSpeedCommand();
			return true;
		}
		case 'T':		// tool select
		{
			_reader->GetNextChar();
			ToolSelectCommand();
			return true;
		}
		case '#':
		{
			_reader->GetNextChar();
			ParameterCommand();
			return true;
		}
		case '?':
		{
			_reader->GetNextChar();
			_OkMessage = PrintInfoAllPreset;
			return true;
		}

			// case '-':
		case '!':
		case '&': CommandEscape();
			return true;

		default: break;
	}

	return super::Command(ch);
}

////////////////////////////////////////////////////////////

bool CGCodeParser::GCommand(uint8_t gcode)
{
	if (super::GCommand(gcode))
		return true;

	switch (gcode)
	{
		// @formatter:off — disable formatter after this line
		case 10: G10Command();	return true;
		case 38: G38Command();	return true;
		case 40: G40Command();	return true;
		case 41: G41Command();	return true;
		case 42: G42Command();	return true;
		case 43: G43Command();	return true;
		case 52: InfoNotImplemented();	return true;
		case 53: G53Command();	return true;
		case 54: G5xCommand(1);	return true;
		case 55: G5xCommand(2);	return true;
		case 56: G5xCommand(3);	return true;
		case 57: G5xCommand(4);	return true;
		case 58: G5xCommand(5);	return true;
		case 59: G5xCommand(6);	return true;
		case 68: G68Command();	return true;
		case 69: G69Command();	return true;
		case 81: G81Command();	return true;
		case 82: G82Command();	return true;
		case 83: G83Command();	return true;
		case 98: G98Command();	return true;
		case 99: G99Command();	return true;
		default: break;
			// @formatter:on — enable formatter after this line
	}
	return false;
}

////////////////////////////////////////////////////////////

bool CGCodeParser::MCommand(mcode_t mcode)
{
	if (super::MCommand(mcode))
		return true;

	switch (mcode)
	{
		// @formatter:off — disable formatter after this line
		case 0: M00Command();	return true;
		case 1: M01Command();	return true;
		case 2: M02Command();	return true;
		case 6: M06Command();	return true;
		case 8: M08Command();	return true;
		case 10: M10Command();	return true;
		case 11: M11Command();	return true;
		case 110: M110Command();	return true;
		case 111: M111Command();	return true;
		case 114: M114Command();	return true;
		case 220: M220Command();	return true;
#ifndef REDUCED_SIZE
		case 300: M300Command();	return true;
#endif
		default: break;
			// @formatter:on — enable formatter after this line
	}
	return false;
}

////////////////////////////////////////////////////////////

void CGCodeParser::ParameterCommand()
{
	_reader->SkipSpaces();

	char ch = _reader->SkipSpaces();
	if (ch == '?' || ch == '!')
	{
		char nextCh = _reader->GetNextCharSkipScaces();
		if (nextCh == 0)
		{
			if (ch == '?')
			{
				PrintAllParam();
			}
			else
			{
				for (unsigned char& paramNo : _modalState.ParamNoToIdx)
				{
					paramNo = 0;
				}
			}
		}
		else
		{
			Error(MESSAGE_GCODE_NoValidVaribaleName);;
		}
		return;
	}

	param_t paramNo = ParseParamNo();
	if (IsError())
	{
		return;
	}

	if (_reader->SkipSpaces() != '=')
	{
		Error(MESSAGE_GCODE_EqExpected);
		return;
	}
	_reader->GetNextCharSkipScaces();
	if (CheckError())
	{
		return;
	}

	if (!SetParamCommand(paramNo))
	{
		SetParamValue(paramNo);
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::ToolSelectCommand()
{
	_reader->SkipSpaces();
	toolnr_t tool = GetUInt16();
	if (IsError()) return;

	if (!CGCodeTools::GetInstance()->IsValidTool(tool))
	{
		Info(MESSAGE_GCODE_NoValidTool);
	}

	_modalState.ToolSelected = tool;
}

////////////////////////////////////////////////////////////

void CGCodeParser::GetR81(SAxisMove& move)
{
	if (move.bitfield.bit.R)
	{
		Error(MESSAGE_GCODE_RalreadySpecified);
		return;
	}
	move.bitfield.bit.R = true;

	_reader->GetNextChar();
	_modalState.G8xR = ParseCoordinate(super::_modalState.Plane_axis_2, CMotionControlBase::GetInstance()->GetPosition(super::_modalState.Plane_axis_2), super::_modalState.IsAbsolut ? AbsolutWithZeroShiftPosition : RelativPosition);
}

////////////////////////////////////////////////////////////

void CGCodeParser::GetP81(SAxisMove& move)
{
	if (move.bitfield.bit.P)
	{
		Error(MESSAGE_GCODE_PalreadySpecified);
		return;
	}
	move.bitfield.bit.P = true;

	_reader->GetNextChar();
	_modalState.G8xP = GetDweel();
}

////////////////////////////////////////////////////////////

void CGCodeParser::GetL81(SAxisMove& move, uint8_t& l)
{
	if (move.bitfield.bit.L)
	{
		Error(MESSAGE_GCODE_LalreadySpecified);
		return;
	}
	move.bitfield.bit.L = true;

	_reader->GetNextChar();
	uint32_t myL = GetUint32OrParam();

	if (myL == 0 || myL > 255)
	{
		Error(MESSAGE_GCODE_LmustBe1_255);
		return;
	}
	l = uint8_t(myL);
}

////////////////////////////////////////////////////////////

void CGCodeParser::GetQ81(SAxisMove& move)
{
	if (move.bitfield.bit.Q)
	{
		Error(MESSAGE_GCODE_QalreadySpecified);
		return;
	}
	move.bitfield.bit.Q = true;

	_reader->GetNextChar();
	mm1000_t q = ParseCoordinate(super::_modalState.Plane_axis_2, 0, AbsolutPosition);

	if (q <= 0)
	{
		Error(MESSAGE_GCODE_QmustBeAPositivNumber);
		return;
	}

	_modalState.G8xQ = q;
}

////////////////////////////////////////////////////////////

void CGCodeParser::G10Command()
{
	uint8_t   specified = 0;
	uint8_t   l         = 0;
	uint8_t   p         = 0;
	uint8_t   g90or91   = 90;
	SAxisMove move(false);

	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) < NUM_AXIS)
		{
			GetAxis(axis, move, AbsolutPosition);
		}
		else if (ch == 'L')
		{
			GetUint8(l, specified, 0);
		}
		else if (ch == 'P')
		{
			GetUint8(p, specified, 1);
		}
		else if (ch == 'G')
		{
			GetUint8(g90or91, specified, 7);
			if (!IsError() && g90or91 != 90 && g90or91 != 91)
			{
				Error(MESSAGE_GCODE_G90OR91);
			}
		}
		else
		{
			break;
		}

		if (CheckError())
		{
			return;
		}
	}

	if (IsBitClear(specified, 0))
	{
		Error(MESSAGE_GCODE_LExpected);
		return;
	}
	if (IsBitClear(specified, 1))
	{
		Error(MESSAGE_GCODE_PExpected);
		return;
	}

	switch (l)
	{
		default: Error(MESSAGE_GCODE_UnsupportedLvalue);
			return;
		case 2:
		{
			if (p == 0)
			{	// current
				p = _modalState.ZeroPresetIdx;
			}
			if (p > G54ARRAYSIZE)
			{
				Error(MESSAGE_GCODE_UnsupportedCoordinateSystemUseG54Instead);
				return;
			}

			for (uint8_t axis = 0; axis < NUM_AXIS; axis++)
			{
				if (IsBitSet(move.axes, axis))
				{
					if (g90or91 == 90)
					{
						_modalState.G54PosPreset[p - 1][axis] = move.newpos[axis];
					}
					else
					{
						_modalState.G54PosPreset[p - 1][axis] += CMotionControlBase::GetInstance()->GetPosition(axis) - CalcAllPreset(axis) - move.newpos[axis];
					}
				}
			}
			break;
		}
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::G38Command()
{
	uint8_t subCode = GetSubCode();

	switch (subCode)
	{
		case 2: G31Command(super::_modalState.ProbeOnValue);
			break;
		case 4: G31Command(!super::_modalState.ProbeOnValue);
			break;
		case 12: G38CenterProbe(super::_modalState.ProbeOnValue);
			return;
		case 14: G38CenterProbe(!super::_modalState.ProbeOnValue);
			return;
		default: ErrorNotImplemented();
			return;
	}

	_modalState.IsProbeOK = !IsError();
	if (_modalState.IsProbeOK)
	{
		CMotionControlBase::GetInstance()->GetPositions(_modalState.G38ProbePos);
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::G38CenterProbe(bool probeValue)
{
	// probe
	SAxisMove move(false);

	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) < NUM_AXIS)
		{
			GetAxis(axis, move, RelativPosition);
		}
		else if (ch == 'F')
		{
			GetFeedrate(move);
		}
		else
		{
			break;
		}

		if (CheckError())
		{
			return;
		}
	}

	CMotionControlBase::GetInstance()->GetPositions(_modalState.G38ProbePos);

	for (axis_t axis = 0; axis < NUM_AXIS; axis++)
	{
		if (IsBitSet(move.axes, axis))
		{
			if (!CenterProbeCommand(move, probeValue, axis))
			{
				return;
			}
		}
	}
}

////////////////////////////////////////////////////////////

bool CGCodeParser::CenterProbeCommand(SAxisMove& move, bool probeValue, axis_t axis)
{
	SAxisMove moveNew(true);
	moveNew.axes = move.axes;
	moveNew.newpos[axis] += move.newpos[axis];

	_modalState.IsProbeOK = ProbeCommand(moveNew, probeValue);
	if (!_modalState.IsProbeOK)
	{
		return false;
	}

	mm1000_t pos = CMotionControlBase::GetInstance()->GetPosition(axis);
	moveNew.newpos[axis] -= move.newpos[axis];
	CMotionControlBase::GetInstance()->MoveAbs(moveNew.newpos, super::_modalState.G0FeedRate);
	moveNew.newpos[axis] -= move.newpos[axis];

	_modalState.IsProbeOK = ProbeCommand(moveNew, probeValue);
	if (!_modalState.IsProbeOK)
	{
		return false;
	}

	_modalState.G38ProbePos[axis] = CMotionControlBase::GetInstance()->GetPosition(axis) + (pos - CMotionControlBase::GetInstance()->GetPosition(axis)) / 2;
	CMotionControlBase::GetInstance()->MoveAbs(_modalState.G38ProbePos, super::_modalState.G0FeedRate);

	return true;
}

////////////////////////////////////////////////////////////

void CGCodeParser::G41Command()
{
	/*
			if (_reader->SkipSpacesToUpper() == 'D')
			{
				_reader->GetNextChar();
				GetUint16OrParam();			// ignore it
			}
	*/
	ErrorNotImplemented();
}

////////////////////////////////////////////////////////////

void CGCodeParser::G42Command()
{
	/*
		if (_reader->SkipSpacesToUpper() == 'D')
		{
			_reader->GetNextChar();
			GetUint16OrParam();			// ignore it
		}
	*/
	ErrorNotImplemented();
}

////////////////////////////////////////////////////////////

void CGCodeParser::G43Command()
{
	if (_reader->SkipSpacesToUpper() == 'H')
	{
		_reader->GetNextChar();
		toolnr_t tool = GetUint16OrParam();
		if (IsError())
		{
			return;
		}

		if (!CGCodeTools::GetInstance()->IsValidTool(tool))
		{
			Error(MESSAGE_GCODE_NoValidTool);
			return;
		}
		_modalState.ToolHeigtCompensation = CGCodeTools::GetInstance()->GetHeight(tool);
	}
	else
	{
		_modalState.ToolHeigtCompensation = 0;
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::GetG68IJK(axis_t axis, SAxisMove& move, mm1000_t offset[NUM_AXISXYZ])
{
	if (!CheckAxisSpecified(axis, move.bitfield.all))
	{
		return;
	}

	_reader->GetNextChar();

	offset[axis] = ParseCoordinateAxis(axis);
}

////////////////////////////////////////////////////////////

void CGCodeParser::GetAngleR(SAxisMove& move, mm1000_t& angle)
{
	if (move.bitfield.bit.R)
	{
		Error(MESSAGE_GCODE_RalreadySpecified);
		return;
	}
	move.bitfield.bit.R = true;

	_reader->GetNextChar();
	angle = ParseCoordinate(false);
}
////////////////////////////////////////////////////////////

void CGCodeParser::G68Command()
{
	uint8_t subCode = GetSubCode();

	switch (subCode)
	{
		case 10: G68Ext10Command();
			break;
		case 11: G68Ext11Command();
			break;
		case 12: G68Ext12Command();
			break;
		case 13: G68ExtXXCommand(X_AXIS);
			break;
		case 14: G68ExtXXCommand(Y_AXIS);
			break;
		case 15: G68ExtXXCommand(Z_AXIS);
			break;
		case 255: G68CommandDefault();
			break;
		default: ErrorNotImplemented();
			return;
	}

	SetPositionAfterG68G69();
}

////////////////////////////////////////////////////////////

void CGCodeParser::G68CommandDefault()
{
	if (CMotionControl::GetInstance()->IsRotate())
	{
		CMotionControl::GetInstance()->ClearRotate();
	}

	SAxisMove move(true);
	mm1000_t  r;
	mm1000_t  offset[NUM_AXISXYZ] = { 0, 0, 0 };
	mm1000_t  vect[NUM_AXISXYZ]   = { 0, 0, 0 };

	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) < NUM_AXISXYZ)
		{
			GetAxis(axis, move, super::_modalState.IsAbsolut ? AbsolutWithZeroShiftPosition : RelativPosition);
		}
		else if ((axis = CharToAxisOffset(ch)) < NUM_AXISXYZ)
		{
			GetG68IJK(axis, move, vect);
		}
		else if (ch == 'R')
		{
			GetAngleR(move, r);
		}
		else
		{
			break;
		}

		if (CheckError()) { return; }
	}

	if (!move.bitfield.bit.R)
	{
		Error(MESSAGE_GCODE_MissingR);
		return;
	}

	memcpy(offset, move.newpos, sizeof(offset));	// use current position!
	/*
		for (uint8_t axis = 0; axis < NUM_AXIS; axis++)
		{
			if (IsBitSet(move.axes, axis))
			{
				offset[axis] = move.newPos[axis];
			}
		}
	*/

	if (move.GetIJK())
	{
		//3D
		// see vect with GetG67IJK
		if (vect[0] == 0 && vect[1] && vect[2])
		{
			Error(MESSAGE_GCODE_IJKVECTORIS0);
			return;
		}
	}
	else
	{
		//2D
		vect[super::_modalState.Plane_axis_2] = 1000;
	}

	CMotionControl::GetInstance()->SetRotate(CMm1000::DegreeToRAD(r), vect, offset);
}

////////////////////////////////////////////////////////////

void CGCodeParser::G68Ext10Command()
{
	// Clear (all and set to 0), no iJK, no xyz

	mm1000_t offset[NUM_AXISXYZ] = { 0, 0, 0 };

	for (uint8_t axis = 0; axis < NUM_AXISXYZ; axis++)
	{
		CMotionControl::GetInstance()->SetRotate2D(axis, 0.0);
	}
	CMotionControl::GetInstance()->SetOffset2D(offset);
}

////////////////////////////////////////////////////////////

void CGCodeParser::G68Ext11Command()
{
	// Set Offset (x,y,z), no IJK => xyz:position(with g92, g54,...) - default is current, converted to abs position (g92 has no effect to offset)

	SAxisMove move(true);

	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) < NUM_AXISXYZ)
		{
			GetAxis(axis, move, super::_modalState.IsAbsolut ? AbsolutWithZeroShiftPosition : RelativPosition);
		}
		else
		{
			break;
		}

		if (CheckError())
		{
			return;
		}
	}

	CMotionControl::GetInstance()->SetOffset2D(move.newpos);
}

////////////////////////////////////////////////////////////

void CGCodeParser::G68Ext12Command()
{
	// with angle+offset (xyz,ijk:angle, no R):   g68.10 x10 k30
	// xyz missing => no chang of offset

	SAxisMove move(true);
	mm1000_t  vect[NUM_AXISXYZ] = { 0, 0, 0 };

	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) < NUM_AXISXYZ)
		{
			GetAxis(axis, move, super::_modalState.IsAbsolut ? AbsolutWithZeroShiftPosition : RelativPosition);
		}
		else if ((axis = CharToAxisOffset(ch)) < NUM_AXISXYZ)
		{
			GetG68IJK(axis, move, vect);
		}
		else
		{
			break;
		}

		if (CheckError())
		{
			return;
		}
	}

	if (move.axes)
	{
		CMotionControl::GetInstance()->SetOffset2D(move.newpos);
	}

	for (uint8_t axis = 0; axis < NUM_AXISXYZ; axis++)
	{
		if (IsBitSet(move.GetIJK(), axis))
		{
			// angle
			CMotionControl::GetInstance()->SetRotate2D(axis, CMm1000::DegreeToRAD(vect[axis]));
		}
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::G68ExtXXCommand(axis_t rotAxis)
{
	// calculate angle(X)

	SAxisMove move(true);
	mm1000_t  vect[NUM_AXISXYZ] = { 0, 0, 0 };

	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) < NUM_AXISXYZ)
		{
			GetAxis(axis, move, super::_modalState.IsAbsolut ? AbsolutWithZeroShiftPosition : RelativPosition);
		}
		else if ((axis = CharToAxisOffset(ch)) < NUM_AXISXYZ)
		{
			if (rotAxis == axis)
			{
				Error(MESSAGE_GCODE_SPECIFIED);
			}
			else
			{
				GetG68IJK(axis, move, vect);
			}
		}
		else
		{
			break;
		}

		if (CheckError())
		{
			return;
		}
	}

	float pos1 = float(move.newpos[rotAxis] - CMotionControl::GetInstance()->GetOffset2D(rotAxis));
	float pos2;
	float angle;

	axis_t axis2;
	axis_t axis3;

	switch (rotAxis)
	{
		case X_AXIS:
		{
			axis2 = Y_AXIS;
			axis3 = Z_AXIS;
			break;
		}
		case Y_AXIS:
		{
			axis2 = Z_AXIS;
			axis3 = X_AXIS;
			break;
		}
		case Z_AXIS:
		{
			axis2 = X_AXIS;
			axis3 = Y_AXIS;
			break;
		}
		default: InfoNotImplemented();
			return;
	}

	if (IsBitSet(move.GetIJK(), axis3))
	{
		// calc angle
		pos2  = float(move.newpos[axis2] - CMotionControl::GetInstance()->GetOffset2D(axis2) - vect[axis3]);
		angle = atan2(pos2, pos1);
		CMotionControl::GetInstance()->SetRotate2D(axis3, angle);
		pos1 = hypotf(pos1, pos2);	// correction for 2nd rotation axis
	}
	if (IsBitSet(move.GetIJK(), axis2))
	{
		// calc angle
		pos2  = float(move.newpos[axis3] - CMotionControl::GetInstance()->GetOffset2D(axis3) - vect[axis2]);
		angle = atan2(pos2, pos1);
		CMotionControl::GetInstance()->SetRotate2D(axis2, angle);
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::G69Command()
{
	if (CMotionControl::GetInstance()->IsRotate())
	{
		CMotionControl::GetInstance()->ClearRotate();
		SetPositionAfterG68G69();
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::G53Command()
{
	_modelessState.ZeroPresetIdx = 0;
}

////////////////////////////////////////////////////////////

void CGCodeParser::G5xCommand(uint8_t idx)
{
	// G54 => idx = 1 => arraySize==1

	if (CutterRadiosIsOn())
	{
		return;
	}

	if (idx > G54ARRAYSIZE)
	{
		ErrorNotImplemented();
		return;
	}

	_modelessState.ZeroPresetIdx = _modalState.ZeroPresetIdx = idx;
	CLcd::InvalidateLcd();
}

////////////////////////////////////////////////////////////

void CGCodeParser::G8xCommand(SAxisMove& move, bool useP, bool useQ, bool useMinQ)
{
	if (CutterRadiosIsOn()) return;

	uint8_t l = 1;

	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) <= Z_AXIS)
		{
			GetAxis(axis, move, super::_modalState.IsAbsolut ? AbsolutWithZeroShiftPosition : RelativPosition);
		}
		else if (ch == 'R')
		{
			GetR81(move);
		}
		else if (ch == 'L')
		{
			GetL81(move, l);
		}
		else if (ch == 'F')
		{
			GetFeedrate(move);
		}
		else if (ch == 'P' && useP)
		{
			GetP81(move);
		}
		else if (ch == 'Q' && useQ)
		{
			GetQ81(move);
		}
		else
		{
			break;
		}

		if (CheckError())
		{
			return;
		}
	}

	if (!CheckError() && move.axes)
	{
		if (useQ && _modalState.G8xQ == 0)
		{
			Error(MESSAGE_GCODE_QmustNotBe0);
			return;
		}

		if (IsBitSet(move.axes, super::_modalState.Plane_axis_2))
		{
			_modalState.G8xPlane2 = move.newpos[super::_modalState.Plane_axis_2];
		}

		mm1000_t pos[NUM_AXIS];
		CMotionControlBase::GetInstance()->GetPositions(pos);

		mm1000_t origPlane2 = pos[super::_modalState.Plane_axis_2];

		bool drillDown = origPlane2 > _modalState.G8xPlane2;

		//		// r : z(now) <= r <= z(down)
		if ((drillDown && (origPlane2 < _modalState.G8xR || _modalState.G8xPlane2 > _modalState.G8xR)) ||
			(!drillDown && (origPlane2 > _modalState.G8xR || _modalState.G8xPlane2 < _modalState.G8xR)))
		{
			Error(MESSAGE_GCODE_RmustBeBetweenCurrentRZ);
			return;
		}

		for (uint8_t i = 0; i < l; i++)
		{
			// 1. Step: GoTo x:y (fast)
			//          For rel move store relative distance in move.newpos
			if (super::_modalState.IsAbsolut || i == 0)
			{
				pos[super::_modalState.Plane_axis_0] = move.newpos[super::_modalState.Plane_axis_0];
				pos[super::_modalState.Plane_axis_1] = move.newpos[super::_modalState.Plane_axis_1];
				if (!super::_modalState.IsAbsolut)
				{
					move.newpos[super::_modalState.Plane_axis_0] -= CMotionControlBase::GetInstance()->GetPosition(super::_modalState.Plane_axis_0);
					move.newpos[super::_modalState.Plane_axis_1] -= CMotionControlBase::GetInstance()->GetPosition(super::_modalState.Plane_axis_1);
				}
			}
			else
			{
				pos[super::_modalState.Plane_axis_0] += move.newpos[super::_modalState.Plane_axis_0];
				pos[super::_modalState.Plane_axis_1] += move.newpos[super::_modalState.Plane_axis_1];
			}

			MoveStart(false);
			CMotionControlBase::GetInstance()->MoveAbs(pos, super::_modalState.G0FeedRate);
			if (CheckError())
			{
				return;
			}

			// 2. Step: GoTo z(R) (fast)
			pos[super::_modalState.Plane_axis_2] = _modalState.G8xR;
			CMotionControlBase::GetInstance()->MoveAbs(pos, super::_modalState.G0FeedRate);
			if (CheckError())
			{
				return;
			}

			mm1000_t nextPlan2 = _modalState.G8xR;
			bool     finalMove = false;

			while (!finalMove)
			{
				if (useQ)
				{
					if (drillDown)
					{
						nextPlan2 -= _modalState.G8xQ;
						finalMove = _modalState.G8xPlane2 >= nextPlan2;
					}
					else
					{
						nextPlan2 += _modalState.G8xQ;
						finalMove = _modalState.G8xPlane2 <= nextPlan2;
					}
				}
				else
				{
					finalMove = true;
				}

				if (finalMove)
				{
					nextPlan2 = _modalState.G8xPlane2;
				}

				// 3. Step: Goto Z (with feedrate)
				MoveStart(true);
				pos[super::_modalState.Plane_axis_2] = nextPlan2;
				CMotionControlBase::GetInstance()->MoveAbs(pos, super::_modalState.G1FeedRate);
				if (CheckError())
				{
					return;
				}

				// 3.a. Step: Wait
				if (useP && _modalState.G8xP != 0)
				{
					Wait(_modalState.G8xP);
				}

				// 4. Step: Goto init Z or R (fast) see G98
				if (finalMove)
				{
					pos[super::_modalState.Plane_axis_2] = (_modalState.IsG98) ? origPlane2 : _modalState.G8xR;
				}
				else if (useMinQ)
				{
					pos[super::_modalState.Plane_axis_2] = nextPlan2 + (drillDown ? G73RETRACTION : -G73RETRACTION);
				}
				else
				{
					pos[super::_modalState.Plane_axis_2] = _modalState.G8xR;
				}

				MoveStart(false);
				CMotionControlBase::GetInstance()->MoveAbs(pos, super::_modalState.G0FeedRate);
				if (CheckError())
				{
					return;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::G73Command()
{
	super::_modalState.LastCommand = LastCommandCB(&CGCodeParser::G73Command);

	SAxisMove move(true);
	G8xCommand(move, false, true, true);
}

////////////////////////////////////////////////////////////

void CGCodeParser::G81Command()
{
	super::_modalState.LastCommand = LastCommandCB(&CGCodeParser::G81Command);

	SAxisMove move(true);
	G8xCommand(move, false, false, false);
}

////////////////////////////////////////////////////////////

void CGCodeParser::G82Command()
{
	super::_modalState.LastCommand = LastCommandCB(&CGCodeParser::G82Command);

	SAxisMove move(true);
	G8xCommand(move, true, false, false);
}

////////////////////////////////////////////////////////////

void CGCodeParser::G83Command()
{
	super::_modalState.LastCommand = LastCommandCB(&CGCodeParser::G83Command);

	SAxisMove move(true);
	G8xCommand(move, false, true, false);
}

////////////////////////////////////////////////////////////

void CGCodeParser::M00Command()
{
	//Stop
	Sync();
	CControl::GetInstance()->StopProgram(false);
}

void CGCodeParser::M01Command()
{
	//Optional Stop
	Sync();
	CControl::GetInstance()->StopProgram(true);
}

void CGCodeParser::M02Command() {}

////////////////////////////////////////////////////////////

void CGCodeParser::M06Command()
{
	// ATC (automatic tool change)	
}

////////////////////////////////////////////////////////////

void CGCodeParser::M08Command()
{
	//coolant on (flood)
	CallIOControl(CControl::Coolant, CControl::CoolantFlood);
}

////////////////////////////////////////////////////////////

void CGCodeParser::M10Command()
{
	//vacuum on
	CallIOControl(CControl::Vacuum, CControl::VacuumOn);
}

////////////////////////////////////////////////////////////

void CGCodeParser::M11Command()
{
	//vacuum off
	CallIOControl(CControl::Vacuum, CControl::VacuumOff);
}

////////////////////////////////////////////////////////////

void CGCodeParser::M110Command()
{
	// set lineNumber

	uint32_t lineNumber = 0;

	if (_reader->SkipSpacesToUpper() == 'N')
	{
		_reader->GetNextChar();
		lineNumber = GetUInt32();
	}

	if (!ExpectEndOfCommand())
	{
		return;
	}

	super::_modalState.LineNumber = lineNumber;
}
////////////////////////////////////////////////////////////

void CGCodeParser::M111Command()
{
	// set debug level

	if (_reader->SkipSpacesToUpper() == 'S')
	{
		_reader->GetNextChar();
		_modalState._debugLevel = GetUInt8();
	}

	if (!ExpectEndOfCommand())
	{
		return;
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::M114Command()
{
	uint8_t posType = 0;

	if (_reader->SkipSpacesToUpper() == 'S')
	{
		_reader->GetNextChar();
		posType = GetUInt8();
	}

	_OkMessage = posType == 1 ? PrintRelPosition : PrintAbsPosition;

	if (!ExpectEndOfCommand())
	{
		return;
	}
}


////////////////////////////////////////////////////////////

void CGCodeParser::M220Command()
{
	// set speed override

	if (_reader->SkipSpacesToUpper() == 'S')
	{
		_reader->GetNextChar();
		uint8_t speedInP = GetUInt8();
		if (IsError())
		{
			return;
		}
		CStepper::GetInstance()->SetSpeedOverride(CStepper::PToSpeedOverride(speedInP));
	}
	else
	{
		Error(MESSAGE_GCODE_SExpected);
		return;
	}

	if (!ExpectEndOfCommand()) { return; }
}

////////////////////////////////////////////////////////////

void CGCodeParser::M300Command()
{
	SPlayTone        tone[2];
	const SPlayTone* myTone      = tone;
	bool             fromProgMem = false;

	tone[0].Tone     = ToneA4;
	tone[0].Duration = MilliSecToDuration(500);

	tone[1].Tone = ToneEnd;

	if (_reader->SkipSpacesToUpper() == 'S')
	{
		_reader->GetNextChar();
		unsigned int freq = GetUInt16();
		tone[0].Tone      = ETone(FreqToTone(freq));
		if (IsError())
		{
			return;
		}

		switch (freq)
		{
			case 1:
			{
				myTone      = SPlayTone::PlayOK;
				fromProgMem = true;
				break;
			}
			case 2:
			{
				myTone      = SPlayTone::PlayError;
				fromProgMem = true;
				break;
			}
			case 3:
			{
				myTone      = SPlayTone::PlayInfo;
				fromProgMem = true;
				break;
			}
			default: break;
		}
	}
	if (!fromProgMem && _reader->SkipSpacesToUpper() == 'P')
	{
		_reader->GetNextChar();
		tone[0].Duration = MilliSecToDuration(GetUInt16());
		if (IsError())
		{
			return;
		}
	}

	if (!ExpectEndOfCommand())
	{
		return;
	}

	if (CLcd::GetInstance())
	{
		CLcd::GetInstance()->Beep(myTone, fromProgMem);
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::CommandEscape()
{
	CNCLibCommandExtensions();
}

////////////////////////////////////////////////////////////

void CGCodeParser::CNCLibCommandExtensions()
{
	char ch = _reader->SkipSpaces();

	switch (ch)
	{
		case '&':
		{
			_reader->GetNextChar();
			if (!ExpectEndOfCommand()) { return; }

			CStepper::GetInstance()->Dump(CStepper::DumpAll);
			break;
		}
		case '!':
		{
			_reader->GetNextChar();
			if (_reader->IsEOC(SkipSpacesOrComment()))
			{
				CControl::GetInstance()->Kill();
			}
			break;
		}
		default: break;
	}
}

////////////////////////////////////////////////////////////

void CGCodeParser::PrintAbsPosition()
{
	PrintPosition([](axis_t axis) { return CMotionControlBase::GetInstance()->GetPosition(axis); });
}

////////////////////////////////////////////////////////////

void CGCodeParser::PrintRelPosition()
{
	PrintPosition([](axis_t axis) { return CMotionControlBase::GetInstance()->GetPosition(axis) - CGCodeParser::GetAllPreset(axis); });
}

////////////////////////////////////////////////////////////

void CGCodeParser::PrintInfoAllPreset()
{
	StepperSerial.print(F("<MPos:"));
	PrintPosition([](axis_t axis) { return CMotionControlBase::GetInstance()->GetPosition(axis); });
	StepperSerial.print(F("|WCO:"));
	PrintPosition([](axis_t axis) { return GetAllPreset(axis); });
	StepperSerial.print('>');
}

////////////////////////////////////////////////////////////

#endif // REDUCED_SIZE
