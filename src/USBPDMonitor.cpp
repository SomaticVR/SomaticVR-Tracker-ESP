/*
    SomaticVR Code is placed under the MIT license
    Copyright (c) 2024 SomaticVR, LLC

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

#include "USBPDMonitor.h"
#include "GlobalVars.h"
#include "status/Status.h"

/* For use in PD_protoco_get_power_info() */
#define PD_Vrev(v)     (((float)v - 0.01 / 20.0))
#define PD_Arev(a)     (((float)a - 0.01 / 100.0))

/* For use in PD_protocol_set_PPS_option() */
#define PPS_Vrev(v)    ((uint16_t)(v * 50 + 0.01))
#define PPS_Arev(a)    ((uint8_t)(a * 20 + 0.01))

namespace SlimeVR
{
    void USBPDMonitor::setup()
    {
        m_Logger.info("Setting up USB PD...");
        // pinMode(m_Pin, INPUT);
        device.init(m_Pin, PD_POWER_OPTION_MAX_12V);
        isSetup = true;
        // Do the initial pull of the state
        update();
    }


    void USBPDMonitor::update()
    {
        if (isSetup)
        {

            // m_Logger.info("PD LOOP");
            unsigned long time = millis();
            unsigned long diff = time - m_LastUpdate;

            device.run();

            // Don't tick the USBPDMonitor *too* often
            if (diff < 15000)
            {
                return;
            }

            m_LastUpdate = time;


            // device.print_status();
            if (device.is_power_ready() && (device.get_voltage() < 98.0f))
            {
                
                m_Logger.debug("PD Voltage: %f", PD_Vrev(device.get_voltage()));
                m_Logger.debug("PD Current: %f", PD_Arev(device.get_current()));
            }
        }
    }
}
