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

#include "stdafx.h"

#include "..\MsvcStepper\MsvcStepper.h"
#include <RotaryButton.h>

#include "CppUnitTest.h"

////////////////////////////////////////////////////////

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StepperSystemTest
{
	TEST_CLASS(CRotaryButtonTest)
	{
	public:

		TEST_METHOD(RotaryCWTest)
		{
			CRotaryButton<signed int, 1> rotary;

			Assert::AreEqual(0, rotary.GetPos());

			// nothing changed
			auto revent = rotary.Tick(1, 1);
			Assert::AreEqual(static_cast<int>(rotary.Nothing), static_cast<int>(revent));
			Assert::AreEqual(0, rotary.GetPos());

			// CW
			revent = rotary.Tick(0, 1);
			Assert::AreEqual(static_cast<int>(rotary.RightTurn), static_cast<int>(revent));
			Assert::AreEqual(1, rotary.GetPos());

			revent = rotary.Tick(0, 0);
			Assert::AreEqual(int(rotary.RightTurn), int(revent));
			Assert::AreEqual(2, rotary.GetPos());

			revent = rotary.Tick(1, 0);
			Assert::AreEqual(int(rotary.RightTurn), int(revent));
			Assert::AreEqual(3, rotary.GetPos());

			revent = rotary.Tick(1, 1);
			Assert::AreEqual(int(rotary.RightTurn), int(revent));
			Assert::AreEqual(4, rotary.GetPos());

			revent = rotary.Tick(0, 1);
			Assert::AreEqual(int(rotary.RightTurn), int(revent));
			Assert::AreEqual(5, rotary.GetPos());
		}

		TEST_METHOD(RotaryCCWTest)
		{
			CRotaryButton<signed int, 1> rotary;
			rotary.SetMinMax(-100, 100, true);


			Assert::AreEqual(0, rotary.GetPos());

			// nothing changed
			auto revent = rotary.Tick(1, 1);
			Assert::AreEqual(int(rotary.Nothing), int(revent));
			Assert::AreEqual(0, rotary.GetPos());

			// CW
			revent = rotary.Tick(1, 0);
			Assert::AreEqual(int(rotary.LeftTurn), int(revent));
			Assert::AreEqual(-1, rotary.GetPos());

			revent = rotary.Tick(0, 0);
			Assert::AreEqual(int(rotary.LeftTurn), int(revent));
			Assert::AreEqual(-2, rotary.GetPos());

			revent = rotary.Tick(0, 1);
			Assert::AreEqual(int(rotary.LeftTurn), int(revent));
			Assert::AreEqual(-3, rotary.GetPos());

			revent = rotary.Tick(1, 1);
			Assert::AreEqual(int(rotary.LeftTurn), int(revent));
			Assert::AreEqual(-4, rotary.GetPos());

			revent = rotary.Tick(1, 0);
			Assert::AreEqual(int(rotary.LeftTurn), int(revent));
			Assert::AreEqual(-5, rotary.GetPos());
		}

		static uint8_t GetPinA(int pos)
		{
			switch (pos % 4)
			{
				default:
				case 0: return HIGH;
				case 1: return LOW;
				case 2: return LOW;
				case 3: return HIGH;
			}
		}

		static uint8_t GetPinB(int pos)
		{
			switch (pos % 4)
			{
				default:
				case 0: return HIGH;
				case 1: return HIGH;
				case 2: return LOW;
				case 3: return LOW;
			}
		}

		TEST_METHOD(RotaryOverrunCWTest)
		{
			CRotaryButton<signed int, 1> rotary;
			rotary.SetMinMax(0, 15, true);

			for (int i = 0; i < 257; i++)
			{
				rotary.Tick(GetPinA(i), GetPinB(i));
				Assert::AreEqual(i % 16, rotary.GetPos());
			}
		}

		TEST_METHOD(RotaryOverrunCCWTest)
		{
			CRotaryButton<signed int, 1> rotary;
			rotary.SetMinMax(0, 15, true);

			for (int i = 256; i > 0; i--)
			{
				rotary.Tick(GetPinA(i), GetPinB(i));
				Assert::AreEqual(i % 16, rotary.GetPos());
			}
		}

		TEST_METHOD(RotaryAccuracCWTest)
		{
			const int ACCURACY = 4;

			CRotaryButton<signed int, ACCURACY> rotary;

			int i;
			for (i = 0; i < 126; i++)
			{
				rotary.Tick(GetPinA(i), GetPinB(i));

				Assert::AreEqual(i, rotary.GetFullRangePos());
				Assert::AreEqual((i + ACCURACY / 2) / ACCURACY, rotary.GetPos());
			}
			Assert::AreEqual(int(rotary.Overrun), int(rotary.Tick(GetPinA(i), GetPinB(i))));
			int pos = i - ACCURACY;
			Assert::AreEqual(pos, rotary.GetFullRangePos());
			Assert::AreEqual((pos + ACCURACY / 2) / ACCURACY, rotary.GetPos());
		}
	};
}
