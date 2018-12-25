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

#pragma once

////////////////////////////////////////////////////////

#include <StepperLib.h>
#include "ConfigurationCNCLib.h"
#include "Parser.h"
#include "Lcd.h"
#include "MenuBase.h"

////////////////////////////////////////////////////////

#ifdef REDUCED_SIZE
#define CGCodeParserDefault CGCodeParserBase
#define CMotionControlDefault CMotionControlBase
#else
#define CGCodeParserDefault CGCodeParser
#define CMotionControlDefault CMotionControl
#endif

////////////////////////////////////////////////////////
// Control: read from USB (or File) and pass string to the Parser

class CControl : public CSingleton<CControl>
{
public:

	CControl();

	virtual void Run();				// run Controller => do not return
	virtual void Kill();			// stop everything => Emergency Stop
	virtual void Resurrect();		// Call after Kill to restart again

	bool IsKilled() { return CStepper::GetInstance()->IsEmergencyStop(); }

	void StopProgram(bool checkConditional);		// see m00 / m01

	void Delay(uint32_t ms);		// delay with idle processing

	void Hold();					// stop executing program (with down-ramp), must not be called in timer interrupt
	void Resume();					// continue executing (start queue from stepper)
	bool IsHold() { return CStepper::GetInstance()->IsPauseMove(); }

	//////////////////////////////////////////

	enum EIOTools
	{
		// Output
		SpindleCW,
		SpindleCCW,
		Coolant,
		ControllerFan,
		Vacuum,
		//Laser, => use Spindle

		Servo1,
		Servo2,
		Servo3,
		Servo4,

		User1,
		User2,
		User3,
		User4,

		// input
		Probe,				// Probe for tool length
		ProgramStop			// see M01
	};

	enum ECoolantState
	{
		CoolantOff = 0,
		CoolantOn = 1,
		CoolantFlood = 2
	};

	enum EVacuumState
	{
		VacuumOff = 0,
		VacuumOn = 1
	};

	virtual void     IOControl(uint8_t /* tool */, uint16_t /*level */) { };
	virtual uint16_t IOControl(uint8_t /* tool */) { return 0; };

	enum EStepperControlEvent
	{
		OnStartEvent = CStepper::OnStartEvent,
		OnIdleEvent = CStepper::OnIdleEvent,
		OnDisableEvent = CStepper::OnDisableEvent,
		OnWaitEvent = CStepper::OnWaitEvent,
		OnErrorEvent = CStepper::OnErrorEvent,
		OnWarningEvent = CStepper::OnWarningEvent,
		OnInfoEvent = CStepper::OnInfoEvent,
		OnIoEvent = CStepper::OnIoEvent,

		OnStartCut
	};
	//////////////////////////////////////////

	REDUCED_SIZE_virtual void GoToReference();								// Goto Reference during init
	REDUCED_SIZE_virtual bool GoToReference(axis_t axis, steprate_t stepRate, bool toMinRef);

	bool GoToReference(axis_t axis);

	//////////////////////////////////////////

	void           InitFromEeprom();
	static uint8_t ConvertSpindleSpeedToIO8(uint16_t maxspeed, uint16_t level); // { return (uint8_t)MulDivU32(abs(level), 255, maxspeed); }

	//////////////////////////////////////////

	void StartPrintFromSD() { _printFromSDFile = true; }
	void ClearPrintFromSD() { _printFromSDFile = false; }
	bool PrintFromSDRunning() const { return _printFromSDFile; }

	//////////////////////////////////////////

	bool PostCommand(FLSTR cmd, Stream* output = nullptr);
	bool PostCommand(char* cmd, Stream* output = nullptr);

	//////////////////////////////////////////

	const char*  GetBuffer() const { return _buffer; }
	uint8_t      GetBufferCount() const { return _bufferIdx; }
	
	REDUCED_SIZE_virtual bool IsEndOfCommandChar(char ch);					// override default End of command char, default \n

	virtual void PrintVersion();

protected:

	bool SerialReadAndExecuteCommand();							// read from serial an execute command, return true if command pending (buffer not empty)
	void FileReadAndExecuteCommand(Stream* stream, Stream* output);// read command until "IsEndOfCommandChar" and execute command (NOT Serial)

	virtual void Init();
	virtual void Initialized();									// called if Init() is done

	REDUCED_SIZE_virtual bool Parse(CStreamReader* reader, Stream* output);	// specify Parser, default parser
	REDUCED_SIZE_virtual bool Command(char*        buffer, Stream* output);	// execute Command (call parser)
	
	virtual void Idle(unsigned int    idleTime);				// called after TIMEOUTCALLIDLE in idle state
	virtual void Poll();										// call in Idle and at least e.g. 100ms (not in interrupt), see CheckIdlePoll
	
	REDUCED_SIZE_virtual void ReadAndExecuteCommand();			// read and execute commands from other source e.g. SD.File

	virtual void TimerInterrupt();								// called from timer (timer0 on AVR) 

	virtual bool IsKill() = 0;

	bool ParseAndPrintResult(CParser* parser, Stream* output);

	virtual bool OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo);

	bool IsResurrectCommand(const char* buffer) { return buffer[0] == '!' && buffer[1] == '!' && buffer[2] == '!' && (buffer[3] == 0 || (buffer[3] == '\r' && buffer[4] == 0)); }

	void DisableBlinkLed() { _timeBlink = 0xffffffff; }

private:

	void ReadAndExecuteCommand(Stream* stream, Stream* output, bool fileStream);	// read command until "IsEndOfCommandChar" and execute command (Serial or SD.File)

	void CheckIdlePoll(bool isIdle);						// check idle time and call Idle every 100ms


	uint8_t _bufferIdx;										// read Buffer index , see SERIALBUFFERSIZE

	uint32_t _lastTime;										// time last char received
	uint32_t _timeBlink;									// time to change blink state
	uint32_t _timePoll;										// time call poll next

	CStepper::SEvent _oldStepperEvent;

	bool _dummy;
	bool _printFromSDFile;

	char _buffer[SERIALBUFFERSIZE];							// serial input buffer

	static void HandleInterrupt() { GetInstance()->TimerInterrupt(); }

	static bool StaticStepperEvent(CStepper* stepper, uintptr_t param, EnumAsByte(CStepper::EStepperEvent) eventType, uintptr_t addInfo);

	bool StepperEvent(EnumAsByte(CStepper::EStepperEvent) eventType, uintptr_t addInfo);

	CStreamReader _reader;

	void PrintError(Stream* output) { output->print(MESSAGE_ERROR); }

public:

	bool CallOnEvent(uint8_t eventType, uintptr_t param);
};

////////////////////////////////////////////////////////
