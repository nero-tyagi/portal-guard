#ifndef GUARD_H
#define GUARD_H

#include <Arduino.h>

// CONSTANTS ____________________________________________________

enum GuardStatus {
    STANDBY,
    BUSY,
    PAIRING,
    ARMED,
    ALARM,
};

struct Guard {
    static const byte TX_PIN = 13; // radio pin

    static constexpr int IMU_SUCCESS = 0;
    static constexpr int MEAN_SAMPLING_SIZE = 100;

    static constexpr unsigned long INIT_DURATION = 10000UL;
    static constexpr unsigned long ALARM_DURATION = 2000UL;
    static constexpr unsigned long HEARTBEAT_INT = 120000UL;

    static GuardStatus status;
};

#endif
