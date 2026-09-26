// messages.cpp
#include <SPI.h>
#include <RH_ASK.h>
#include "transmit.h"

void transmit_heartbeat(const byte TX_PIN, RH_ASK &radio);
void transmit_alarm_status(const byte TX_PIN, RH_ASK &radio, bool status);