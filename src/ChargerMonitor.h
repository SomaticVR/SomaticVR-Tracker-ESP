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
#pragma once

#include "BQ24195.h"
#include "globals.h"
#include "logging/Logger.h"

#define POWERTESTSTARTUPTIME 32000

namespace SlimeVR
{
    class ChargerMonitor
    {
    public:
        ChargerMonitor(uint8_t pin) : m_Pin(pin) {}

        void setup();

        void update();

        void turnFETOff();

    private:
        uint8_t m_CurrentCount = 0;
        unsigned long m_Timer = 0;
        unsigned long m_LastUpdate = 0;

        bool isSetup = false;
        uint8_t m_addr;
        uint8_t m_Pin;

        Logging::Logger m_Logger = Logging::Logger("ChargerMonitor");
    };
}
