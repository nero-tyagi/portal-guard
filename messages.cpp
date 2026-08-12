// messages.cpp
#include <SPI.h>
#include <RH_ASK.h>
#include "transmit.h"

void transmit_heartbeat(RH_ASK &radio) {
  char serialNumber[33];
  char message[50];

  getArduinoSerialNumber(serialNumber, sizeof(serialNumber));
  snprintf(message, sizeof(message), "ID:%s", serialNumber, ";", "HEARTBEAT");

  transmitMessage(radio, message);
  Serial.print("\nTransmitting: ");
  Serial.println(message);
  // delay(5000);
}

void transmit_alarm_status(RH_ASK &radio, bool status) { 
  char serialNumber[33];
  char message[50];

  getArduinoSerialNumber(serialNumber, sizeof(serialNumber));
  if (status) {
    snprintf(message, sizeof(message), "ID:%s", serialNumber, ";", "ALARM");
  }
  else {
    snprintf(message, sizeof(message), "ID:%s", serialNumber, ";", "NOALARM");
  }

  transmitMessage(radio, message);
  Serial.print("\nTransmitting: ");
  Serial.println(message);
}