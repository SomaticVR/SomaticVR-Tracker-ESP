/*
	SlimeVR Code is placed under the MIT license
	Copyright (c) 2021 Eiren Rain & SlimeVR contributors

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
#include "batterymonitor.h"

#include "GlobalVars.h"

void BatteryMonitor::Setup() {
}

void BatteryMonitor::Loop() {
#if BATTERY_MONITOR == BAT_EXTERNAL || BATTERY_MONITOR == BAT_INTERNAL \
	|| BATTERY_MONITOR == BAT_MCP3021 || BATTERY_MONITOR == BAT_INTERNAL_MCP3021
	auto now_ms = millis();
	if (now_ms - last_battery_sample >= batterySampleRate) {
		last_battery_sample = now_ms;
		voltage
			= ((float)analogReadMilliVolts(PIN_BATTERY_LEVEL)) / 1000 * ADCMultiplier;
		if (voltage > 0)  // valid measurement
		{
			// Estimate battery level, 3.2V is 0%, 4.17V is 100% (1.0)
			if (voltage > 3.975f) {
				level = (voltage - 2.920f) * 0.8f;
			} else if (voltage > 3.678f) {
				level = (voltage - 3.300f) * 1.25f;
			} else if (voltage > 3.489f) {
				level = (voltage - 3.400f) * 1.7f;
			} else if (voltage > 3.360f) {
				level = (voltage - 3.300f) * 0.8f;
			} else {
				level = (voltage - 3.200f) * 0.3f;
			}

			level = (level - 0.05f) / 0.95f;  // Cut off the last 5% (3.36V)

			if (level > 1) {
				level = 1;
			} else if (level < 0) {
				level = 0;
			}
			networkConnection.sendBatteryLevel(voltage, level);
#ifdef BATTERY_LOW_POWER_VOLTAGE
			if (voltage < BATTERY_LOW_POWER_VOLTAGE) {
#if defined(BATTERY_LOW_VOLTAGE_DEEP_SLEEP) && BATTERY_LOW_VOLTAGE_DEEP_SLEEP
				ESP.deepSleep(0);
#else
				statusManager.setStatus(SlimeVR::Status::LOW_BATTERY, true);
#endif
			} else {
				statusManager.setStatus(SlimeVR::Status::LOW_BATTERY, false);
			}
#endif
		}
	}
#endif
}
