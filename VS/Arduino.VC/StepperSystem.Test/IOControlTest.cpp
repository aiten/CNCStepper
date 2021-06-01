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
#include <Analog8IOControlSmooth.h>
#include <Analog8IOControl.h>
#include <Analog8InvertIOControl.h>
#include <Analog8XIOControlSmooth.h>

#include "CppUnitTest.h"

////////////////////////////////////////////////////////

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StepperSystemTest
{
	TEST_CLASS(CIOControlTest)
	{
	public:

		TEST_METHOD(AnalogIOTest)
		{
			CAnalog8IOControl<10> spindle;

			spindle.Init(0);

			Assert::AreEqual(false, spindle.IsOn());
			Assert::AreEqual(uint8_t(0), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());

			spindle.On(0);
			Assert::AreEqual(uint8_t(0), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());

			spindle.OnMax();
			Assert::AreEqual(uint8_t(255), spindle.GetLevel());
			Assert::AreEqual(uint8_t(255), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.Off();
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());
			Assert::AreEqual(false, spindle.IsOn());

			spindle.On();
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			spindle.SetLevel(111);
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(uint8_t(111), spindle.GetLevel());
		}

		TEST_METHOD(AnalogIOInvertTest)
		{
			CAnalog8InvertIOControl<10> spindle;

			spindle.Init(0);

			Assert::AreEqual(false, spindle.IsOn());
			Assert::AreEqual(uint8_t(0), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());

			spindle.On(0);
			Assert::AreEqual(uint8_t(0), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());

			spindle.OnMax();
			Assert::AreEqual(uint8_t(255), spindle.GetLevel());
			Assert::AreEqual(uint8_t(255), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.Off();
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());
			Assert::AreEqual(false, spindle.IsOn());

			spindle.On();
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			spindle.SetLevel(111);
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(uint8_t(111), spindle.GetLevel());
		}

		TEST_METHOD(AnalogIOSmoothTest)
		{
			CAnalog8IOControlSmooth<10> spindle;
			spindle.SetDelay(1);

			spindle.Init(0);

			Assert::AreEqual(false, spindle.IsOn());
			Assert::AreEqual(uint8_t(0), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());

			spindle.On(0);
			Assert::AreEqual(uint8_t(0), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());

			spindle.OnMax();
			Assert::AreEqual(uint8_t(255), spindle.GetLevel());
			Assert::AreEqual(uint8_t(255), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.Off();
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetIOLevel());
			Assert::AreEqual(false, spindle.IsOn());

			spindle.On();
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			spindle.SetLevel(111);
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(uint8_t(111), spindle.GetLevel());
		}

		TEST_METHOD(AnalogIOSmoothFadeTest)
		{
			CAnalog8IOControlSmooth<10> spindle;
			spindle.SetDelay(255);

			spindle.Init(100);
			Assert::AreEqual(uint8_t(100), spindle.GetLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
			Assert::AreEqual(uint8_t(0), spindle.GetCurrentIOLevel());

			// delay is 0 => inc each call to "poll"

			for (uint8_t i = 1; i < 100; i++)
			{
				spindle.PollForce();
				Assert::AreEqual(uint8_t(100), spindle.GetLevel());
				Assert::AreEqual(uint8_t(100), spindle.GetIOLevel());
				Assert::AreEqual(i, spindle.GetCurrentIOLevel());
			}
			spindle.PollForce();
			Assert::AreEqual(uint8_t(100), spindle.GetCurrentIOLevel());
			spindle.PollForce();
			Assert::AreEqual(uint8_t(100), spindle.GetCurrentIOLevel());

			spindle.On(50);
			Assert::AreEqual(uint8_t(50), spindle.GetLevel());
			Assert::AreEqual(uint8_t(50), spindle.GetIOLevel());
			Assert::AreEqual(uint8_t(100), spindle.GetCurrentIOLevel());

			for (uint8_t i = 99; i > 50; i--)
			{
				spindle.PollForce();
				Assert::AreEqual(uint8_t(50), spindle.GetLevel());
				Assert::AreEqual(uint8_t(50), spindle.GetIOLevel());
				Assert::AreEqual(i, spindle.GetCurrentIOLevel());
			}
		}

		TEST_METHOD(Analog9IOSmoothTest)
		{
			CAnalog8XIOControlSmooth<10, 11> spindle;
			spindle.SetDelay(255);	// never reached => we use PollForce

			spindle.Init(0);

			Assert::AreEqual(false, spindle.IsOn());
			Assert::AreEqual(int16_t(0), spindle.GetLevel());
			Assert::AreEqual(int16_t(0), spindle.GetIOLevel());

			spindle.On(0);
			Assert::AreEqual(int16_t(0), spindle.GetLevel());
			Assert::AreEqual(int16_t(0), spindle.GetIOLevel());

			spindle.OnMax();
			Assert::AreEqual(int16_t(255), spindle.GetLevel());
			Assert::AreEqual(int16_t(255), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(-100);
			Assert::AreEqual(int16_t(-100), spindle.GetLevel());
			Assert::AreEqual(int16_t(-100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			Assert::AreEqual(int16_t(100), spindle.GetLevel());
			Assert::AreEqual(int16_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.Off();
			Assert::AreEqual(int16_t(100), spindle.GetLevel());
			Assert::AreEqual(int16_t(0), spindle.GetIOLevel());
			Assert::AreEqual(false, spindle.IsOn());

			spindle.On();
			Assert::AreEqual(int16_t(100), spindle.GetLevel());
			Assert::AreEqual(int16_t(100), spindle.GetIOLevel());
			Assert::AreEqual(true, spindle.IsOn());

			spindle.On(100);
			spindle.SetLevel(111);
			Assert::AreEqual(int16_t(100), spindle.GetIOLevel());
			Assert::AreEqual(int16_t(111), spindle.GetLevel());
		}

		TEST_METHOD(Analog9IOSmoothFadeTest)
		{
			CAnalog8XIOControlSmooth<10, 11> spindle;
			spindle.SetDelay(255);	// never reached => we use PollForce

			spindle.Init(CHAR_MAX);
			Assert::AreEqual(int16_t(CHAR_MAX), spindle.GetLevel());
			Assert::AreEqual(int16_t(CHAR_MAX), spindle.GetIOLevel());
			Assert::AreEqual(int16_t(0), spindle.GetCurrentIOLevel());

			for (int16_t i = 1; i <= CHAR_MAX; i++)
			{
				spindle.PollForce();
				Assert::AreEqual(int16_t(CHAR_MAX), spindle.GetLevel());
				Assert::AreEqual(int16_t(CHAR_MAX), spindle.GetIOLevel());
				Assert::AreEqual(i, spindle.GetCurrentIOLevel());
			}

			spindle.PollForce();
			Assert::AreEqual(int16_t(CHAR_MAX), spindle.GetCurrentIOLevel());
			spindle.PollForce();
			Assert::AreEqual(int16_t(CHAR_MAX), spindle.GetCurrentIOLevel());

			spindle.On(CHAR_MIN);
			Assert::AreEqual(int16_t(CHAR_MIN), spindle.GetLevel());
			Assert::AreEqual(int16_t(CHAR_MIN), spindle.GetIOLevel());
			Assert::AreEqual(int16_t(CHAR_MAX), spindle.GetCurrentIOLevel());

			for (int16_t i = CHAR_MAX - 1; i >= CHAR_MIN; i--)
			{
				spindle.PollForce();
				Assert::AreEqual(int16_t(CHAR_MIN), spindle.GetLevel());
				Assert::AreEqual(int16_t(CHAR_MIN), spindle.GetIOLevel());
				Assert::AreEqual(i, spindle.GetCurrentIOLevel());
			}
		}
	};
}
