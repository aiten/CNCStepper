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

////////////////////////////////////////////////////////

#ifdef _NO_LONG_MESSAGE

#define StepperMessage(a,b)	F(a)
#define StepperMessageOr(a,b)	F(a)
#define StepperMessageOne(a)	F(a)

#define MESSAGE(a)

#else

//#define StepperMessage(a,b) b
#define StepperMessage(a,b) F(a ":" b)
#define StepperMessageOr(a,b)	F(b)
#define StepperMessageOne(a)	F(a)

#define MESSAGE(a) a

#endif

#define MESSAGE_CTRLX								StepperMessageOne("Grbl 0.9j ['$' for help]")
#define MESSAGE_OK_INITIALIZED						MESSAGE_OK
#define MESSAGE_OK_RESURRECT						MESSAGE_OK
#define MESSAGE_OK_EMPTYLINE						MESSAGE_OK
#define MESSAGE_OK									StepperMessageOne("ok")
#define MESSAGE_ERROR								StepperMessageOne("error:")
#define MESSAGE_INFO								StepperMessageOne("info:")
#define MESSAGE_WARNING								StepperMessageOne("warning:")

#define MESSAGE_UNKNOWNERROR						StepperMessage("?","Unknown error")

#define MESSAGE_STEPPER_EmptyMoveSkipped			StepperMessage("1","EmptyMove skipped")
#define MESSAGE_STEPPER_Backlash					StepperMessage("2","Backlash")
#define MESSAGE_STEPPER_IsAnyReference				StepperMessage("3","IsAnyReference")
#define MESSAGE_STEPPER_RangeLimit					StepperMessage("1","Range limit")
#define MESSAGE_STEPPER_IsReferenceIsOn				StepperMessage("2","IsReference is on")
#define MESSAGE_STEPPER_MoveReferenceFailed			StepperMessage("4","MoveReference failed")

#define MESSAGE_STEPPER_MoveAwayFromReference		StepperMessage("5","Move away from reference")
