#include <SPI.h> //RadioHead's dependency

#include "transmit.h"
#include "message.h"
#include "guard.h"

void transmitMessage(const byte TX_PIN, RH_ASK &radio, Message msg) {
  // Creating the transmission
  char message[TRANSMISSION_SIZE];
  snprintf(message, sizeof(message),
        "{\"type\":%i,\"id\":\"%s\",\"msg\":\"%s\"}",
        msg.getType(), msg.getID(), msg.getText());

  digitalWrite(TX_PIN, HIGH);
  radio.send((uint8_t *)message, strlen(message));
  radio.waitPacketSent();
  digitalWrite(TX_PIN, LOW);
}

void transmit_heartbeat(const byte TX_PIN, RH_ASK &radio) {
  MessageType type = MessageType::HEARTBEAT;
  Message heartbeat(type);
  transmitMessage(TX_PIN, radio, heartbeat);
}

void transmit_alarm_on(const byte TX_PIN, RH_ASK &radio, GuardStatus &status) {
  guard.status = GuardStatus::ALARM;
  MessageType type = MessageType::ALARMON;
  Message alarm(type);
  transmitMessage(TX_PIN, radio, alarm);
}

void transmit_alarm_off(const byte TX_PIN, RH_ASK &radio, GuardStatus &status) {
  guard.status = GuardStatus::ARMED;
  MessageType type = MessageType::ALARMON;
  Message alarm(type);
  transmitMessage(TX_PIN, radio, alarm);
}
