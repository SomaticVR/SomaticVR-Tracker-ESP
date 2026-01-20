/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2021 Eiren Rain

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/
// ================================================
// See docs for configuration options and examples:
// https://docs.slimevr.dev/firmware/configuring-project.html#2-configuring-definesh
// ================================================

// Set parameters of IMU and board used
#define IMU IMU_AUTO
#define SECOND_IMU IMU_AUTO
#define BOARD BOARD_SOMATIC_EROS
#define IMU_ROTATION DEG_180
#define SECOND_IMU_ROTATION DEG_270

#define PRIMARY_IMU_OPTIONAL false
#define SECONDARY_IMU_OPTIONAL true

#if BOARD != BOARD_GLOVE_IMU_SLIMEVR_DEV
#define MAX_SENSORS_COUNT 1
#define TRACKER_TYPE TrackerType::TRACKER_TYPE_SVR_ROTATION

#ifndef SENSOR_DESC_LIST
#define SENSOR_DESC_LIST                       \
	SENSOR_DESC_ENTRY(                         \
		IMU,                                   \
		PRIMARY_IMU_ADDRESS_ONE,               \
		IMU_ROTATION,                          \
		DIRECT_WIRE(PIN_IMU_SCL, PIN_IMU_SDA), \
		PRIMARY_IMU_OPTIONAL,                  \
		DIRECT_PIN(PIN_IMU_INT),               \
		0                                      \
	)                                          \
	SENSOR_DESC_ENTRY(                         \
		SECOND_IMU,                            \
		SECONDARY_IMU_ADDRESS_TWO,             \
		SECOND_IMU_ROTATION,                   \
		DIRECT_WIRE(PIN_IMU_SCL, PIN_IMU_SDA), \
		SECONDARY_IMU_OPTIONAL,                \
		DIRECT_PIN(PIN_IMU_INT_2),             \
		0                                      \
	)
#endif
#endif

// Battery monitoring options (comment to disable):
//   BAT_EXTERNAL for ADC pin,
//   BAT_INTERNAL for internal - can detect only low battery,
//   BAT_MCP3021 for external ADC connected over I2C
#define BATTERY_MONITOR BAT_EXTERNAL

// Board-specific configurations
#define PIN_IMU_SDA 8
#define PIN_IMU_SCL 7
#define PIN_IMU_INT 5
#define PIN_IMU_INT_2 255
#define PIN_IMU_ENABLE 1
#define PIN_CHRG_DONE 1
#define PIN_BATTERY_LEVEL 0
#define LED_PIN 9
#define LED_INVERTED true
#define LEDC_FREQ_LED 20000
#define LEDC_BITS_LED  10  
#define PIN_ENABLE_LATCH 2
#define PIN_BUTTON_INPUT 3
#define PIN_TACT_MOTOR 6
#define LEDC_FREQ_TACT_MOTOR 20000
#define LEDC_BITS_TACT_MOTOR  8
#define PIN_USB_PD_INT 4
#define PIN_CHARGER_INT 10
#define PIN_BAT_STAT_CHRG 4
#define PIN_BAT_STAT_CHRG_DONE 10
#define TACT_MOTOR_MAX_V  3.0f
#ifndef BATTERY_SHIELD_RESISTANCE
#define BATTERY_SHIELD_RESISTANCE 0
#endif
#ifndef BATTERY_SHIELD_R1 
#define BATTERY_SHIELD_R1 10
#endif
#ifndef BATTERY_SHIELD_R2
#define BATTERY_SHIELD_R2 45.3
#endif
