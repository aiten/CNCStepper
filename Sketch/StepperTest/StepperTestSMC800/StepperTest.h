////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////

#pragma once

class CStepperTest
{
public:

  struct SMove
  {
    sdist_t dist;
    steprate_t rate;
  };

private:

  struct SMove* _moves;
  steprate_t _maxspeed;
  udist_t _maxdist;

  steprate_t _movespeed;
  udist_t _movedist;

  sdist_t _count[NUM_AXIS];

public:

  CStepperTest(struct SMove* moves, steprate_t maxspeed, udist_t maxdist)
  {
    _moves = moves;
    _maxspeed = maxspeed;
    _maxdist = maxdist;
    
    _movespeed = 0;
    _movedist = 0;
    
    for(unsigned char i=0;i<NUM_AXIS;i++) _count[i] = 0;

    for(unsigned char i=0;_moves[i].rate;i++)
    {
      _movedist += _moves[i].dist;
      if (_moves[i].dist && _movespeed < _moves[i].rate)   _movespeed = _moves[i].rate;
    }
  }

  void TestAxis(axis_t axis)
  {
    for (register unsigned char i=0;_moves[i].rate != 0; i++)
    {
      if (_moves[i].dist)
      {
        sdist_t    dist = RoundMulDivI32(_moves[i].dist,_maxdist,_movedist);
        steprate_t rate = RoundMulDivU32(_moves[i].rate,_maxspeed,_movespeed);
        CStepper::GetInstance()->MoveRel(axis, dist, rate); _count[axis] -= dist;
      }
      else
      {
        CStepper::GetInstance()->Wait((unsigned int) _moves[i].rate);
      }
    }
  }

  void Home(steprate_t speed=0)
  {
    CStepper::GetInstance()->MoveRel(_count,speed ? speed : _maxspeed);

    for(unsigned char i=0;i<NUM_AXIS;i++) _count[i] = 0;
  }
};
