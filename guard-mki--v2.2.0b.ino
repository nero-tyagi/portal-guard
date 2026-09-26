#include <LSM6DS3-SOLDERED.h>
#include "RTCZero.h"
#include <RH_ASK.h>

#include "messages.h"

constexpr int IMU_SUCCESS = 0;

// Components.
Soldered_LSM6DS3 lsm6ds3(
    LSM6DS3_ACC_GYRO_I2C_ADDRESS_LOW
);

const byte TX_PIN = 13; // radio pin
RH_ASK radio(2000, 0xFF, TX_PIN, 0xFF); // radio object


// Timer variables for initializing threshold values for the IMU
// The guard's environment may have substantial acoustic or mechanical vibrations which aren't meant to trigger the alarm; the threshold must account for such vibrations
RTCZero intialize_timer;
int initialize_reading_count = 0;
bool IMU_initializing = false;
unsigned long initialize_length = 10000;

// IMU acceleration variables
int32_t g[3];
int32_t g_avg[3] = {0, 0, 0};
int32_t g_threshold[3] = {50, 50, 50}; // threshold values in milli-g's

// Guard variables
bool armed = true; // in the future, guard will have a receiver that will look for arming signals from a remote; currently, guard is armed by default when it's powered

// Timer for the alarm
RTCZero alarm_timer;
bool ALARM = false;
unsigned long alarm_length = 30000;

// Timer for the heartbeat
// A heartbeat is necessary to let the sentinel know the guard is still alive; in an event where the guard is somehow neutralized, the sentinel must be informed somehow
RTCZero heartbeat_timer;
unsigned long heartbeat_length = 10000;

void serialize_json() { }
String authorize_packet() { }

void arm() { }

void setup() {

  // Serial.begin(115200);
  // while (!Serial);

  // Initialize I2C bus.
  Wire.begin();

  // Initialize components.
  lsm6ds3.begin(); // initializing the IMU sensor
  lsm6ds3.enableAccelerator();

  if (!radio.init()) { // initializing the radio module
    Serial.print("Radio initialization failed.");
    while (true) { }
  }

  Serial.print("Radio initialized.");

  // ____ Low power settings ____
  
  lsm6ds3.disableGyro();
  uint8_t ctrl1;
  // Read the current accelerometer configuration
  lsm6ds3.readRegister(
      LSM6DS3_ACC_GYRO_CTRL1_XL,
      &ctrl1
  );

  // Clear only the ODR bits (upper four bits)
  ctrl1 &= 0x0F;

  // Set accelerometer ODR to approximately 13 Hz
  ctrl1 |= LSM6DS3_ACC_GYRO_ODR_XL_13Hz;

  // Write the modified configuration back
  lsm6ds3.writeRegister(
      LSM6DS3_ACC_GYRO_CTRL1_XL,
      ctrl1
  );

  // Turn off the NINA comms chip (WiFi and BLE) to save power.
  pinMode(NINA_RESETN, OUTPUT);
  digitalWrite(NINA_RESETN, LOW);

  // ________

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
      // Serial.print(currentSecond);
      // Serial.print("...");
      previousSecond = currentSecond;
    }

    if (lsm6ds3.getAcceleratorAxes(g) == 0) {
      initialize_reading_count++;
      for (int i=0; i<3; i++) {
        g_avg[i] += g[i];
      }
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
    g_threshold[i] += abs(g_avg[i]);
  }
}

void loop() {

  digitalWrite(TX_PIN, LOW);
  // Beginning heartbeat to let the sentinel know that guard is active
  heartbeat_timer.begin();
  transmit_heartbeat(TX_PIN, radio);
  // Serial.print("HEARTBEAT - NO ALARM");

  unsigned long heartbeatStart = millis();
  int previousSecondHeartbeat = 0;
  int currentSecondHeartbeat = 0;
  while (millis() - heartbeatStart < heartbeat_length) {
    currentSecondHeartbeat = (millis() - heartbeatStart) / 1000; // this "previousSecond" bit is only here to make Serial output prettier; can be removed for prod version

    if (previousSecondHeartbeat != currentSecondHeartbeat) {
      // Serial.print(currentSecondHeartbeat);
      // Serial.print("...");
      previousSecondHeartbeat = currentSecondHeartbeat;
    }

    // Checking for movement in between heartbeats
    if (lsm6ds3.getAcceleratorAxes(g) == IMU_SUCCESS) {
      if (abs(g[0]) > abs(g_threshold[0])) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      }
      if (abs(g[1]) > abs(g_threshold[1])) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      }
      if (abs(g[2]) > abs(g_threshold[2])) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      }
    }
  }

  if (ALARM) {
    alarm_timer.begin();
    transmit_alarm_status(TX_PIN, radio, ALARM);
    unsigned long alarmStart = millis();
    int previousSecondAlarm = 0;
    int currentSecondAlarm = 0;

    while (millis() - alarmStart < alarm_length) {
      currentSecondAlarm = (millis() - alarmStart) / 1000; // this "previousSecond" bit is only here to make Serial output prettier; can be removed for prod version

      if (previousSecondAlarm != currentSecondAlarm) {
        // Serial.print(currentSecondAlarm);
        // Serial.print("...");
        previousSecondAlarm = currentSecondAlarm;
      }

      // Resetting the timer if the guard is continually moved so the alarm stops only once the guard has been left alone for more than the alarm length
      if (lsm6ds3.getAcceleratorAxes(g) == IMU_SUCCESS) {
        if (abs(g[0]) > abs(g_threshold[0])) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(TX_PIN, radio, ALARM);
          alarmStart = millis();
        }
        if (abs(g[1]) > abs(g_threshold[1])) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(TX_PIN, radio, ALARM);
          alarmStart = millis();
        }
        if (abs(g[2]) > abs(g_threshold[2])) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(TX_PIN, radio, ALARM);
          alarmStart = millis();
        }
      }
    }
    ALARM = false;
          transmit_alarm_status(TX_PIN, radio, ALARM);
    // Serial.println("NO ALARM\n");
  }
}