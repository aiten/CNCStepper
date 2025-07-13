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

#include <Control.h>
#include <ConfigEeprom.h>

#include <OnOffIOControl.h>
#include <Analog8IOControl.h>
#include <Analog8IOControlSmooth.h>
#include <Analog8XIOControlSmooth.h>
#include <ReadPinIOControl.h>
#include <PushButtonLow.h>
#include <DummyIOControl.h>

////////////////////////////////////////////////////////

#ifndef SPINDLE_FADETIME
#define SPINDLE_FADETIME 8
#endif

#ifndef PROBE_INPUTPINMODE
#define PROBE_INPUTPINMODE INPUT_PULLUP
#endif
#ifndef KILL_INPUTPINMODE
#define KILL_INPUTPINMODE INPUT_PULLUP
#endif

////////////////////////////////////////////////////////

struct ControlData
{
#ifdef SPINDLE_ENABLE_PIN
#ifdef SPINDLE_ANALOGSPEED
#ifdef SPINDLE_ISLASER
	CAnalog8IOControl<SPINDLE_ENABLE_PIN> _spindle;
	inline uint8_t                        ConvertSpindleSpeedToIO(uint16_t level) { return (uint8_t)level; }
#elif defined(SPINDLE_FADE)
#ifdef SPINDLE_DIR_PIN
	CAnalog8XIOControlSmooth<SPINDLE_ENABLE_PIN, SPINDLE_DIR_PIN> _spindle;
	inline int16_t                                                ConvertSpindleSpeedToIO(uint16_t level) { return CControl::ConvertSpindleSpeedToIO8(CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, MaxSpindleSpeed)), level); }
#undef SPINDLE_DIR_PIN
#define SPINDLESPEEDISINT
#define SPINDLESMOOTH
#else
				CAnalog8IOControlSmooth<SPINDLE_ENABLE_PIN> _spindle;
				inline uint8_t ConvertSpindleSpeedToIO(uint16_t level) { return CControl::ConvertSpindleSpeedToIO8(CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, MaxSpindleSpeed)), level); }
			#define SPINDLESMOOTH
#endif
#else
			CAnalog8IOControl<SPINDLE_ENABLE_PIN> _spindle;
			inline uint8_t ConvertSpindleSpeedToIO(uint16_t level) { return CControl::ConvertSpindleSpeedToIO8(CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, MaxSpindleSpeed)), level); }
#endif
#else
	COnOffIOControl<SPINDLE_ENABLE_PIN, SPINDLE_DIGITAL_ON, SPINDLE_DIGITAL_OFF> _spindle;

	inline uint8_t ConvertSpindleSpeedToIO(uint16_t level) { return (uint8_t)level; }
#endif
#ifdef SPINDLE_DIR_PIN
	COnOffIOControl<SPINDLE_DIR_PIN, SPINDLE_DIR_CLW, SPINDLE_DIR_CCLW> _spindleDir;
#else
	CDummyIOControl _spindleDir;
#endif
#else
	CDummyIOControl _spindle;
	CDummyIOControl _spindleDir;
	inline uint8_t  ConvertSpindleSpeedToIO(uint16_t level) { return uint8_t(level); }
#endif

#ifdef COOLANT_PIN
	COnOffIOControl<COOLANT_PIN, COOLANT_PIN_ON, COOLANT_PIN_OFF> _coolant;
#else
	CDummyIOControl _coolant;
#endif
#ifdef PROBE_PIN
	CReadPinIOControl<PROBE_PIN, PROBE_PIN_ON> _probe;
#else
	CDummyIOControl _probe;
#endif

#ifdef KILL_PIN
#ifdef KILL_ISTRIGGER
	CReadPinIOTriggerControl<KILL_PIN, KILL_PIN_ON, 200> _kill;
#else
	CReadPinIOControl<KILL_PIN, KILL_PIN_ON> _kill;
#endif
#else
	CDummyIOControl _kill;
#endif

#if defined(HOLD_PIN) && defined(RESUME_PIN)
	CPushButtonLow<HOLD_PIN, HOLD_PIN_ON>     _hold;
	CPushButtonLow<RESUME_PIN, RESUME_PIN_ON> _resume;
#else
	CDummyIOControl _hold;
	CDummyIOControl _resume;
#endif

#if defined(HOLDRESUME_PIN)
	CPushButtonLow<HOLDRESUME_PIN, HOLDRESUME_PIN_ON> _holdresume;
#else
	CDummyIOControl _holdresume;
#endif

#ifdef CONTROLLERFAN_FAN_PIN
#ifdef CONTROLLERFAN_ANALOGSPEED
#ifdef CONTROLLERFAN_ANALOGSPEED_INVERT
	CAnalog8InvertIOControl<CONTROLLERFAN_FAN_PIN> _controllerfan;
#else
	CAnalog8IOControl<CONTROLLERFAN_FAN_PIN> _controllerfan;
#endif
#else
	COnOffIOControl<CONTROLLERFAN_FAN_PIN, CONTROLLERFAN_DIGITAL_ON, CONTROLLERFAN_DIGITAL_OFF> _controllerfan;
#endif
	inline bool IsControllerFanTimeout() { return (millis() - CStepper::GetInstance()->IdleTime()) > (CStepper::GetInstance()->GetEnableTimeoutInMs() + 1000); }
#else
	CDummyIOControl _controllerfan;
	inline bool     IsControllerFanTimeout() { return false; }
#endif

	inline void Init()
	{
		_controllerfan.Init(255);

		_spindle.Init();
#ifdef SPINDLESMOOTH
		_spindle.SetDelay(CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, SpindleFadeTime)));
#endif
		_probe.Init(PROBE_INPUTPINMODE);
		_kill.Init(KILL_INPUTPINMODE);
		_coolant.Init();

		_hold.Init();
		_resume.Init();
		_holdresume.Init();
	}

	inline bool IOControl(uint8_t tool, uint16_t level)
	{
		switch (tool)
		{
#ifdef SPINDLESPEEDISINT
			case CControl::SpindleCW: _spindle.On(ConvertSpindleSpeedToIO(level));
				return true;
			case CControl::SpindleCCW: _spindle.On(-ConvertSpindleSpeedToIO(level));
				return true;
#else

			case CControl::SpindleCW:
			case CControl::SpindleCCW:
			{
				_spindle.On(ConvertSpindleSpeedToIO(level));
				_spindleDir.Set(tool == CControl::SpindleCCW);
				return true;
			}
#endif
			case CControl::Coolant:
			{
				_coolant.Set(level > 0);
				return true;
			}
			case CControl::ControllerFan:
			{
				_controllerfan.SetLevel(uint8_t(level));
				return true;
			}

			default: break;
		}
		return false;
	}

	inline void Kill()
	{
		_spindle.Off();
		_coolant.Set(false);
	}

	inline bool IsKill()
	{
		if (_kill.IsOn())
		{
			return true;
		}
		return false;
	}

	inline void TimerInterrupt()
	{
		_hold.Check();
		_resume.Check();
		_holdresume.Check();
		_spindle.Poll();
	}

	inline void Initialized()
	{
		_controllerfan.SetLevel(128);
	}

	inline void OnEvent(EnumAsByte(CControl::EStepperControlEvent) eventType, uintptr_t /* addInfo */)
	{
		switch (eventType)
		{
			case CControl::OnStartEvent:
				_controllerfan.On();
				break;
			case CControl::OnIdleEvent:
				if (IsControllerFanTimeout())
				{
					_controllerfan.Off();
				}
				break;

			default: break;
		}
	}
};

////////////////////////////////////////////////////////

constexpr uint16_t GetInfo1a()
{
	return
#ifdef SPINDLE_ENABLE_PIN
		CConfigEeprom::HAVE_SPINDLE |
#ifdef SPINDLE_ANALOGSPEED
		CConfigEeprom::HAVE_SPINDLE_ANALOG |
#endif
#if defined(SPINDLE_DIR_PIN) || defined(SPINDLESPEEDISINT)
		CConfigEeprom::HAVE_SPINDLE_DIR |
#endif
#endif
#ifdef SPINDLE_ISLASER
		CConfigEeprom::IS_LASER |
#endif
#ifdef COOLANT_PIN
		CConfigEeprom::HAVE_COOLANT |
#endif
#ifdef PROBE_PIN
		CConfigEeprom::HAVE_PROBE |
#endif
#ifdef KILL_PIN
		CConfigEeprom::HAVE_KILL |
#endif
#ifdef HOLD_PIN
		CConfigEeprom::HAVE_HOLD |
#endif
#ifdef RESUME_PIN
		CConfigEeprom::HAVE_RESUME |
#endif
#ifdef HOLDRESUME_PIN
		CConfigEeprom::HAVE_HOLDRESUME |
#endif
#ifndef REDUCED_SIZE
		CConfigEeprom::CAN_ROTATE |
#endif
#if defined(__AVR_ARCH__) || defined(__SAMD21G18A__) || defined(ESP32)
		CConfigEeprom::HAVE_EEPROM |
#elif  defined(__SAM3X8E__)
		//		(CHAL::HaveEeprom() ? CConfigEeprom::HAVE_EEPROM : 0) |
#endif
#ifdef MYUSE_LCD
		CConfigEeprom::HAVE_SD |
#endif
#ifdef REDUCED_SIZE
		COMMANDSYNTAX_VALUE(CConfigEeprom::GCodeBasic) |
#else
		COMMANDSYNTAX_VALUE(CConfigEeprom::GCode) |
#endif
		CConfigEeprom::NONE1a;
}

constexpr uint16_t GetInfo1b()
{
	return
#if defined(__SAMD21G18A__) || defined(ESP32)
	CConfigEeprom::EEPROM_NEED_EEPROM_FLUSH |
	CConfigEeprom::EEPROM_NEED_DTR |
#else
		CConfigEeprom::EEPROM_DTRISRESET |
#endif
#ifndef REDUCED_SIZE
		WORKOFFSETCOUNT_VALUE(G54ARRAYSIZE) |
#endif
	CConfigEeprom::NONE1b;
}
