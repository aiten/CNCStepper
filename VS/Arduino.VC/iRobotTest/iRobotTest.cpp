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
#include <math.h>

#include "..\MsvcStepper\MsvcStepper.h"
#include "TestTools.h"

#include "..\..\..\Sketch\libraries\CNCLib\src\Matrix4x4.h"
#include "DenavitHartenberg.h"

////////////////////////////////////////////////////////////////////////////////////////

int _tmain(int /* argc */, _TCHAR* /* argv */[])
{
	//////////////////////////////////////////

	{
		/*
		float alpha = M_PI / 5;
		float theta = M_PI / 6;
		float a = 1.123456;
		float d = 4.321;
		CMatrix4x4<float> T1; T1.InitDenavitHartenberg1Rot(theta);
		CMatrix4x4<float> T2; T2.InitDenavitHartenberg3Trans(a);

		CMatrix4x4<float> T3 = T1*T2;

		CMatrix4x4<float> T4; T4.InitDenavitHartenberg1Rot3Trans(theta, a);

		if (!T4.IsEqual(T3, 0.00001))
		{
			printf("Error InitDenavitHartenberg\n");
		}
		*/
	}

	//////////////////////////////////////////
	{
		CDenavitHartenberg dh;

		struct STest
		{
			float posxyz[3];
			float angles[3];
			float allowDiff;
		};
		float allowDiff = float(0.0001);

		STest values[] = {

			{ { 1.000000, 200.000000, 105.000000 }, { 1.008016f, 1.240501f, 1.565796f }, allowDiff },
			{ { 0.000000, 200.000000, 105.000000 }, { 1.008028f, 1.240480f, 1.570796f }, allowDiff },
			{ { -1.000000, 200.000000, 105.000000 }, { 1.008016f, 1.240501f, 1.575796f }, allowDiff },
			{ { 1.000000, -200.000000, 105.000000 }, { 1.008016f, 1.240501f, -1.565796f }, allowDiff },
			{ { 0.000000, -200.000000, 105.000000 }, { 1.008028f, 1.240480f, -1.570796f }, allowDiff },
			{ { -1.000000, -200.000000, 105.000000 }, { 1.008016f, 1.240501f, -1.575796f }, allowDiff },
			{ { 322.000000, 0.000000, 105.000000 }, { 0.000000f, 3.1415926f, 0.000000 }, allowDiff },
			{ { 182.000000, 0.000000, 245.000000 }, { 1.5707963f, 1.5707963f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 105.000000 }, { 1.008028f, 1.240480f, 0.000000 }, allowDiff },
			{ { 200.000000, 100.000000, 105.000000 }, { 0.893337f, 1.447827f, 0.463648f }, allowDiff },
			{ { 100.000000, 100.000000, 105.000000 }, { 1.281145f, 0.778904f, 0.785398f }, allowDiff },
			{ { 50.000000, 0.000000, 105.000000 }, { 2.158301f, 0.109737f, 0.000000 }, allowDiff },
			{ { 60.000000, 0.000000, 105.000000 }, { 1.885989f, 0.188764f, 0.000000 }, allowDiff },
			{ { 70.000000, 0.000000, 105.000000 }, { 1.741626f, 0.262326f, 0.000000 }, allowDiff },
			{ { 80.000000, 0.000000, 105.000000 }, { 1.642572f, 0.334292f, 0.000000 }, allowDiff },
			{ { 90.000000, 0.000000, 105.000000 }, { 1.565082f, 0.405774f, 0.000000 }, allowDiff },
			{ { 100.000000, 0.000000, 105.000000 }, { 1.499511f, 0.477271f, 0.000000 }, allowDiff },
			{ { 110.000000, 0.000000, 105.000000 }, { 1.441148f, 0.549075f, 0.000000 }, allowDiff },
			{ { 120.000000, 0.000000, 105.000000 }, { 1.387389f, 0.621401f, 0.000000 }, allowDiff },
			{ { 130.000000, 0.000000, 105.000000 }, { 1.336663f, 0.694426f, 0.000000 }, allowDiff },
			{ { 140.000000, 0.000000, 105.000000 }, { 1.287949f, 0.768320f, 0.000000 }, allowDiff },
			{ { 150.000000, 0.000000, 105.000000 }, { 1.240540f, 0.843252f, 0.000000 }, allowDiff },
			{ { 160.000000, 0.000000, 105.000000 }, { 1.193915f, 0.919401f, 0.000000 }, allowDiff },
			{ { 170.000000, 0.000000, 105.000000 }, { 1.147671f, 0.996961f, 0.000000 }, allowDiff },
			{ { 180.000000, 0.000000, 105.000000 }, { 1.101470f, 1.076153f, 0.000000 }, allowDiff },
			{ { 190.000000, 0.000000, 105.000000 }, { 1.055015f, 1.157228f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 105.000000 }, { 1.008028f, 1.240480f, 0.000000 }, allowDiff },
			{ { 210.000000, 0.000000, 105.000000 }, { 0.960228f, 1.326261f, 0.000000 }, allowDiff },
			{ { 220.000000, 0.000000, 105.000000 }, { 0.911315f, 1.414998f, 0.000000 }, allowDiff },
			{ { 230.000000, 0.000000, 105.000000 }, { 0.860954f, 1.507220f, 0.000000 }, allowDiff },
			{ { 240.000000, 0.000000, 105.000000 }, { 0.808743f, 1.603603f, 0.000000 }, allowDiff },
			{ { 250.000000, 0.000000, 105.000000 }, { 0.754183f, 1.705034f, 0.000000 }, allowDiff },
			{ { 260.000000, 0.000000, 105.000000 }, { 0.696615f, 1.812717f, 0.000000 }, allowDiff },
			{ { 270.000000, 0.000000, 105.000000 }, { 0.635121f, 1.928367f, 0.000000 }, allowDiff },
			{ { 280.000000, 0.000000, 105.000000 }, { 0.568332f, 2.054579f, 0.000000 }, allowDiff },
			{ { 290.000000, 0.000000, 105.000000 }, { 0.494008f, 2.195630f, 0.000000 }, allowDiff },
			{ { 300.000000, 0.000000, 105.000000 }, { 0.407949f, 2.359574f, 0.000000 }, allowDiff },
			{ { 310.000000, 0.000000, 105.000000 }, { 0.300103f, 2.565728f, 0.000000 }, allowDiff },
			{ { 320.000000, 0.000000, 105.000000 }, { 0.122047f, 2.907177f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 50.000000 }, { 0.653714f, 1.314741f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 150.000000 }, { 1.238929f, 1.290373f, 0.000000 }, allowDiff },
			{ { 200.000000, 50.000000, 150.000000 }, { 1.201563f, 1.342109f, 0.244979f }, allowDiff },
			{ { -1234.0f } }
		};

		for (STest* v = values; v->posxyz[0] != -1234; v++)
		{
			float out[3];
			dh.ToPosition(v->angles, out);

			bool print = false;

			for (uint8_t n = 0; n < 3; n++)
			{
				if (!CMatrix4x4<float>::IsEqual(out[n], v->posxyz[n], v->allowDiff))
				{
					if (!print)
						printf("Error InitDenavitHartenberg #20: %.2f:%.2f:%.2f ", v->posxyz[0], v->posxyz[1], v->posxyz[2]);
					printf("%f=>%f(%f) ", out[n], v->angles[n], out[n] - v->angles[n]);
					print = true;
				}
			}
			if (print)
				printf("\n");
		}
	}

	//////////////////////////////////////////
	{
		CDenavitHartenberg dh;
		struct STest
		{
			float posxyz[3];
			float angles[3];
			float allowDiff;
		};
		float allowDiff = float(0.001);

		STest values[] = {


			{ { 182.000000, 0.000000, 245.000000 }, { 1.5707963f, 1.5707963f, 0.000000 }, allowDiff },
			{ { 0.000000, -200.000000, 105.000000 }, { 1.008028f, 1.240480f, -1.570796f }, allowDiff },
			//			{ { 320.000000, 0.000000, 105.000000 }, { 0.122047, 2.907177, 0.000000 }, allowDiff },
			{ { 322.000000, 0.000000, 105.000000 }, { 0.000000, 3.1415926f, 0.000000 }, allowDiff },

			{ { 1.000000, 200.000000, 105.000000 }, { 1.008016f, 1.240501f, 1.565796f }, allowDiff },
			{ { 0.000000, 200.000000, 105.000000 }, { 1.008028f, 1.240480f, 1.570796f }, allowDiff },
			{ { -1.000000, 200.000000, 105.000000 }, { 1.008016f, 1.240501f, 1.575796f }, allowDiff },
			{ { 1.000000, -200.000000, 105.000000 }, { 1.008016f, 1.240501f, -1.565796f }, allowDiff },
			{ { 0.000000, -200.000000, 105.000000 }, { 1.008028f, 1.240480f, -1.570796f }, allowDiff },
			{ { -1.000000, -200.000000, 105.000000 }, { 1.008016f, 1.240501f, -1.575796f }, allowDiff },
			{ { 322.000000, 0.000000, 105.000000 }, { 0.000000, 3.1415926f, 0.000000 }, allowDiff },
			{ { 182.000000, 0.000000, 245.000000 }, { 1.5707963f, 1.5707963f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 105.000000 }, { 1.008028f, 1.240480f, 0.000000 }, allowDiff },
			{ { 200.000000, 100.000000, 105.000000 }, { 0.893337f, 1.447827f, 0.463648f }, allowDiff },
			{ { 100.000000, 100.000000, 105.000000 }, { 1.281145f, 0.778904f, 0.785398f }, allowDiff },
			{ { 50.000000, 0.000000, 105.000000 }, { 2.158301f, 0.109737f, 0.000000 }, allowDiff },
			{ { 60.000000, 0.000000, 105.000000 }, { 1.885989f, 0.188764f, 0.000000 }, allowDiff },
			{ { 70.000000, 0.000000, 105.000000 }, { 1.741626f, 0.262326f, 0.000000 }, allowDiff },
			{ { 80.000000, 0.000000, 105.000000 }, { 1.642572f, 0.334292f, 0.000000 }, allowDiff },
			{ { 90.000000, 0.000000, 105.000000 }, { 1.565082f, 0.405774f, 0.000000 }, allowDiff },
			{ { 100.000000, 0.000000, 105.000000 }, { 1.499511f, 0.477271f, 0.000000 }, allowDiff },
			{ { 110.000000, 0.000000, 105.000000 }, { 1.441148f, 0.549075f, 0.000000 }, allowDiff },
			{ { 120.000000, 0.000000, 105.000000 }, { 1.387389f, 0.621401f, 0.000000 }, allowDiff },
			{ { 130.000000, 0.000000, 105.000000 }, { 1.336663f, 0.694426f, 0.000000 }, allowDiff },
			{ { 140.000000, 0.000000, 105.000000 }, { 1.287949f, 0.768320f, 0.000000 }, allowDiff },
			{ { 150.000000, 0.000000, 105.000000 }, { 1.240540f, 0.843252f, 0.000000 }, allowDiff },
			{ { 160.000000, 0.000000, 105.000000 }, { 1.193915f, 0.919401f, 0.000000 }, allowDiff },
			{ { 170.000000, 0.000000, 105.000000 }, { 1.147671f, 0.996961f, 0.000000 }, allowDiff },
			{ { 180.000000, 0.000000, 105.000000 }, { 1.101470f, 1.076153f, 0.000000 }, allowDiff },
			{ { 190.000000, 0.000000, 105.000000 }, { 1.055015f, 1.157228f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 105.000000 }, { 1.008028f, 1.240480f, 0.000000 }, allowDiff },
			{ { 210.000000, 0.000000, 105.000000 }, { 0.960228f, 1.326261f, 0.000000 }, allowDiff },
			{ { 220.000000, 0.000000, 105.000000 }, { 0.911315f, 1.414998f, 0.000000 }, allowDiff },
			{ { 230.000000, 0.000000, 105.000000 }, { 0.860954f, 1.507220f, 0.000000 }, allowDiff },
			{ { 240.000000, 0.000000, 105.000000 }, { 0.808743f, 1.603603f, 0.000000 }, allowDiff },
			{ { 250.000000, 0.000000, 105.000000 }, { 0.754183f, 1.705034f, 0.000000 }, allowDiff },
			{ { 260.000000, 0.000000, 105.000000 }, { 0.696615f, 1.812717f, 0.000000 }, allowDiff },
			{ { 270.000000, 0.000000, 105.000000 }, { 0.635121f, 1.928367f, 0.000000 }, allowDiff },
			{ { 280.000000, 0.000000, 105.000000 }, { 0.568332f, 2.054579f, 0.000000 }, allowDiff },
			{ { 290.000000, 0.000000, 105.000000 }, { 0.494008f, 2.195630f, 0.000000 }, allowDiff },
			{ { 300.000000, 0.000000, 105.000000 }, { 0.407949f, 2.359574f, 0.000000 }, allowDiff },
			{ { 310.000000, 0.000000, 105.000000 }, { 0.300103f, 2.565728f, 0.000000 }, allowDiff },
			{ { 320.000000, 0.000000, 105.000000 }, { 0.122047f, 2.907177f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 50.000000 }, { 0.653714f, 1.314741f, 0.000000 }, allowDiff },
			{ { 200.000000, 0.000000, 150.000000 }, { 1.238929f, 1.290373f, 0.000000 }, allowDiff },
			{ { 200.000000, 50.000000, 150.000000 }, { 1.201563f, 1.342109f, 0.244979f }, allowDiff },
			{ { -1234.0f } }
		};


		for (STest* v = values; v->posxyz[0] != -1234; v++)
		{
			bool print      = false;
			bool printPrint = false;

			float out[3] = { 0, 0, 0 };
			dh.FromPosition(v->posxyz, out, float(0.001));

			for (uint8_t n = 0; n < 3; n++)
			{
				if (!CMatrix4x4<float>::IsEqual(out[n], v->angles[n], v->allowDiff))
				{
					if (!print)
						printf("Error InitDenavitHartenberg #21: %.2f:%.2f:%.2f\t", v->posxyz[0], v->posxyz[1], v->posxyz[2]);

					printf("%f=>%f(%f)\t", out[n], v->angles[n], out[n] - v->angles[n]);
					print      = true;
					printPrint = true;
				}
			}

			if (print)
				printf("\n");

			print = false;

			float posxyz[3];
			dh.ToPosition(out, posxyz);

			for (uint8_t n = 0; n < 3; n++)
			{
				if (!CMatrix4x4<float>::IsEqual(v->posxyz[n], posxyz[n], float(000.1)))
				{
					if (!print)
						printf("Error InitDenavitHartenberg #22: %.2f:%.2f:%.2f\t", v->posxyz[0], v->posxyz[1], v->posxyz[2]);

					printf("%f=>%f(%f)\t", posxyz[n], v->posxyz[n], posxyz[n] - v->posxyz[n]);
					print      = true;
					printPrint = true;
				}
			}

			if (print)
				printf("\n");

			if (printPrint)
				printf("\n");
		}
	}
}
