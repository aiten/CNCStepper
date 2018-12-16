////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/
////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "Control.h"
#include "Lcd.h"

#include "GCodeParser.h"
#include "ConfigEeprom.h"

////////////////////////////////////////////////////////////

template <> CControl* CSingleton<CControl>::_instance = nullptr;

////////////////////////////////////////////////////////

CControl::CControl()
{
	_bufferIdx = 0;
}

////////////////////////////////////////////////////////////

void CControl::Init()
{
	CStepper::GetInstance()->Init();
	CStepper::GetInstance()->AddEvent(StaticStepperEvent, uintptr_t(this), _oldStepperEvent);

#ifdef _USE_LCD

	if (CLcd::GetInstance())
	{
		CLcd::GetInstance()->Init();
	}

#endif

	if (_timeBlink == 0)
	{
		CHAL::pinModeOutput(BLINK_LED);
	}

	CHAL::InitTimer0(HandleInterrupt);
	CHAL::StartTimer0(IDLETIMER0VALUE);
}

////////////////////////////////////////////////////////////

void CControl::Initialized()
{
	StepperSerial.println(MESSAGE_OK_INITIALIZED);
	GoToReference();
	CMotionControlBase::GetInstance()->SetPositionFromMachine();
}

////////////////////////////////////////////////////////////

void CControl::InitFromEeprom()
{
	CStepper::GetInstance()->SetDirection(CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, StepperDirections)));

#ifdef REDUCED_SIZE

	CMotionControlBase::GetInstance()->InitConversionStepsPer(CConfigEeprom::GetConfigFloat(offsetof(CConfigEeprom::SCNCEeprom, StepsPerMm1000)));

#else

	CMotionControlBase::GetInstance()->InitConversionBestStepsPer(CConfigEeprom::GetConfigFloat(offsetof(CConfigEeprom::SCNCEeprom, StepsPerMm1000)));

#endif

	uint16_t jerkspeed = CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, JerkSpeed));
	if (jerkspeed == 0)
	{
		jerkspeed = 1024;
	}

	CStepper::GetInstance()->SetDefaultMaxSpeed(
		steprate_t(CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MaxStepRate))),
		steprate_t(CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, Acc))),
		steprate_t(CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, Dec))),
		jerkspeed);

	for (uint8_t axis = 0; axis < NUM_AXIS; axis++)
	{
		eepromofs_t ofs = sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions) * axis;
		CStepper::GetInstance()->SetReferenceHitValue(CStepper::GetInstance()->ToReferenceId(axis, true), CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].ReferenceValueMin) + ofs));
		CStepper::GetInstance()->SetReferenceHitValue(CStepper::GetInstance()->ToReferenceId(axis, false), CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].ReferenceValueMax) + ofs));

		CStepper::GetInstance()->SetLimitMax(axis, CMotionControlBase::GetInstance()->ToMachine(axis, CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].Size) + ofs)));

#ifndef REDUCED_SIZE

		steprate_t steprate = CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].MaxStepRate) + ofs);
		if (steprate != 0)
		{
			CStepper::GetInstance()->SetMaxSpeed(axis, steprate);
		}

		steprate = CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].Acc) + ofs);
		if (steprate != 0)
		{
			CStepper::GetInstance()->SetAcc(axis, steprate);
		}

		steprate = CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].Dec) + ofs);
		if (steprate != 0)
		{
			CStepper::GetInstance()->SetDec(axis, steprate);
		}

		float stepsPerMM1000 = CConfigEeprom::GetConfigFloat(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].StepsPerMm1000) + ofs);
		if (stepsPerMM1000 != 0.0)
		{
			CMotionControlBase::GetInstance()->SetConversionStepsPerEx();
			CMotionControlBase::GetInstance()->SetConversionStepsPerEx(axis, stepsPerMM1000);
		}

#endif
	}
}

////////////////////////////////////////////////////////////

uint8_t CControl::ConvertSpindleSpeedToIO8(uint16_t maxspeed, uint16_t level)
{
	if (level >= maxspeed)
	{
		return 255;
	}
	return uint8_t(MulDivU32(level, 255, maxspeed));
}

////////////////////////////////////////////////////////////

void CControl::GoToReference()
{
	for (axis_t i = 0; i < NUM_AXIS; i++)
	{
		axis_t axis = CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].RefmoveSequence) + sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions) * i);
		if (axis < NUM_AXIS)
		{
			eepromofs_t ofs = sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions) * axis;
			GoToReference(axis, CMotionControlBase::GetInstance()->ToMachine(axis, CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].PosNoRefMove) + ofs)));
		}
	}
}

////////////////////////////////////////////////////////////

bool CControl::GoToReference(axis_t axis, udist_t posIfNoRefMove)
{
	EnumAsByte(EReverenceType) referenceType = EReverenceType(CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].ReferenceType) + sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions) * axis));

	if (referenceType == EReverenceType::NoReference)
	{
		CStepper::GetInstance()->SetPosition(axis, posIfNoRefMove);
		return false;
	}

	GoToReference(
		axis,
		steprate_t(CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, RefMoveStepRate))),
		referenceType == EReverenceType::ReferenceToMin);
	return true;
}

////////////////////////////////////////////////////////////

bool CControl::GoToReference(axis_t axis, steprate_t stepRate, bool toMinRef)
{
	// goto min/max
	return CStepper::GetInstance()->MoveReference(
		axis,
		CStepper::GetInstance()->ToReferenceId(axis, toMinRef),
		toMinRef,
		stepRate,
		0,
		CMotionControlBase::GetInstance()->ToMachine(axis, CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, MoveAwayFromReference))));
}

////////////////////////////////////////////////////////////

void CControl::Kill()
{
	// may be in ISR context, do not print anything
	CStepper::GetInstance()->EmergencyStop();
	CMotionControlBase::GetInstance()->SetPositionFromMachine();
}

////////////////////////////////////////////////////////

void CControl::Resurrect()
{
	CStepper::GetInstance()->EmergencyStopResurrect();
	CMotionControlBase::GetInstance()->SetPositionFromMachine();

#ifdef _USE_LCD

	if (CLcd::GetInstance())
	{
		CLcd::GetInstance()->ClearDiagnostic();
	}

#endif

	_bufferIdx = 0;
	StepperSerial.println(MESSAGE_OK_RESURRECT);
}

////////////////////////////////////////////////////////////

void CControl::StopProgram(bool /*checkConditional*/) {}

////////////////////////////////////////////////////////////

void CControl::Idle(unsigned int /*idleTime*/) {}

////////////////////////////////////////////////////////////

void CControl::Hold()
{
	CStepper::GetInstance()->PauseMove();
}

////////////////////////////////////////////////////////////

void CControl::Resume()
{
	CStepper::GetInstance()->ContinueMove();
}

////////////////////////////////////////////////////////////

void CControl::Poll()
{
#ifdef _USE_LCD

	if (CLcd::GetInstance())
	{
		CLcd::GetInstance()->Poll();
	}

#endif
}

////////////////////////////////////////////////////////////

bool CControl::Parse(CStreamReader* reader, Stream* output)
{
	CGCodeParserDefault gcode(reader, output);
	return ParseAndPrintResult(&gcode, output);
}

////////////////////////////////////////////////////////////

bool CControl::ParseAndPrintResult(CParser* parser, Stream* output)
{
	bool ret = true;

	///////////////////////////////////////////////////////////////////////////
	// send OK after Parse

	parser->ParseCommand();

	if (parser->IsError())
	{
		if (output)
		{
			PrintError(output);
			output->print(parser->GetError());
			output->print(MESSAGE_CONTROL_RESULTS);
			output->print(_buffer);
			//			output->print(millis());
		}
		ret = false;
	}
	else
	{
		// => not in "else" if "OK" should be sent after "Error:"
		if (output) output->print(MESSAGE_OK);
		if (parser->GetOkMessage() != nullptr)
		{
			if (output)
			{
				output->print(F(" "));
			}
			parser->GetOkMessage()();
		}
	}

	if (output)
	{
		output->println();
	}

	return ret;
}

////////////////////////////////////////////////////////////

bool CControl::Command(char* buffer, Stream* output)
{
#ifdef _USE_LCD

	if (CLcd::GetInstance())
	{
		CLcd::GetInstance()->Command(buffer);
	}

#endif

	if (IsKilled())
	{
#ifndef REDUCED_SIZE
		if (IsResurrectCommand(buffer))		// restart with "!!!"
		{
			Resurrect();
			return true;
		}
#endif
		if (output)
		{
			PrintError(output);
			output->println(MESSAGE_CONTROL_KILLED);
			// => uncomment if "OK" will not be sent after "Error:"
			//			output->println(MESSAGE_OK);
		}
		return false;
	}

	// if one Parse fails, return false

	bool ret = true;

	_reader.Init(buffer);

	if (_reader.GetChar())
	{
		while (_reader.GetChar())
		{
			if (!Parse(&_reader, output))
			{
				ret = false;
			}
		}
	}
	else if (output)
	{
		// send OK on empty line (command)
		output->println(MESSAGE_OK_EMPTYLINE);
	}

	return ret;
}

////////////////////////////////////////////////////////////

bool CControl::IsEndOfCommandChar(char ch)
{
	//return ch == '\n' || ch == '\r' || ch == -1;
	return ch == '\n' || ch == char(-1);
}

////////////////////////////////////////////////////////////

void CControl::ReadAndExecuteCommand(Stream* stream, Stream* output, bool fileStream)
{
	// call this method if ch is available in stream

	if (stream->available() > 0)
	{
		while (stream->available() > 0)
		{
			char ch = _buffer[_bufferIdx] = stream->read();

			if (IsEndOfCommandChar(ch))
			{
				_buffer[_bufferIdx] = 0;			// remove from buffer 
				Command(_buffer, output);
				_bufferIdx = 0;

				_lastTime = millis();

				return;
			}

			_bufferIdx++;
			if (_bufferIdx >= sizeof(_buffer))
			{
				if (output)
				{
					PrintError(output);
					output->println(MESSAGE_CONTROL_FLUSHBUFFER);
				}
				_bufferIdx = 0;
			}
			/*
						if (ch == '\x18')
						{
							StepperSerial.println(MESSAGE_CTRLX);
							_bufferIdx = 0;
						}
			*/
		}

		if (fileStream)						// e.g. SD card => execute last line without "EndOfLine"
		{
			if (_bufferIdx > 0)
			{
				_buffer[_bufferIdx + 1] = 0;
				Command(_buffer, output);
				_bufferIdx = 0;
			}
		}
		_lastTime = millis();
	}
}

////////////////////////////////////////////////////////////

bool CControl::SerialReadAndExecuteCommand()
{
	if (StepperSerial.available() > 0)
	{
		ReadAndExecuteCommand(&StepperSerial, &StepperSerial, false);
	}

	return _bufferIdx > 0;		// command pending, buffer not empty
}

////////////////////////////////////////////////////////

void CControl::FileReadAndExecuteCommand(Stream* stream, Stream* output)
{
	ReadAndExecuteCommand(stream, output, true);
}

////////////////////////////////////////////////////////////

void CControl::Run()
{
	_bufferIdx = 0;
	_lastTime  = _timeBlink = _timePoll = 0;

	Init();
	Initialized();

#ifdef _MSC_VER
	while (!CGCodeParserBase::_exit)
#else
	while (true)
#endif
	{
		if (IsHold())
		{
			CheckIdlePoll(true);
		}
		else
		{
			while (SerialReadAndExecuteCommand())
			{
				// wait until serial command processed
				CheckIdlePoll(true);
			}

			CheckIdlePoll(true);

			ReadAndExecuteCommand();
		}
	}
}

////////////////////////////////////////////////////////////

void CControl::CheckIdlePoll(bool isIdle)
{
	uint32_t time = millis();

	if (isIdle && _lastTime + TIMEOUTCALLIDLE < time)
	{
		Idle(time - _lastTime);
		Poll();
		_timePoll = time;
	}
	else if (_timePoll + TIMEOUTCALLPOLL < time)
	{
		Poll();
		_timePoll = time;
	}

	if (_timeBlink < time)
	{
		HALFastdigitalWrite(BLINK_LED, CHAL::digitalRead(BLINK_LED) == HIGH ? LOW : HIGH);
		_timeBlink = time + TIMEOUTBLINK;
	}
}

////////////////////////////////////////////////////////////

void CControl::ReadAndExecuteCommand()
{
	// override for alternative command source e.g. File
}

////////////////////////////////////////////////////////////

bool CControl::PostCommand(FLSTR cmd, Stream* output)
{
	// use _buffer to execute command

	auto    cmd1       = (const char*)(cmd);
	uint8_t idx        = _bufferIdx;
	uint8_t idxProgMem = 0;

	for (; idx < sizeof(_buffer); idx++, idxProgMem++)
	{
		_buffer[idx] = pgm_read_byte(&cmd1[idxProgMem]);

		if (_buffer[idx] == 0)
		{
			return Command(&_buffer[_bufferIdx], output);
		}
	}
	
	// do not fit in buffer
	return false;
}

////////////////////////////////////////////////////////////

bool CControl::PostCommand(char* cmd, Stream* output)
{
	return Command(cmd, output);
}

////////////////////////////////////////////////////////////

void CControl::TimerInterrupt()
{
	CHAL::EnableInterrupts();	// enable irq for timer1 (Stepper)

	if (!IsKilled())
	{
		if (IsKill())
		{
			Kill();
		}
	}

#ifdef _USE_LCD

	if (CLcd::GetInstance())
	{
		CLcd::GetInstance()->TimerInterrupt();
	}

#endif
}

////////////////////////////////////////////////////////////

void CControl::Delay(uint32_t ms)
{
	uint32_t expected_end = millis() + ms;

	while (expected_end > millis())
	{
#ifdef _USE_LCD

		if (CLcd::GetInstance())
		{
			CLcd::GetInstance()->Poll();
		}

#endif
	}
}

////////////////////////////////////////////////////////////

bool CControl::StaticStepperEvent(CStepper* /*stepper*/, uintptr_t param, EnumAsByte(CStepper::EStepperEvent) eventType, uintptr_t addInfo)
{
	return ((CControl*)param)->StepperEvent(eventType, addInfo);
}

////////////////////////////////////////////////////////////

bool CControl::StepperEvent(EnumAsByte(CStepper::EStepperEvent) eventType, uintptr_t addInfo)
{
	if (CallOnEvent(eventType, addInfo))
	{
		return true;
	}

	return _oldStepperEvent.Call(CStepper::GetInstance(), eventType, addInfo);
}

////////////////////////////////////////////////////////////

bool CControl::CallOnEvent(uint8_t eventType, uintptr_t param)
{
	return OnEvent(EnumAsByte(EStepperControlEvent)(eventType), param);
}

////////////////////////////////////////////////////////////

bool CControl::OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo)
{
	switch (eventType)
	{
		case OnWaitEvent: if (CStepper::WaitTimeCritical > CStepper::EWaitType((unsigned int)addInfo))
			{
				CheckIdlePoll(false);
			}
			break;

		case OnIoEvent: IOControl(reinterpret_cast<CStepper::SIoControl*>(addInfo)->_tool, reinterpret_cast<CStepper::SIoControl*>(addInfo)->_level);
			break;
	}
	return true;
}
