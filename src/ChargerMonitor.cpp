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

#include "ChargerMonitor.h"
#include "GlobalVars.h"
#include "status/Status.h"

namespace SlimeVR
{
    void ChargerMonitor::setup()
    {
        m_Logger.info("Setting up Charger interface...");
        pinMode(m_Pin, INPUT_PULLUP);

        if (!PMIC.begin()) 
        {
            isSetup = false;
            m_Logger.warn("Failed to initialize PMIC!");
        } else
            isSetup = true;

        // Set the input current limit to 2 A and the overload input voltage to 3.88 V
        if (!PMIC.setInputCurrentLimit(2.0)) 
        {
            isSetup = false;
            m_Logger.warn("Error in set input current limit");
        }

        if (!PMIC.setInputVoltageLimit(4.36)) 
        {
            isSetup = false;
            m_Logger.warn("Error in set input voltage limit");
        }

        // set the minimum voltage used to feeding the module embed on Board
        if (!PMIC.setMinimumSystemVoltage(3.3)) 
        {
            isSetup = false;
            m_Logger.warn("Error in set minimum system volage");
        }

        // Set the desired charge voltage to 4.2 V
        if (!PMIC.setChargeVoltage(4.2)) 
        {
            isSetup = false;
            m_Logger.warn("Error in set charge volage");
        }

        // Set the max die temperature for thermal current regulation in deg C
        // only works for 60, 80, 100, 120 degrees C
        if (!PMIC.setThermalRegulationTemperature(60))
        {
            isSetup = false;
            m_Logger.warn("Error in set thermal regulation");
        }
        // Set the charge current to 1000 mA
        // the charge current should be defined as maximum at (C for hour)/1h
        // to avoid battery explosion (for example for a 750 mAh battery set to 0.750 A)
        if (!PMIC.setChargeCurrent(1.0)) 
        {
            isSetup = false;
            m_Logger.warn("Error in set charge current");
        }

        if (!PMIC.disableCharge()) 
        {
            isSetup = false;
            m_Logger.warn("Error disabling Charge mode");
        }

        if (!isSetup)
        {
            isSetup = false;
            m_Logger.warn("Charger Setup Failed");
        }

        statusManager.setStatus(Status::BATTERY_CHARGING, false);
        statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, true);
        digitalWrite(PIN_CHRG_DONE, HIGH);
        
        // Do the initial pull of the state
        update();
    }


    void ChargerMonitor::update()
    {
        if (isSetup)
        {
            unsigned long time = millis();
            unsigned long diff = time - m_LastUpdate;

            // Don't tick the ChargerManager *too* often, unless an Interrupt occurred
            if ((diff < 250) || (digitalRead(PIN_CHARGER_INT) == LOW) || (time < POWERTESTSTARTUPTIME))
            {
                return;
            }

            m_LastUpdate = time;
            if (PMIC.isPowerGood() && time >= POWERTESTSTARTUPTIME)
            {
                digitalWrite(PIN_ENABLE_LATCH, HIGH);

                // getChargeFault() returns the charge fault state, the fault could be:
                //   - Thermal shutdown: occurs if internal junction temperature exceeds
                //     the preset limit;
                //   - input over voltage: occurs if VBUS voltage exceeds 18 V;
                //   - charge safety timer expiration: occurs if the charge timer expires.
                switch (PMIC.getChargeFault()) 
                {
                    case INPUT_OVER_VOLTAGE: 
                        m_Logger.warn("Input over voltage fault occurred");
                        statusManager.setStatus(Status::BATTERY_CHARGING, false);
                        statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                        digitalWrite(PIN_CHRG_DONE, LOW);
                        break;
                    case THERMAL_SHUTDOWN: 
                        m_Logger.warn("Thermal shutdown occurred");
                        statusManager.setStatus(Status::BATTERY_CHARGING, false);
                        statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                        digitalWrite(PIN_CHRG_DONE, LOW);
                        break;
                    case CHARGE_SAFETY_TIME_EXPIRED: 
                        m_Logger.warn("Charge safety timer expired");
                        statusManager.setStatus(Status::BATTERY_CHARGING, false);
                        statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                        digitalWrite(PIN_CHRG_DONE, LOW);
                        break;
                    case NO_CHARGE_FAULT:
                        switch (PMIC.chargeStatus())
                        {
                            case NOT_CHARGING:
                                if (!PMIC.enableCharge()) 
                                {
                                    m_Logger.warn("Error enabling Charge mode");
                                    statusManager.setStatus(Status::BATTERY_CHARGING, false);
                                }
                                else
                                    statusManager.setStatus(Status::BATTERY_CHARGING, true);
                                statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                                digitalWrite(PIN_CHRG_DONE, LOW);
                                break;
                            case PRE_CHARGING:
                            case FAST_CHARGING:
                                statusManager.setStatus(Status::BATTERY_CHARGING, true);
                                statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                                digitalWrite(PIN_CHRG_DONE, LOW);
                                break;
                            case CHARGE_TERMINATION_DONE:
                                // if (!PMIC.disableCharge())
                                //     m_Logger.warn("Error disabling Charge mode");
                                statusManager.setStatus(Status::BATTERY_CHARGING, false);
                                statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, true);
                                digitalWrite(PIN_CHRG_DONE, HIGH);
                                break;
                            default:
                                statusManager.setStatus(Status::BATTERY_CHARGING, false);
                                statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                                digitalWrite(PIN_CHRG_DONE, LOW);
                                break;
                        }
                        break;
                    default:
                        break;
                }
            } else
            {
                statusManager.setStatus(Status::BATTERY_CHARGING, false);
                statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                digitalWrite(PIN_CHRG_DONE, LOW);
            }


            // The isBatteryInOverVoltage() returns if battery over-voltage fault occurs.
            // When battery over voltage occurs, the charger device immediately disables
            // charge and sets the battery fault bit, in fault register, to high.
            if (PMIC.isBatteryInOverVoltage()) 
            {
                m_Logger.warn("Error: battery over voltage fault");
                statusManager.setStatus(Status::BATTERY_CHARGING, false);
                statusManager.setStatus(Status::BATTERY_CHARGE_COMPLETE, false);
                digitalWrite(PIN_CHRG_DONE, LOW);
            }

            int batTempfault = PMIC.hasBatteryTemperatureFault();
            switch (batTempfault) 
            {
                case NO_TEMPERATURE_FAULT: 
                    //m_Logger.warn("No temperature fault");
                    break;
                case LOWER_THRESHOLD_TEMPERATURE_FAULT: 
                    m_Logger.warn("Lower threshold Battery temperature fault");
                    break;
                case HIGHER_THRESHOLD_TEMPERATURE_FAULT: 
                    m_Logger.warn("Higher threshold Battery temperature fault");
                    break;
                default:
                    break;
            }

        }
    }

    void ChargerMonitor::turnFETOff()
    {
        if (isSetup)
        {
            PMIC.disableWatchdog();
            PMIC.disableBATFET();
        }        
    }
}
