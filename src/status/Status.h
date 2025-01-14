#ifndef STATUS_STATUS_H
#define STATUS_STATUS_H

namespace SlimeVR
{
    namespace Status
    {
        enum Status
        {
            LOADING = 1 << 0,
            LOW_BATTERY = 1 << 1,
            IMU_ERROR = 1 << 2,
            WIFI_CONNECTING = 1 << 3,
            SERVER_SEARCHING = 1 << 4,
            SERVER_CONNECTING = 1 << 5,
            BATTERY_CHARGING = 1 << 6,
            BATTERY_CHARGE_COMPLETE = 1 << 7,
            SHUTDOWN_INITIATED = 1 << 8,
            SHUTDOWN_COMPLETE = 1 << 9
        };

        const char *statusToString(Status status);
    }
}

#endif
