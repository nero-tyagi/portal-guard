#include <LSM6DS3-SOLDERED.h>
#include "RTCZero.h"
#include <RH_ASK.h>

#include "transmit.h"

constexpr int IMU_SUCCESS = 0;

// Components.
Soldered_LSM6DS3 lsm6ds3(
    LSM6DS3_ACC_GYRO_I2C_ADDRESS_LOW
);

const byte TX_PIN = 13; // radio pin
RH_ASK radio(2000, 0xFF, TX_PIN, 0xFF); // radio object


// Timer variables for initializing threshold values for the IMU
// The guard's environment may have substantial acoustic or mechanical vibrations which aren't meant to trigger the alarm; the threshold must account for such vibrations. Therefore, it uses a 10 second period to average out its g values.
RTCZero intialize_timer;
int init_sample_count = 0;
bool IMU_initializing = false;
unsigned long initialize_length = 10000;

// IMU acceleration variables
int32_t g[3];
int32_t g_mean[3] = {0, 0, 0};
int32_t g_llimits[3] = {0, 0, 0};
int32_t g_ulimits[3] = {0, 0, 0};
int32_t g_custom_threshold = 50; // in milli-g's. Gets applied to both upper and lower limits.
bool mean_calculated = false;

// Guard variables
bool armed = true; // in the future, guard will have a receiver that will look for arming signals from a remote; currently, guard is armed by default when it's powered

// Timer for the alarm
RTCZero alarm_timer;
bool ALARM = false;
unsigned long alarm_length = 2000;

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
  int mean_count = 100;

  while (millis() - initializationStart < initialize_length) {

    currentSecond = (millis() - initializationStart) / 1000;
    if (previousSecond != currentSecond) {  // this "previousSecond" bit is only here to make Serial output prettier; can be removed for prod version
      Serial.print(currentSecond);
      Serial.print("...");
      previousSecond = currentSecond;
    }

    if (lsm6ds3.getAcceleratorAxes(g) == 0) {
      init_sample_count++;
      if (init_sample_count < mean_count) {
        for (int i = 0; i < 3; i++) {
          g_mean[i] += g[i];
        }
      } else if (init_sample_count == mean_count) {
        for (int i = 0; i < 3; i++) {
          g_mean[i] /= mean_count;
          g_llimits[i] = g_mean[i];
          g_ulimits[i] = g_mean[i];
        }
      } else {
        for (int i=0; i<3; i++) {
          if (g[i] < g_llimits[i]) {
            g_llimits[i] = g[i];
          };
          if (g[i] > g_ulimits[i]) {
            g_ulimits[i] = g[i];
          };
        }
      }
    }
  }

  Serial.println();
  IMU_initializing = false;

  if (!IMU_initializing) {
    Serial.println(init_sample_count+(String)" readings recorded. Averaging and setting new threshold...");
    char g_values[300];
    snprintf(g_values, sizeof(g_values),
      "g limits: \nx limits: [%d, %d]\ny limits: [%d, %d]\nz limits: [%d, %d].\n\n mean g values: [%d, %d, %d]",
      g_llimits[0], g_ulimits[0], g_llimits[1], g_ulimits[1], g_llimits[2], g_ulimits[2],
      g_mean[0], g_mean[1], g_mean[2]);
    Serial.println(g_values);
  }

  // Changing threshold relative to the recorded floor
    for (int i=0; i<3; i++) {
      g_llimits[i] -= g_custom_threshold;
      g_ulimits[i] += g_custom_threshold;
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
      Serial.print(currentSecondHeartbeat);
      Serial.print("...");
      previousSecondHeartbeat = currentSecondHeartbeat;
    }

    // Checking for movement in between heartbeats
    if (lsm6ds3.getAcceleratorAxes(g) == IMU_SUCCESS) {
      if (g[0] < g_llimits[0] || g[0] > g_ulimits[0]) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      };
      if (g[1] < g_llimits[1] || g[1] > g_ulimits[1]) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      };
      if (g[2] < g_llimits[2] || g[2] > g_ulimits[2]) {
        Serial.println("ALARM");
        ALARM = true;
        break;
      };
    };
  };

  if (ALARM) {
    alarm_timer.begin();
    transmit_alarm_status(TX_PIN, radio, ALARM);
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
      if (lsm6ds3.getAcceleratorAxes(g) == IMU_SUCCESS) {
        if (g[0] < g_llimits[0] || g[0] > g_ulimits[0]) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(TX_PIN, radio, ALARM);
          alarmStart = millis();
        }
        if (g[1] < g_llimits[1] || g[1] > g_ulimits[1]) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(TX_PIN, radio, ALARM);
          alarmStart = millis();
        }
        if (g[2] < g_llimits[2] || g[2] > g_ulimits[2]) {
          Serial.println("ALARM");
          ALARM = true;
          transmit_alarm_status(TX_PIN, radio, ALARM);
          alarmStart = millis();
        }
      }
    }
    ALARM = false;
          transmit_alarm_status(TX_PIN, radio, ALARM);
  }
}