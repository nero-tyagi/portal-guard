#include <LSM6DS3-SOLDERED.h>
#include "RTCZero.h"
#include <RH_ASK.h>

#include "transmit.h"

// ARDUINO COMPONENTS ___________________________________________

Soldered_LSM6DS3 lsm6ds3(
    LSM6DS3_ACC_GYRO_I2C_ADDRESS_LOW
);

RH_ASK radio(2000, 0xFF, Guard::TX_PIN, 0xFF); // radio object

//_______________________________________________________________

// Timer for initializing g limits and threshold values
// The guard's environment may have substantial acoustic or mechanical vibrations which aren't meant to trigger the alarm; the threshold must account for such vibrations. Therefore, it uses a 10 second period to average out its g values.
RTCZero intialize_timer;
int init_sample_count = 0;

// IMU acceleration variables
int32_t g[3];
int32_t g_mean[3] = {0, 0, 0}; // mean values of g calculated during initialization
int32_t g_llimits[3] = {0, 0, 0}; // lower-limit for triggering
int32_t g_ulimits[3] = {0, 0, 0}; // upper-limit for triggering
int32_t g_custom_threshold = 25; // in milli-g's. Gets applied to both upper and lower limits. Could be 0 for very sophisticated applications.

// Timer for the alarm
RTCZero alarm_timer;

// Timer for the heartbeat
// A heartbeat is necessary to let the sentinel know the guard is still alive; in an event where the guard is somehow neutralized, the sentinel must be informed somehow
RTCZero heartbeat_timer;

void setup() {

  // Serial.begin(115200);
  // while (!Serial);

  // Initialize I2C bus.
  Wire.begin();

  // Initialize components.
  lsm6ds3.begin(); // initializing the IMU sensor
  lsm6ds3.enableAccelerator();

  if (!radio.init()) { // initializing the radio module
    while (true) { }
  }

  // LOW POWER SETUP ____________________________________________

  pinMode(NINA_RESETN, OUTPUT);
  digitalWrite(NINA_RESETN, LOW); // Turn off the NINA comms chip (WiFi and BLE) to save power.
  lsm6ds3.disableGyro(); // Only need accelerometer – no need for gyro

  // Read the current accelerometer configuration
  uint8_t ctrl1;
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

  // IMU Initialization _________________________________________

  // Gathering IMU acceleration readings to average out environmental vibrations and account for them in the threshold values
  intialize_timer.begin();
  unsigned long initializationStart = millis();

  while (millis() - initializationStart < Guard::INIT_DURATION) {

    if (lsm6ds3.getAcceleratorAxes(g) == 0) {
      init_sample_count++;
      if (init_sample_count <= Guard::MEAN_SAMPLING_SIZE) {
        for (int i = 0; i < 3; i++) {
          g_mean[i] += g[i];
        }
      }
      if (init_sample_count == Guard::MEAN_SAMPLING_SIZE) {
        for (int i = 0; i < 3; i++) {
          g_mean[i] /= Guard::MEAN_SAMPLING_SIZE;
          g_llimits[i] = g_mean[i];
          g_ulimits[i] = g_mean[i];
        }
      } else {
        for (int i=0; i<3; i++) {
          if (g[i] < g_llimits[i]) {
            g_llimits[i] = g[i];
          }
          if (g[i] > g_ulimits[i]) {
            g_ulimits[i] = g[i];
          }
        }
      }
    }
  }

  // Changing threshold relative to the recorded floor
    for (int i=0; i<3; i++) {
      g_llimits[i] -= g_custom_threshold;
      g_ulimits[i] += g_custom_threshold;
  }
 // _____________________________________________________________

 Guard::status = GuardStatus::ARMED; // Need to add pairing behavior along with arming control.
}

void loop() {

  digitalWrite(Guard::TX_PIN, LOW);

  // Beginning heartbeat to let the sentinel know that guard is active
  heartbeat_timer.begin();
  transmit_heartbeat(Guard::TX_PIN, radio);

  unsigned long heartbeatStart = millis();

  while (millis() - heartbeatStart < Guard::HEARTBEAT_INT) {

    // Checking for movement in between heartbeats
    if (lsm6ds3.getAcceleratorAxes(g) == Guard::IMU_SUCCESS) {
      if (g[0] < g_llimits[0] || g[0] > g_ulimits[0]) {
        Guard::status = GuardStatus::ALARM;
        break;
      };
      if (g[1] < g_llimits[1] || g[1] > g_ulimits[1]) {
        Guard::status = GuardStatus::ALARM;
        break;
      };
      if (g[2] < g_llimits[2] || g[2] > g_ulimits[2]) {
        Guard::status = GuardStatus::ALARM;
        break;
      };
    };
  };

  if (Guard::status == GuardStatus::ALARM) {
    alarm_timer.begin();
    transmit_alarm_on(Guard::TX_PIN, radio, Guard::status);
    unsigned long alarmStart = millis();

    while (millis() - alarmStart < Guard::ALARM_DURATION) {

      // Resetting the timer if the guard is continually moved so the alarm stops only once the guard has been left alone for more than the alarm length
      if (lsm6ds3.getAcceleratorAxes(g) == Guard::IMU_SUCCESS) {
        if (g[0] < g_llimits[0] || g[0] > g_ulimits[0]) {
          transmit_alarm_on(Guard::TX_PIN, radio, Guard::status);
          alarmStart = millis();
        }
        if (g[1] < g_llimits[1] || g[1] > g_ulimits[1]) {
          transmit_alarm_on(Guard::TX_PIN, radio, Guard::status);
          alarmStart = millis();
        }
        if (g[2] < g_llimits[2] || g[2] > g_ulimits[2]) {
          transmit_alarm_on(Guard::TX_PIN, radio, Guard::status);
          alarmStart = millis();
        }
      }
    }
    transmit_alarm_off(Guard::TX_PIN, radio, Guard::status);
  }
}
