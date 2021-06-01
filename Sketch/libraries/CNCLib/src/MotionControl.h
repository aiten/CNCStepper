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

// Cartesian coordinate system, same as CMotionControlBase
// CMotionControl supports rotation

////////////////////////////////////////////////////////

#include "MotionControlBase.h"

////////////////////////////////////////////////////////

class CMotionControl : public CMotionControlBase
{
private:

	typedef CMotionControlBase super;

public:

	CMotionControl();

	void     SetRotate(float rad, const mm1000_t vect[NUM_AXISXYZ], const mm1000_t ofs[NUM_AXISXYZ]);
	void     ClearRotate() { _rotateType = NoRotate; }
	bool     IsRotate() const { return _rotateType != NoRotate; }
	mm1000_t GetOffset(axis_t axis) { return _rotateOffset[axis]; }
	mm1000_t GetVector(axis_t axis) { return _vect[axis]; }
	float    GetAngle() const { return _angle; }

	void     SetRotate2D(float          alpha, float beta, float gamma, const mm1000_t ofs[NUM_AXISXYZ]);
	void     SetRotate2D(axis_t         axis, float  rad);
	void     SetOffset2D(const mm1000_t ofs[NUM_AXISXYZ]);
	mm1000_t GetOffset2D(axis_t         axis) { return _rotateOffset2D[axis]; }
	float    GetAngle2D(axis_t          axis) const { return _rotate2D[axis].GetAngle(); }
	bool     IsEnabled2D(axis_t         axis) const { return IsBitSet(_rotateEnabled2D, axis); }
	void     ClearRotate2D() { _rotateEnabled2D = 0; }

	static CMotionControl* GetInstance() { return static_cast<CMotionControl*>(CMotionControlBase::GetInstance()); }

protected:

	virtual void TransformFromMachinePosition(const udist_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS]) override;
	virtual bool TransformPosition(const mm1000_t           src[NUM_AXIS], mm1000_t dest[NUM_AXIS]) override;

private:

	float    _angle;
	mm1000_t _vect[NUM_AXISXYZ];

	struct SRotate3D // Performance Array for matrix
	{
		float _vect[NUM_AXISXYZ][NUM_AXISXYZ];
		void  Set(float rad, const mm1000_t vect[NUM_AXISXYZ]);

		void Rotate(float&         x, float&                     y, float&                  z);
		void Rotate(const mm1000_t src[NUM_AXIS], const mm1000_t ofs[NUM_AXISXYZ], mm1000_t dest[NUM_AXIS]);
	};

	SRotate3D _rotate3D;
	mm1000_t  _rotateOffset[NUM_AXISXYZ];

	enum ERotateType
	{
		NoRotate=0,
		Rotate,
		RotateInvert
	};

	EnumAsByte(ERotateType) _rotateType = NoRotate;

private:

	struct SRotate
	{
		float _sin;
		float _cos;
		float _angle;

		void Set(float rad)
		{
			_angle = rad;
			_sin   = sin(rad);
			_cos   = cos(rad);
		}

		float GetAngle() const { return _angle; }

		void Rotate(float& ax1, float& ax2) const ALWAYSINLINE
		{
			// rotate with positive angle
			float fx = ax1;
			float fy = ax2;

			ax1 = fx * _cos - fy * _sin;
			ax2 = fy * _cos + fx * _sin;
		}

		void RotateInvert(float& ax1, float& ax2) const ALWAYSINLINE
		{
			// rotate with negative angle (e.g. from 30 to -30)
			float fx = ax1;
			float fy = ax2;

			ax1 = fx * _cos + fy * _sin;
			ax2 = fy * _cos - fx * _sin;
		}
	};

	SRotate  _rotate2D[NUM_AXISXYZ];
	mm1000_t _rotateOffset2D[NUM_AXISXYZ];

	axisArray_t _rotateEnabled2D = 0;

#ifdef _MSC_VER

public:

	virtual void UnitTest() override;

	bool Test3D(const mm1000_t src[NUM_AXIS], const mm1000_t ofs[NUM_AXISXYZ], mm1000_t dest[NUM_AXIS], mm1000_t vect[NUM_AXISXYZ], float angle, bool printOK);
	bool Test2D(const mm1000_t src[NUM_AXIS], const mm1000_t ofs[NUM_AXISXYZ], mm1000_t dest[NUM_AXIS], float    angle[NUM_AXISXYZ], bool printOK);

	bool Test(const mm1000_t src[NUM_AXIS], const mm1000_t ofs[NUM_AXISXYZ], mm1000_t dest[NUM_AXIS], bool printOK, std::function<void()> print);

#endif
};

////////////////////////////////////////////////////////
