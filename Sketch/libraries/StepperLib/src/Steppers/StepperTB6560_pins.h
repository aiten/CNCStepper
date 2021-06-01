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

#define TB6560_PIN_DIR_OFF 0
#define TB6560_PIN_DIR_ON 1

#define TB6560_PIN_STEP_OFF 0
#define TB6560_PIN_STEP_ON 1

// Enable: HIGH Active?
#define TB6560_PIN_ENABLE_OFF 1
#define TB6560_PIN_ENABLE_ON 0

////////////////////////////////////////////////////////

#define TB6560_REF_ON	0
#define TB6560_REF_OFF	1

#define TB6560_X_STEP_PIN		2
#define TB6560_X_DIR_PIN		3
#define TB6560_X_ENABLE_PIN		4
#define TB6560_X_MIN_PIN		
#define TB6560_X_MAX_PIN		

#define TB6560_Y_STEP_PIN		5
#define TB6560_Y_DIR_PIN		6
#define TB6560_Y_ENABLE_PIN		7
#define TB6560_Y_MIN_PIN		
#define TB6560_Y_MAX_PIN		

#define TB6560_Z_STEP_PIN		8
#define TB6560_Z_DIR_PIN		9
#define TB6560_Z_ENABLE_PIN		10
#define TB6560_Z_MIN_PIN		
#define TB6560_Z_MAX_PIN		
