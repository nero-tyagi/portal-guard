// transmit.cpp
#include "transmit.h"

void getArduinoSerialNumber(char *serialNumber, size_t bufferSize) {
  uint32_t word0 = *(volatile uint32_t *)0x0080A00C;
  uint32_t word1 = *(volatile uint32_t *)0x0080A040;
  uint32_t word2 = *(volatile uint32_t *)0x0080A044;
  uint32_t word3 = *(volatile uint32_t *)0x0080A048;

  snprintf(serialNumber, bufferSize, "%08lX%08lX%08lX%08lX",
           (unsigned long)word0, (unsigned long)word1,
           (unsigned long)word2, (unsigned long)word3);
}

void transmitMessage(const byte TX_PIN, RH_ASK &radio, const char *message) {
  digitalWrite(TX_PIN, HIGH);
  radio.send((uint8_t *)message, strlen(message));
  radio.waitPacketSent();
  digitalWrite(TX_PIN, LOW);
}