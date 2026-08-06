/*
  Arduino LSM6DS3 - Simple Accelerometer

  This example reads the acceleration values from the LSM6DS3
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Uno WiFi Rev 2 or Arduino Nano 33 IoT

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include <Arduino_LSM6DS3.h>
#include "RTCZero.h"

// Timer variables for initializing threshold values for the IMU
// The guard's environment may have substantial acoustic or mechanical vibrations which aren't meant to trigger the alarm; the threshold must account for such vibrations
RTCZero intialize_timer;
int initialize_reading_count = 0;
bool IMU_initializing = false;
unsigned long initialize_length = 10000;

// IMU acceleration variables
float x, y, z;
float g_avg[3] = {0, 0, 0};
float g_threshold[3] = {0.1, 0.1, 0.1};

// Guard variables
bool armed = true; // in the future, guard will have a receiver that will look for arming signals from a remote; currently, guard is armed by default when it's powered

// Timer for the alarm
RTCZero alarm_timer;
bool ALARM = false;
unsigned long alarm_length = 30000;

// Timer for the heartbeat
// A heartbeat is necessary to let the sentinel know the guard is still alive; in an event where the guard is somehow neutralized, the sentinel must be informed somehow
RTCZero heartbeat_timer;
unsigned long heartbeat_length = 30000;

void serialize_json() { }
String authorize_packet() { }
void transmit_heartbeat() { }
void transmit_alarm_status(bool status) { }
void arm() { }

void setup() {

  Serial.begin(300);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.print(" Hz");
  Serial.println();

  // Gathering IMU acceleration readings to average out environmental vibrations and account for them in the threshold values
  Serial.print("Initializing IMU readings for 10 seconds...");
  IMU_initializing = true;

  intialize_timer.begin();
  unsigned long initializationStart = millis();
  int previousSecond = 0;
  int currentSecond = 0;

  while (millis() - initializationStart < initialize_length) {

    currentSecond = (millis() - initializationStart) / 1000; // this "previousSecond" bit is only here to make Serial output prettier; can be removed for prod version

    if (previousSecond != currentSecond) {
      Serial.print(currentSecond);
      Serial.print("...");
      previousSecond = currentSecond;
    }

    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);
      initialize_reading_count++;
      g_avg[0] += x;
      g_avg[1] += y;
      g_avg[2] += z;
    }
  }

  Serial.println();
  IMU_initializing = false;

  if (!IMU_initializing) {
    Serial.println(initialize_reading_count+(String)" readings recorded. Averaging and setting new threshold...");
    for (int i=0; i<3; i++) {
      g_avg[i] = g_avg[i]/initialize_reading_count;
    }
    
    Serial.print("x threshold: ");
    Serial.println(g_avg[0]);
    Serial.print("y threshold: ");
    Serial.println(g_avg[1]);
    Serial.print("z threshold: ");
    Serial.println(g_avg[2]);
  }

  // Changing threshold relative to the recorded floor
    for (int i=0; i<3; i++) {
    g_threshold[i] += g_avg[i];
  }
}

void loop() {

  // Beginning heartbeat to let the sentinel know that guard is active
  heartbeat_timer.begin();
  transmit_heartbeat();
  Serial.println("HEARTBEAT - NO ALARM");

  unsigned long heartbeatStart = millis();
  int previousSecondHeartbeat = 0;
  int currentSecondHeartbeat = 0;
  while (millis() - heartbeatStart < heartbeat_length) {
    currentSecondHeartbeat = (millis() - heartbeatStart) / 1000; // this "previousSecond" bit is only here to make Serial output prettier; can be removed for prod version

    if (previousSecondHeartbeat != currentSecondHeartbeat) {
      Serial.print(currentSecondHeartbeat);
      Serial.print("...");
      previousSecondHeartbeat = currentSecondHeartbeat;
    }

    // Checking for movement in between heartbeats
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);
      if (abs(x) > abs(g_threshold[0])) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      }
      if (abs(y) > abs(g_threshold[1])) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      }
      if (abs(z) > abs(g_threshold[2])) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      }
    }
  }

  if (ALARM) {
    alarm_timer.begin();
    transmit_alarm_status(ALARM);
    unsigned long alarmStart = millis();
    int previousSecondAlarm = 0;
    int currentSecondAlarm = 0;

    while (millis() - alarmStart < alarm_length) {
      currentSecondAlarm = (millis() - alarmStart) / 1000; // this "previousSecond" bit is only here to make Serial output prettier; can be removed for prod version

      if (previousSecondAlarm != currentSecondAlarm) {
        Serial.print(currentSecondAlarm);
        Serial.print("...");
        previousSecondAlarm = currentSecondAlarm;
      }

      // Resetting the timer if the guard is continually moved so the alarm stops only once the guard has been left alone for more than the alarm length
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z);
        if (abs(x) > abs(g_threshold[0])) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(ALARM);
          alarmStart = millis();
        }
        if (abs(y) > abs(g_threshold[1])) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(ALARM);
          alarmStart = millis();
        }
        if (abs(z) > abs(g_threshold[2])) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(ALARM);
          alarmStart = millis();
        }
      }
    }
    ALARM = false;
    transmit_alarm_status(ALARM);
    Serial.println("NO ALARM");
  }
}
