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

#ifndef RAMPS14_NUM_AXIS
#define RAMPS14_NUM_AXIS 5
#endif

////////////////////////////////////////////////////////

#undef  RAMPS14_USE_A4998
#define RAMPS14_USE_DRV8825
#undef  RAMPS14_USE_TMC220X

////////////////////////////////////////////////////////

#define RAMPS14_PIN_STEP_OFF 0
#define RAMPS14_PIN_STEP_ON 1

#define RAMPS14_PIN_DIR_OFF 0
#define RAMPS14_PIN_DIR_ON 1

// Enable: LOW Active
#define RAMPS14_PIN_ENABLE_OFF 1
#define RAMPS14_PIN_ENABLE_ON 0

////////////////////////////////////////////////////////

// only available on Arduino Mega / due

#define RAMPS14_REF_ON	0
#define RAMPS14_REF_OFF	1

// 54 => AD0
#define RAMPS14_X_STEP_PIN			54
#define RAMPS14_X_DIR_PIN			55
#define RAMPS14_X_ENABLE_PIN		38
#define RAMPS14_X_MIN_PIN			3
#define RAMPS14_X_MAX_PIN			2

#define RAMPS14_Y_STEP_PIN			60
#define RAMPS14_Y_DIR_PIN			61
#define RAMPS14_Y_ENABLE_PIN		56
#define RAMPS14_Y_MIN_PIN			14
#define RAMPS14_Y_MAX_PIN			15

#define RAMPS14_Z_STEP_PIN			46
#define RAMPS14_Z_DIR_PIN			48
#define RAMPS14_Z_ENABLE_PIN		62
#define RAMPS14_Z_MIN_PIN			18
#define RAMPS14_Z_MAX_PIN			19

#define RAMPS14_Z2_STEP_PIN			36
#define RAMPS14_Z2_DIR_PIN			34
#define RAMPS14_Z2_ENABLE_PIN		30

#if RAMPS14_NUM_AXIS > 3

#define RAMPS14_E0_STEP_PIN			26
#define RAMPS14_E0_DIR_PIN			28
#define RAMPS14_E0_ENABLE_PIN		24

#endif

#if RAMPS14_NUM_AXIS > 4

#define RAMPS14_E1_STEP_PIN			36
#define RAMPS14_E1_DIR_PIN			34
#define RAMPS14_E1_ENABLE_PIN		30

#endif

#define RAMPS14_SDPOWER				(-1)
#define RAMPS14_LED_PIN				13

#if defined(__SAM3X8E__)
#define RAMPS14_SDSS_PIN	 		52
#else
#define RAMPS14_SDSS_PIN			53
#endif

#define RAMPS14_PS_ON_PIN			12

#define RAMPS14_FET1D8_PIN			8		// D8 FET1
#define RAMPS14_FET2D9_PIN			9		// D9 FET2
#define RAMPS14_FET3D10_PIN			10		// D10 FET3

#define RAMPS14_KILL_PIN			41		// D41

#define RAMPS14_TEMP_0_PIN			67		// AD13
#define RAMPS14_TEMP_1_PIN			68		// AD14
#define RAMPS14_TEMP_2_PIN			69		// AD15

#define RAMPS14_SERVO0_PIN			11		// D11
#define RAMPS14_SERVO1_PIN			6		// D6
#define RAMPS14_SERVO2_PIN			5		// D5
#define RAMPS14_SERVO3_PIN			4		// D4

// these pins are defined in the SD library if building with SD support  
#define RAMPS14_MAX_SCK_PIN         52		// D52
#define RAMPS14_MAX_MISO_PIN        50		// D50
#define RAMPS14_MAX_MOSI_PIN        51		// D51
#define RAMPS14_MAX6675_SS			53		// D53

#define RAMPS14_AUX2_1				(-1)	// VLogic
#define RAMPS14_AUX2_2				(-1)	// GND
#define RAMPS14_AUX2_3				59		// AD5
#define RAMPS14_AUX2_4				63		// AD9
#define RAMPS14_AUX2_5				64		// A10
#define RAMPS14_AUX2_6				40		// D40
#define RAMPS14_AUX2_7				44		// D44
#define RAMPS14_AUX2_8				42		// D42
#define RAMPS14_AUX2_9				66		// A12
#define RAMPS14_AUX2_10				65		// A11

#define RAMPS14_AUX3_1				(-1)	// 5V
#define RAMPS14_AUX3_2				49		// D49
#define RAMPS14_AUX3_3				50		// MISO - D50
#define RAMPS14_AUX3_4				51		// MOSI - D51
#define RAMPS14_AUX3_5				52		// SCK  - D52
#define RAMPS14_AUX3_6				53		// SPI_CS1 - D53
#define RAMPS14_AUX3_7				(-1)	// GND
#define RAMPS14_AUX3_8				(-1)	// NC

#define RAMPS14_AUX4_1				(-1)	// VLOGIC
#define RAMPS14_AUX4_2				(-1)	// GND
#define RAMPS14_AUX4_3				32		// D32
#define RAMPS14_AUX4_4				47		// D47
#define RAMPS14_AUX4_5				45		// D45
#define RAMPS14_AUX4_6				43		// D43
#define RAMPS14_AUX4_7				41		// D41
#define RAMPS14_AUX4_8				39		// D39
#define RAMPS14_AUX4_9				37		// D37
#define RAMPS14_AUX4_10				35		// D35
#define RAMPS14_AUX4_11				33		// D33
#define RAMPS14_AUX4_12				31		// D31
#define RAMPS14_AUX4_13				29		// D29
#define RAMPS14_AUX4_14				27		// D27
#define RAMPS14_AUX4_15				25		// D25
#define RAMPS14_AUX4_16				23		// D23
#define RAMPS14_AUX4_17				17		// UART2_RX  Serial2 on pins 17 (RX) and 16 (TX)
#define RAMPS14_AUX4_18				16		// UART2_TX
// 3-8: share E1&E2

////////////////////////////////////////////////////////
// LCD

#define RAMPS14_LCD_ROTARY_ENC		RAMPS14_AUX4_10		// Dreh Encoder auf Ramps 1.4 - Press button
#define RAMPS14_LCD_ROTARY_EN1      RAMPS14_AUX4_12		// Dreh Encoder auf Ramps 1.4
#define RAMPS14_LCD_ROTARY_EN2      RAMPS14_AUX4_11		// Dreh Encoder auf Ramps 1.4

#define RAMPS14_LCD_ROTARY_ENC_ON	LOW		// Pressed
#define RAMPS14_LCD_ROTARY_ENC_OFF	HIGH

#define RAMPS14_LCD_BEEPER          RAMPS14_AUX4_9		// Summer auf Ramps 1.4
#define RAMPS14_LCD_KILL_PIN        RAMPS14_AUX4_7		// Stoptaste auf Ramps 1.4

#define RAMPS14_LCD_KILL_PIN_ON  LOW	// Pressed
#define RAMPS14_LCD_KILL_PIN_OFF HIGH

#define RAMPS14_LCD_PINS_RS			RAMPS14_AUX4_18
#define RAMPS14_LCD_PINS_ENABLE		RAMPS14_AUX4_17
#define RAMPS14_LCD_PINS_D4			RAMPS14_AUX4_16
#define RAMPS14_LCD_PINS_D5			RAMPS14_AUX4_15
#define RAMPS14_LCD_PINS_D6			RAMPS14_AUX4_14
#define RAMPS14_LCD_PINS_D7			RAMPS14_AUX4_13

#define RAMPS14_ST7920_CLK_PIN		RAMPS14_LCD_PINS_D4
#define RAMPS14_ST7920_DAT_PIN		RAMPS14_LCD_PINS_ENABLE
#define RAMPS14_ST7920_CS_PIN		RAMPS14_LCD_PINS_RS
