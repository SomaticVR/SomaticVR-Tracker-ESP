/*
	SomaticVR Code is placed under the MIT license
	Copyright (c) 2023 SomaticVR, LLC

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

#include "ButtonMonitor.h"

#include "GlobalVars.h"
#include "status/Status.h"

#define SHUTDOWN_BUTTON_TIME 2000
#define FULL_OFF_BUTTON_TIME 10000

namespace SlimeVR {
void ButtonMonitor::setup() {
	pinMode(m_Pin, INPUT);
	// Do the initial pull of the state
	update();
}

bool ButtonMonitor::isPressed() { return digitalRead(m_Pin) == HIGH; }

void ButtonMonitor::update() {
	unsigned long time = millis();
	unsigned long diff = time - m_LastUpdate;

	// Don't tick the ButtonMonitor *too* often
	if (diff < 10) {
		return;
	}

	m_LastUpdate = time;

	// Advance stage
	switch (m_CurrentState) {
		case RELEASED:
			if (isPressed()) {
				m_CurrentState = JUSTPRESSED;
				m_PressedTime = time;
			}
			break;
		case JUSTPRESSED:
			m_Logger.trace("Button Pressed");
			m_CurrentState = PRESSED;
		case PRESSED:
			if (isPressed()) {
				unsigned long pressedDiff = time - m_PressedTime;
				// m_Logger.debug("Pressed diff: %ul", pressedDiff);
                if (pressedDiff >= FULL_OFF_BUTTON_TIME)
                {
                    if (!statusManager.hasStatus(Status::SHUTDOWN)) {
                        statusManager.setStatus(Status::SHUTDOWN, false);
					}
					m_Logger.info("Entering Full Off");
					ledManager.off();
					chargerMonitor.turnFETOff();
					digitalWrite(PIN_ENABLE_LATCH, LOW);
					// Deep sleep for the max uint32 amount of time, effectively a shutdown
					ESP.deepSleep(-1);

                } else if (pressedDiff >= SHUTDOWN_BUTTON_TIME)
                {
                    if (!statusManager.hasStatus(Status::SHUTDOWN)) {
                        statusManager.setStatus(Status::SHUTDOWN, true);
                        m_Logger.info("Entering Shutdown");
                    }
                }
			} else {
				m_CurrentState = JUSTRELEASED;
			}
			break;
		case JUSTRELEASED:
			m_Logger.trace("Button Released");
			if (statusManager.hasStatus(Status::SHUTDOWN)) {
				statusManager.setStatus(Status::SHUTDOWN, false);
				digitalWrite(PIN_ENABLE_LATCH, LOW);
			}
			m_CurrentState = RELEASED;
			break;
	}
}
}  // namespace SlimeVR
