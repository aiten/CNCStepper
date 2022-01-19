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

#include "CppUnitTest.h"

#include "..\MsvcStepper\MsvcStepper.h"

////////////////////////////////////////////////////////

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StepperSystemTest
{
	TEST_CLASS(CMotionControlTest)
	{
	public:

		CMsvcStepper Stepper;

		TEST_METHOD(MotionControlTest)
		{
			Stepper.Init();
			Stepper.UseSpeedSign = true;

			for (axis_t x = 0; x < NUM_AXIS; x++)
			{
				Stepper.SetLimitMax(x, 0x100000);
			}
			Stepper.SetWaitFinishMove(false);

			FeedRateTest();
		}

		void FeedRateTest() const
		{
			CMotionControlBase mc;
			mc.UnitTest();
			mc.InitConversion(
				[](axis_t, sdist_t  val) { return static_cast<mm1000_t>(val); },
				[](axis_t, mm1000_t val) { return sdist_t(val); }
			);

			mm1000_t to1[NUM_AXIS] = { 1000, 0, 0 };
			mm1000_t to2[NUM_AXIS] = { 1000, 2000, 0 };
			mm1000_t to3[NUM_AXIS] = { 1000, 2000, 3000 };

			Assert::AreEqual(long(1234), long(mc.CalcStepRate(to1, 1234 * 60)));

			Assert::AreEqual(long(1103), long(mc.CalcStepRate(to2, 1234 * 60)));		// 2000/sqrt(1000*1000 + 2000*2000) * 1234

			Assert::AreEqual(long(989), long(mc.CalcStepRate(to3, 1234 * 60)));
		}

		TEST_METHOD(FeedRateDistOverrunTest)
		{
			CMotionControlBase mc;
			mc.UnitTest();
			mc.InitConversion(
				[](axis_t, sdist_t  val) { return mm1000_t(val); },
				[](axis_t, mm1000_t val) { return sdist_t(val); }
			);

			mm1000_t to1[] = { 100000, 0, 0 };
			mm1000_t to2[] = { 100000, 20000, 0 };
			mm1000_t to3[] = { 100000, 20000, 30000 };

			Assert::AreEqual(long(1234), long(mc.CalcStepRate(to1, 1234 * 60)));

			Assert::AreEqual(long(1209), long(mc.CalcStepRate(to2, 1234 * 60)));

			Assert::AreEqual(long(1159), long(mc.CalcStepRate(to3, 1234 * 60)));
		}

		TEST_METHOD(FeedRateOverrun16BitTest)
		{
			CMotionControlBase mc;
			mc.UnitTest();
			mc.InitConversion(
				[](axis_t, sdist_t  val) { return mm1000_t(val) * 10; },
				[](axis_t, mm1000_t val) { return sdist_t(val) / 10; }
			);

			mm1000_t to1[] = { 100, 0, 0 };
			mm1000_t to2[] = { 100, 200, 0 };
			mm1000_t to3[] = { 100, 200, 300 };

			Assert::AreEqual(long(12345), long(mc.CalcStepRate(to1, 123456 * 60)));

			Assert::AreEqual(long(11022), long(mc.CalcStepRate(to2, 123456 * 60)));

			Assert::AreEqual(long(9902), long(mc.CalcStepRate(to3, 123456 * 60)));
		}
	};
}
