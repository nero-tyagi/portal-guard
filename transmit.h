#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <Arduino.h>
#include <RH_ASK.h> // RadioHead – radio transceiver library

#include "message.h"
#include "guard.h"

void transmitMessage(const byte TX_PIN, RH_ASK &radio, Message msg);
void transmit_heartbeat(const byte TX_PIN, RH_ASK &radio);
void transmit_alarm_on(const byte TX_PIN, RH_ASK &radio, GuardStatus &status);
void transmit_alarm_off(const byte TX_PIN, RH_ASK &radio, GuardStatus &status);

#endif
