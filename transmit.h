#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <Arduino.h>
#include <RH_ASK.h> // RadioHead – radio transceiver library

void transmitMessage(const byte TX_PIN, RH_ASK &radio, const char *message);
void transmit_heartbeat(const byte TX_PIN, RH_ASK &radio);
void transmit_alarm_status(const byte TX_PIN, RH_ASK &radio, bool status);

#endif