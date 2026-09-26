// messages.cpp
#include <SPI.h>
#include <RH_ASK.h>
#include "transmit.h"

void transmit_heartbeat(const byte TX_PIN, RH_ASK &radio) {
  char serialNumber[33];
  char message[50];

  getArduinoSerialNumber(serialNumber, sizeof(serialNumber));
  snprintf(message, sizeof(message), "ID:%s", serialNumber, ";", "HEARTBEAT");

  digitalWrite(TX_PIN, HIGH);
  transmitMessage(TX_PIN, radio, message);
  Serial.print("\nTransmitting: ");
  Serial.println(message);
  digitalWrite(TX_PIN, LOW);
  // delay(5000);
}

void transmit_alarm_status(const byte TX_PIN, RH_ASK &radio, bool status) { 
  char serialNumber[33];
  char message[50];

  getArduinoSerialNumber(serialNumber, sizeof(serialNumber));
  if (status) {
    snprintf(message, sizeof(message), "ID:%s", serialNumber, ";", "ALARM");
  }
  else {
    snprintf(message, sizeof(message), "ID:%s", serialNumber, ";", "NOALARM");
  }

  digitalWrite(TX_PIN, HIGH);
  transmitMessage(TX_PIN, radio, message);
  Serial.print("\nTransmitting: ");
  Serial.println(message);
  digitalWrite(TX_PIN, LOW);
}