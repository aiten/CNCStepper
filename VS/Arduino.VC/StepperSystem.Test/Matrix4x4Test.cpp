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

#include <Matrix4x4.h>

#include "CppUnitTest.h"

////////////////////////////////////////////////////////

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StepperSystemTest
{
	TEST_CLASS(CMatrix4x4Test)
	{
	public:

		bool CompareMatrix(CMatrix4x4<float>& m, float in1[][4], float out1[][4], uint8_t size) const
		{
			for (uint8_t i = 0; i < size; i++)
			{
				float v2[4];
				m.Mul(in1[i], v2);

				for (uint8_t n = 0; n < 4; n++)
				{
					if (!CMatrix4x4<float>::IsEqual(out1[i][n], v2[n], 0.00001f))
					{
						return false;
					}
				}
			}
			return true;
		}

		TEST_METHOD(Matrix4x4Test)
		{
			float out[4];

			float t[4][4]  = { { 1.0, 2.0, 3.0, 4.0 }, { 5.0, 6.0, 7.0, 8.0 }, { 9.0, 10.0, 11.0, 12.0 }, { 13.0, 14.0, 15.0, 16.0 } };
			float t2[4][4] = { { 5.0, 4.0, 3.0, 2.0 }, { 9.0, 8.0, 7.0, 6.0 }, { 13.0, 12.0, 11.0, 10.0 }, { 17.0, 16.0, 15.0, 14.0 } };

			//////////////////////////////////////////
			//Test Compare
			{
				CMatrix4x4<float> T1(t);
				CMatrix4x4<float> T2(t);

				if (T1 == T2)
				{
					if (T1 != T2)
					{
						Assert::Fail(L"Error Compare\n");
					}
				}
				else
				{
					Assert::Fail(L"Error Compare\n");
				}

				T1.Set(3, 3, 1.2345f);

				if (T1 == T2)
				{
					Assert::Fail(L"Error Compare\n");
				}
				else { }
			}

			//////////////////////////////////////////

			{
				CMatrix4x4<float> T1(t);
				CMatrix4x4<float> T2(t);

				CMatrix4x4<float> T3 = T1 * T2;

				float r[4][4] = { { 90.0, 100.0, 110.0, 120.0 }, { 202.0, 228.0, 254.0, 280.0 }, { 314.0, 356.0, 398.0, 440.0 }, { 426.0, 484.0, 542.0, 600.0 } };

				CMatrix4x4<float> T4(r);

				if (T3 != T4)
				{
					Assert::Fail(L"Error Mul\n");
				}
			}

			//////////////////////////////////////////

			{
				CMatrix4x4<float> T1(t);
				CMatrix4x4<float> T2(t2);
				CMatrix4x4<float> T3 = T2 * T1;

				T2 *= T1;

				if (T2 != T3)
				{
					Assert::Fail(L"Error Mul\n");
				}
			}

			//////////////////////////////////////////
			{
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg1Rot(float(M_PI_4));
				CMatrix4x4<float> T2;
				T2.InitDenavitHartenberg1Rot(-float(M_PI_4));

				CMatrix4x4<float> T3 = T1 * T2;
				CMatrix4x4<float> T4;
				T4.InitDenavitHartenbergNOP();


				if (!T3.IsEqual(T4, 0.0000001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg1Rot\n");
				}
			}

			//////////////////////////////////////////
			{
				const float       angle = float(M_PI / 5);
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg1Rot(angle);

				float in1[][4] = {
					{ 1, 0, 0, 1 },
					{ 0, 1, 0, 1 },
					{ 0, 0, 1, 1 },
					{ 1, 1, 0, 1 }
				};
				float out1[][4] = {
					{ cos(angle), sin(angle), 0, 1 },
					{ -sin(angle), cos(angle), 0, 1 },
					{ 0, 0, 1, 1 },
					{ cos(angle) - sin(angle), cos(angle) + sin(angle), 0, 1 }
				};

				if (!CompareMatrix(T1, in1, out1, sizeof(in1) / sizeof(float[4])))
				{
					Assert::Fail(L"Error InitDenavitHartenberg1Rot#2\n");
				}
			}

			//////////////////////////////////////////

			{
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg2Trans(float(M_PI_4));
				CMatrix4x4<float> T2;
				T2.InitDenavitHartenberg2Trans(-float(M_PI_4));

				CMatrix4x4<float> T3 = T1 * T2;
				CMatrix4x4<float> T4;
				T4.InitDenavitHartenbergNOP();


				if (!T3.IsEqual(T4, 0.0000001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg2Trans\n");
				}
			}

			//////////////////////////////////////////
			{
				const float       d = float(M_PI / 5);
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg2Trans(d);

				float in1[][4] = {
					{ 1, 0, 0, 1 },
					{ 0, 1, 0, 1 },
					{ 0, 0, 1, 1 },
					{ 1, 1, 0, 1 }
				};
				float out1[][4] = {
					{ 1, 0, d, 1 },
					{ 0, 1, d, 1 },
					{ 0, 0, 1 + d, 1 },
					{ 1, 1, 0 + d, 1 }
				};

				if (!CompareMatrix(T1, in1, out1, sizeof(in1) / sizeof(float[4])))
				{
					Assert::Fail(L"Error InitDenavitHartenberg2Trans#2\n");
				}
			}

			//////////////////////////////////////////

			{
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg3Trans(float(M_PI_4));
				CMatrix4x4<float> T2;
				T2.InitDenavitHartenberg3Trans(-float(M_PI_4));

				CMatrix4x4<float> T3 = T1 * T2;
				CMatrix4x4<float> T4;
				T4.InitDenavitHartenbergNOP();


				if (!T3.IsEqual(T4, 0.0000001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg3Trans\n");
				}
			}

			//////////////////////////////////////////
			{
				const float       a = float(M_PI / 5);
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg3Trans(a);

				float in1[][4] = {
					{ 1, 0, 0, 1 },
					{ 0, 1, 0, 1 },
					{ 0, 0, 1, 1 },
					{ 1, 1, 0, 1 }
				};
				float out1[][4] = {
					{ 1 + a, 0, 0, 1 },
					{ 0 + a, 1, 0, 1 },
					{ 0 + a, 0, 1, 1 },
					{ 1 + a, 1, 0, 1 }
				};

				if (!CompareMatrix(T1, in1, out1, sizeof(in1) / sizeof(float[4])))
				{
					Assert::Fail(L"Error InitDenavitHartenberg3Trans#2\n");
				}
			}

			//////////////////////////////////////////

			{
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg4Rot(float(M_PI_4));
				CMatrix4x4<float> T2;
				T2.InitDenavitHartenberg4Rot(-float(M_PI_4));

				CMatrix4x4<float> T3 = T1 * T2;
				CMatrix4x4<float> T4;
				T4.InitDenavitHartenbergNOP();


				if (!T3.IsEqual(T4, 0.0000001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg4Rot\n");
				}
			}

			//////////////////////////////////////////
			{
				const float       angle = float(M_PI / 5);
				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg4Rot(angle);

				float in1[][4] = {
					{ 1, 0, 0, 1 },
					{ 0, 1, 0, 1 },
					{ 0, 0, 1, 1 },
					{ 1, 1, 1, 1 }
				};
				float out1[][4] = {
					{ 1, 0, 0, 1 },
					{ 0, cos(angle), sin(angle), 1 },
					{ 0, -sin(angle), cos(angle), 1 },
					{ 1, cos(angle) - sin(angle), cos(angle) + sin(angle), 1 }
				};

				if (!CompareMatrix(T1, in1, out1, sizeof(in1) / sizeof(float[4])))
				{
					Assert::Fail(L"Error InitDenavitHartenberg4Rot#2\n");
				}
			}

			//////////////////////////////////////////

			{
				float alpha = float(M_PI / 5);
				float theta = float(M_PI / 6);
				float a     = 1.123456f;
				float d     = 4.321f;

				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg(a, alpha, theta, d);
				CMatrix4x4<float> T2;
				T2.InitDenavitHartenbergInverse(a, alpha, theta, d);

				CMatrix4x4<float> T3 = T1 * T2;
				CMatrix4x4<float> T4;
				T4.InitDenavitHartenbergNOP();


				if (!T3.IsEqual(T4, 0.00001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg\n");
				}
			}

			//////////////////////////////////////////

			{
				float alpha = float(M_PI / 5);
				float theta = float(M_PI / 6);
				float a     = 1.123456f;
				float d     = 4.321f;

				CMatrix4x4<float> T10;
				T10.InitDenavitHartenberg(a, alpha, d, theta);

				CMatrix4x4<float> T1;
				T1.InitDenavitHartenberg1Rot(theta);
				CMatrix4x4<float> T2;
				T2.InitDenavitHartenberg2Trans(d);
				CMatrix4x4<float> T3;
				T3.InitDenavitHartenberg3Trans(a);
				CMatrix4x4<float> T4;
				T4.InitDenavitHartenberg4Rot(alpha);

				CMatrix4x4<float> T5 = T1 * T2 * T3 * T4;

				if (!T5.IsEqual(T10, 0.00001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg sequence\n");
				}
			}

			//////////////////////////////////////////

			{
				float alpha = float(M_PI_2);
				float v[4]  = { 1, 1, 1, 1 };


				CMatrix4x4<float> T10;
				T10.InitDenavitHartenberg(0, alpha, 0, 0);
				CMatrix4x4<float> T4;
				T4.InitDenavitHartenberg4Rot(alpha);

				if (!T4.IsEqual(T10, 0.00001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg alpha\n");
				}

				T10.Mul(v, out);

				if (!CMatrix4x4<float>::IsEqual(out[0], 1, 0.001f) ||
					!CMatrix4x4<float>::IsEqual(out[1], -1, 0.001f) ||
					!CMatrix4x4<float>::IsEqual(out[2], 1, 0.001f) ||
					!CMatrix4x4<float>::IsEqual(out[3], 1, 0.001f))
				{
					Assert::Fail(L"Error InitDenavitHartenberg rotate a\n");
				}
			}
		}
	};
}
