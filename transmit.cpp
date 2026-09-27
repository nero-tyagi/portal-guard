#include "transmit.h"
#include "message.h"
#include <SPI.h> //RadioHead's dependency

void transmitMessage(const byte TX_PIN, RH_ASK &radio, const char *message) {
  digitalWrite(TX_PIN, HIGH);
  radio.send((uint8_t *)message, strlen(message));
  radio.waitPacketSent();
  digitalWrite(TX_PIN, LOW);
}

void transmit_heartbeat(const byte TX_PIN, RH_ASK &radio) {
  MessageType type = MessageType::HEARTBEAT;
  Message heartbeat(type);
  char transmission[TRANSMISSION_SIZE];
  snprintf(transmission, sizeof(transmission),
          "{\"type\":%i,\"id\":\"%s\",\"msg\":\"%s\"}",
          heartbeat.getType(), heartbeat.getID(), heartbeat.getText());

  digitalWrite(TX_PIN, HIGH);
  transmitMessage(TX_PIN, radio, transmission);
  Serial.print("\nTransmitting: ");
  Serial.println(transmission);
  digitalWrite(TX_PIN, LOW);
  // delay(5000);
}

void transmit_alarm_status(const byte TX_PIN, RH_ASK &radio, bool status) {
  MessageType type;
  if (status) {
    type = MessageType::ALARMON;
  } else {
    type = MessageType::ALARMOFF;
  }

  Message alarm(type);
  char transmission[TRANSMISSION_SIZE];
  snprintf(transmission, sizeof(transmission),
          "{\"type\":%i,\"id\":\"%s\",\"msg\":\"%s\"}",
          alarm.getType(), alarm.getID(), alarm.getText());

  digitalWrite(TX_PIN, HIGH);
  transmitMessage(TX_PIN, radio, transmission);
  Serial.print("\nTransmitting: ");
  Serial.println(transmission);
  digitalWrite(TX_PIN, LOW);
}