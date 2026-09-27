#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstddef>

static constexpr int TRANSMISSION_SIZE = 512;
static constexpr int MESSAGE_SIZE = 255;
static constexpr int UUID_SIZE = 127;
static constexpr int SN_SIZE = 33;

enum MessageType {
  HEARTBEAT,
  ALARMON,
  ALARMOFF,
  SYNC,
  ACKNOWLEDGE,
  ERROR
};

class DeviceID {
private:
  char serialNumber[SN_SIZE];
  char UUID[UUID_SIZE];

  void getArduinoSerialNumber(char *serialNumber, size_t bufferSize);

public:
  DeviceID();
  DeviceID(char* id);
  
  const char* getID();
};

class Message {
private:
  MessageType type;
  DeviceID deviceID;
  char text[255];

public:
  Message();
  Message(MessageType type);
  Message(MessageType type, const char* message);
  Message(MessageType type, DeviceID deviceID, const char* message);
  
  int getType();
  const char* getID();
  const char* getText();
};

#endif