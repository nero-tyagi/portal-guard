// DeviceID class definitions

#include "message.h"
#include <stdint.h>
#include <cstdio>
#include <cstring>

void DeviceID::getArduinoSerialNumber(char *serialNumber, size_t bufferSize) {
  uint32_t word0 = *(volatile uint32_t *)0x0080A00C;
  uint32_t word1 = *(volatile uint32_t *)0x0080A040;
  uint32_t word2 = *(volatile uint32_t *)0x0080A044;
  uint32_t word3 = *(volatile uint32_t *)0x0080A048;

  snprintf(serialNumber, bufferSize, "%08lX%08lX%08lX%08lX",
           (unsigned long)word0, (unsigned long)word1,
           (unsigned long)word2, (unsigned long)word3);
};

DeviceID::DeviceID() {
  this->getArduinoSerialNumber(this->serialNumber, sizeof(this->serialNumber));

  // Change this when introducing encryption
  strncpy(this->UUID, this->serialNumber, sizeof(this->UUID) - 1);
  this->UUID[sizeof(this->UUID) - 1] = '\0';
};

DeviceID::DeviceID(char* id) {
  strncpy(this->UUID, id, sizeof(this->UUID) - 1);
  this->UUID[sizeof(this->UUID) - 1] = '\0';
}

const char* DeviceID::getID() {
  return this->UUID;
};

// Message class definitions

Message::Message() {
  this->type = MessageType::ACKNOWLEDGE;
  DeviceID deviceID;
  this->deviceID = deviceID;
  strncpy(this->text, "", sizeof(this->text) - 1);
  this->text[sizeof(this->text) - 1] = '\0';
}
 
Message::Message(MessageType type) {
  this->type = type;
  DeviceID deviceID;
  this->deviceID = deviceID;
  strncpy(this->text, "", sizeof(this->text) - 1);
  this->text[sizeof(this->text) - 1] = '\0';
}

Message::Message(MessageType type, const char* message) {
  this->type = type;
  DeviceID deviceID;
  this->deviceID = deviceID;
  strncpy(this->text, message, sizeof(this->text) - 1);
  this->text[sizeof(this->text) - 1] = '\0';
}

Message::Message(MessageType type, DeviceID deviceID, const char* message) {
  this->type = type;
  this->deviceID = deviceID;
  strncpy(this->text, message, sizeof(this->text) - 1);
  this->text[sizeof(this->text) - 1] = '\0';
}

int Message::getType() {
  return this->type;
}
const char* Message::getID() {
  return this->deviceID.getID();
}
const char* Message::getText() {
  return this->text;
}