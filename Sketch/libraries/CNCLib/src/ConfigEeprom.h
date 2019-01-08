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
*/
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include <StepperLib.h>
#include "ConfigurationCNCLib.h"

////////////////////////////////////////////////////////

#ifdef REDUCED_SIZE

typedef uint8_t eepromofs_t;
#define AXISPARAMETER(a,b,c,d,e,f,g,h,i,j,k,l) a,b,c,d,e,f

#else

typedef uint16_t eepromofs_t;
#define AXISPARAMETER(a,b,c,d,e,f,g,h,i,j,k,l) a,b,c,d,e,f,g,h,i,j,k,l

#endif

enum EReverenceType
{
	NoReference,
	ReferenceToMin,
	ReferenceToMax
};

class CConfigEeprom : public CSingleton<CConfigEeprom>
{
private:

	uint16_t    _eepromSize;
	const void* _defaultEeprom;

	bool _eepromValid    = false;
	bool _eepromCanWrite = false;

public:

	CConfigEeprom() { };

	CConfigEeprom(uint16_t eepromSize, const void* defaultEeprom, uint32_t eepromID)
	{
		Init(eepromSize, defaultEeprom, eepromID);
	}

	void Init(uint16_t eepromSize, const void* defaultEeprom, uint32_t eepromID);
	void Flush();

	static uint32_t GetConfigU32(eepromofs_t);

#if defined(__AVR_ARCH__)

	static uint8_t  GetConfigU8(eepromofs_t ofs)  { return (uint8_t)GetConfigU32(ofs); };
	static uint16_t GetConfigU16(eepromofs_t ofs) { return (uint16_t)GetConfigU32(ofs); };

#else

	static uint8_t GetConfigU8(eepromofs_t ofs)
	{
		// must be dword aligned
		eepromofs_t diff = ofs % 4;
		uint32_t    val  = GetConfigU32(ofs - diff);
		return (val >> (diff * 8)) & 0xff;
	};

	static uint16_t GetConfigU16(eepromofs_t ofs)
	{
		// must be dword aligned
		// must be in this 32bit value (diff can only be 0,1,2 and not 3)
		eepromofs_t diff = ofs % 4;
		uint32_t    val  = GetConfigU32(ofs - diff);
		return (val >> (diff * 8)) & 0xffff;
	}

#endif

	static float GetConfigFloat(eepromofs_t);

	void PrintConfig();

	bool ParseConfig(class CParser*);

	uint32_t GetConfig32(eepromofs_t ofs) const;

private:

	void FlushConfig();
	void SetConfig32(eepromofs_t ofs, uint32_t value);

public:

	enum ECommandSyntax
	{
		GCodeBasic = 0,
		GCode = 1,
		HPGL = 7,
		// max 3 bit
	};

	enum EEpromInfo1a
	{
		NONE1a = 0,
		HAVE_SPINDLE = (1 << 0),
		HAVE_SPINDLE_ANALOG = (1 << 1),
		HAVE_SPINDLE_DIR = (1 << 2),
		HAVE_COOLANT = (1 << 3),
		HAVE_PROBE = (1 << 4),

		IS_LASER = (1 << 5),

		COMMANDSYNTAXBIT0 = (1 << 6),
		COMMANDSYNTAXBIT1 = (1 << 7),
		COMMANDSYNTAXBIT2 = (1 << 8),

		HAVE_EEPROM = (1 << 9),
		HAVE_SD = (1 << 10),
		CAN_ROTATE = (1 << 11),

		HAVE_HOLDRESUME = (1 << 12),
		HAVE_HOLD = (1 << 13),
		HAVE_RESUME = (1 << 14),
		HAVE_KILL = (1 << 15)
	};

	enum EEpromInfo1b
	{
		NONE1b = 0,
		EEPROM_NEED_EEPROM_FLUSH = (1 << 0),
		EEPROM_NEED_DTR = (1 << 1),			// deprecated, replaced by EEPROM_DTRISRESET
		EEPROM_DTRISRESET = (1 << 2),
	};

#define COMMANDSYNTAX_BIT	6
#define COMMANDSYNTAX_LEN	2
#define COMMANDSYNTAX_MASK  (CConfigEeprom::COMMANDSYNTAXBIT0+CConfigEeprom::COMMANDSYNTAXBIT1+CConfigEeprom::COMMANDSYNTAXBIT2)
#define COMMANDSYNTAX_VALUE(a)	(((a)*(1<<COMMANDSYNTAX_BIT))&COMMANDSYNTAX_MASK)
#define COMMANDSYNTAX_CLEAR(a)	((a)&~COMMANDSYNTAX_MASK)

#define EPROM_SIGNATURE		0x21436502

	struct SCNCEeprom
	{
		uint32_t Signature;

		uint8_t NumAxis;
		uint8_t UsedAxis;
		uint8_t OffsetAxis;
		uint8_t SizeOfAxis;

		uint16_t Info1a;
		uint16_t Info1b;
		uint32_t Info2;

		uint8_t StepperDirections; // bits for each axis, see CStepper::SetDirection
		uint8_t Dummy2;
		uint8_t Dummy3;
		uint8_t SpindleFadeTime;

		uint16_t MaxSpindleSpeed;
		uint16_t JerkSpeed;

		uint32_t MaxStepRate;
		uint16_t Acc;
		uint16_t Dec;
		uint32_t RefMoveStepRate;
		uint32_t MoveAwayFromReference;

		float StepsPerMm1000;

		struct SAxisDefinitions
		{
			mm1000_t Size;

			uint8_t ReferenceType; // EReverenceType
			uint8_t RefmoveSequence;

			uint8_t ReferenceValueMin;
			uint8_t ReferenceValueMax;

			uint32_t InitPosition;

#ifndef REDUCED_SIZE

			uint32_t MaxStepRate;
			uint16_t Acc;
			uint16_t Dec;
			uint32_t RefMoveStepRate;

			float StepsPerMm1000;

			mm1000_t ProbeSize;
#endif
		} Axis[NUM_AXIS];
	};
};

////////////////////////////////////////////////////////
