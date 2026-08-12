// messages.cpp
#include <SPI.h>
#include <RH_ASK.h>
#include "transmit.h"

void transmit_heartbeat(RH_ASK &radio);
void transmit_alarm_status(RH_ASK &radio, bool status);