#ifndef GUARD_H
#define GUARD_H

#include <Arduino.h>

// CONSTANTS ____________________________________________________

enum class GuardStatus {
    STANDBY,
    BUSY,
    PAIRING,
    ARMED,
    ALARM,
    ERROR
};

struct Guard {
    static const byte TX_PIN = 13; // radio pin
    static const uint16_t RADIO_BITRATE = 2000; // Must exactly match Sentinel's RadioHead bitrate and its TX pin configuration

    static constexpr int IMU_SUCCESS = 0;
    static constexpr int MEAN_SAMPLING_SIZE = 100;

    static constexpr unsigned long INIT_DURATION = 10000UL;
    static constexpr unsigned long ALARM_DURATION = 2000UL;
    static constexpr unsigned long HEARTBEAT_INT = 120000UL;

    static GuardStatus status;
};

#endif
