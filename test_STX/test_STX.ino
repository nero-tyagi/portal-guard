#include <RH_ASK.h>
#include <SPI.h>

constexpr uint8_t TX_PIN = 13;
RH_ASK radio(2000, 0xFF, TX_PIN, 0xFF);

void setup() {
  Serial.begin(115200);
  delay(2000);

  if (!radio.init()) {
    Serial.println("RadioHead failed");
    while (true) {}
  }

  Serial.println("Guard transmitter ready");
}

void loop() {
  const char message[] = "HELLO";

  radio.send((uint8_t *)message, strlen(message));
  radio.waitPacketSent();

  Serial.println("Sent HELLO");
  delay(1000);
}