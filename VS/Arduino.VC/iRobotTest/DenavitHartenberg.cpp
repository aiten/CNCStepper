#include "stdafx.h"
#include <math.h>

#include "..\MsvcStepper\MsvcStepper.h"
#include "TestTools.h"

#include "..\..\..\Sketch\libraries\CNCLib\src\Matrix4x4.h"
#include "DenavitHartenberg.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////

CDenavitHartenberg::CDenavitHartenberg() {}

//////////////////////////////////////////////////////////////////////////////////////////////////////

CDenavitHartenberg::~CDenavitHartenberg() {}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void CDenavitHartenberg::InitMatrix(CMatrix4x4<float>& m, float in[NUM_AXIS])
{
	CMatrix4x4<float> A;

	float servo1 = in[0];
	float servo2 = in[1];
	float servo3 = in[2];

	float pos1 = servo1;
	float pos2 = float((M_PI) + servo2);
	float pos3 = servo3;

	float pos4 = float(M_PI + (M_PI - pos1 - pos2));

	float l0 = 105;
	float l1 = 140;
	float l2 = 152;
	float l3 = 30;

	m.InitDenavitHartenberg(0, float(M_PI_2), l0, pos3);
	//TestConvert(AX, v);
	m *= A.InitDenavitHartenberg1Rot3Trans(l1, pos1);
	//TestConvert(AX, v);
	m *= A.InitDenavitHartenberg1Rot3Trans(l2, pos2);
	//TestConvert(AX, v);
	m *= A.InitDenavitHartenberg1Rot3Trans(l3, pos4);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void CDenavitHartenberg::ToPosition(float in[NUM_AXIS], float out[3])
{
	float             v[4] = { 0, 0, 0, 1 };
	CMatrix4x4<float> A;

	InitMatrix(A, in);

	TestConvert(A, v);

	float d[4];

	A.Mul(v, d);

	memcpy(out, d, sizeof(float) * 3);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void CDenavitHartenberg::FromPosition(float posxyz[3], float angles[NUM_AXIS], float epsilon)
{
	float angle = float(M_PI + 0.1);

	SSearchDef search[] =
	{
		{ 0, angle, angle / 10 },
		{ 0, angle, angle / 10 },
		{ -angle, angle, angle / 10 }
	};

	for (uint8_t j = 0; j < 3; j++)
		angles[j]  = (search[j].max - search[j].min) / 2 + search[j].min;

	if (true)
	{
		unsigned int count;
		uint8_t      i    = 0;
		float        diff = FLT_MAX;

		for (count = 0; count < 10000; count++)
		{
			diff = SearchStep(posxyz, angles, i, diff, search[i]);

			if (diff < epsilon)
				break;
#define TEST_U
#ifdef TEST
			printf("%i:\t%08f:\t", count,diff);
			for (uint8_t j = 0; j < NUM_AXIS; j++)
				printf("%08f%s", angles[j], j < NUM_AXIS-1 ? ":" : "\t");
			for (uint8_t j = 0; j < NUM_AXIS; j++)
				printf("%08f%s", search[j].dist, j < NUM_AXIS - 1 ? ":" : "\n");
#endif
			i++;
			if (i >= NUM_AXIS)
				i = 0;
		}

		//		for (i = 0; i < NUM_AXIS; i++)
		//			printf("dist:%f ", search[i].dist);
		//		printf("max:%i\n", count);
	}
	else if (true)
	{
		float             v[4] = { 0, 0, 0, 1 };
		CMatrix4x4<float> A;

		//InitMatrix(A, in);
	}
	else if (false)
	{
		uint8_t i;
		for (i = 0; i < 200; i++)
		{
			for (uint8_t j = 0; j < 3; j++)
			{
				if (SearchMinOld(posxyz, angles, j, search[j], epsilon) < epsilon)
				{
					printf("max:%i\n", static_cast<unsigned int>(i));
					return;
				}
			}
		}
		printf("max:%i\n", static_cast<unsigned int>(i));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////

float CDenavitHartenberg::SearchStep(float pos[3], float inout[NUM_AXIS], uint8_t idx, float diff, SSearchDef& def)
{
	float oldDiff = diff;

	float oldPos = inout[idx];
	float newPos = oldPos + def.dist;

	if (newPos > def.max)
	{
		newPos = def.max;
	}
	else if (newPos < def.min)
	{
		newPos = def.min;
	}
	else if (oldPos == newPos)
	{
		return oldDiff;
	}

	inout[idx] = newPos;

	diff = CalcDist(pos, inout);

	if (diff > oldDiff)
	{
		inout[idx] = oldPos;
		if (fabs(def.dist) < 0.01)
			def.dist = def.dist / -1.01f;
		else
			def.dist = def.dist / -2;
		return oldDiff;
	}
	else if (diff == oldDiff)
	{
		if (oldPos == newPos)
			def.dist = def.dist / -2.0f;
		return diff;
	}
	else if (newPos >= def.max)
	{
		def.dist = def.dist / -2.0f;
		return diff;
	}
	else if (newPos <= def.min)
	{
		def.dist = def.dist / -2.0f;
		return diff;
	}

	return diff;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


float CDenavitHartenberg::SearchMinOld(float pos[3], float inout[NUM_AXIS], uint8_t idx, SSearchDef& def, float epsilon)
{
	float inPos = inout[idx];
	float dist   = def.dist;

	float    olDiff = CalcDist(pos, inout);
	float    diff   = olDiff;
	uint16_t count  = 0;

	while (true)
	{
		float oldPos = inout[idx];
		float newPos = oldPos + dist;

		if (oldPos == newPos || count > 25)		// dist < FLT_EPSILON
			break;

		if (newPos > def.max)
		{
			newPos = def.max;
		}
		else if (newPos < def.min)
		{
			newPos = def.min;
		}

		inout[idx] = newPos;

		diff = CalcDist(pos, inout);

		if (diff < epsilon)
			break;

		if (diff > olDiff)
		{
			inout[idx] = oldPos;
			dist       = -dist / 2;
			count++;
		}
		else if (newPos == def.max)
		{
			dist = -dist / 2;
			count++;
		}
		else if (newPos == def.min)
		{
			dist = -dist / 2;
			count++;
		}
		olDiff = diff;
	}

	def.changeToPrev = inPos - inout[idx];
	return diff;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

float CDenavitHartenberg::CalcDist(float pos[3], float in[NUM_AXIS])
{
	float             v[4] = { 0, 0, 0, 1 };
	CMatrix4x4<float> A;

	InitMatrix(A, in);
	TestConvert(A, v, true);

	float        dist = 0;
	for (uint8_t i    = 0; i < 3; i++)
		dist += (pos[i] - v[i]) * (pos[i] - v[i]);

	return dist;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

#define FORMAT_MM "%.0f:%.0f:%.0f"
#define FORMAT_GRAD "%.0f:%.0f:%.0f"

void CDenavitHartenberg::TestConvert(CMatrix4x4<float>& m, float inout[4], bool out)
{
	float d[4];

	m.Mul(inout, d);

	if (out)
		memcpy(inout, d, sizeof(d));

	//printf(FORMAT_MM"\n", d[0], d[1], d[2]);
}

/*

n = no of equations
a[n]n] = coefficient matrix
b[n] = right hand side vector

x[n] - solution vector


*/

bool CDenavitHartenberg::Jacobi(double a[][MAXSIZE], double b[], int n, int maxiter, double tol, double x[])
{
	int    numiter     = 0;
	bool   tolexceeded = true;
	int    i, j;
	double xold[MAXSIZE];
	double sum;

	for (i   = 0; i < n; i++)
		x[i] = b[i] / a[i][i];

	while (tolexceeded && numiter < maxiter)
	{
		for (i      = 0; i < n; ++i)
			xold[i] = x[i];

		for (i = 0; i < n; ++i)
		{
			sum    = b[i];
			for (j = 0; j < n; ++j)
				if (i != j)
					sum -= a[i][j] * xold[j];

			x[i] = sum / a[i][i];
		}
		tolexceeded = false;
		for (i      = 0; i < n; i++)
			if (fabs(x[i] - xold[i]) > fabs(xold[i] * tol))
				tolexceeded = true;
		++numiter;
	}
	return tolexceeded;
}
