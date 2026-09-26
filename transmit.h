// transmit.h
#pragma once

#include <RH_ASK.h>
#include <SPI.h>

void getArduinoSerialNumber(char *serialNumber, size_t bufferSize);
void transmitMessage(const byte TX_PIN, RH_ASK &radio, const char *message);