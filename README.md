# Components of the Portal Security System \- MK1

The Portal Security System is based on radio communication between a guard and a sentinel. The guard is set at a monitoring location. Upon the arming of the system any movement in the guard triggers the transmission of an authenticated and encrypted packet that is received by the sentinel, which in turn triggers the alarm. Specific features that make this system secure and robust are touched on below:

1. Authentication
    1. Infiltrator cannot fashion his own device to speak to the Sentinel.
    2. Device UUIDs, which are securely kept on the microcontroller, are sent to the Sentinel and matched against a pre-existing set of authenticated UUIDs.
2. Encryption
    1. Radio packets, if intercepted by an infiltrator, will read gibberish without the secret key that is saved on the microcontroller.
    2. No exposure of the knowledge that a security system is communicating.
3. Heartbeat
    1. The Guard constantly sends a heartbeat signal to the Sentinel. If the Sentinel doesn’t see the heartbeat, it triggers the alarm.
    2. This mechanism defends against unforeseen changes to the Guard.

##  Component basics

| **Component** | **Model** |
| --- | --- |
| Portal Guard (PG) | Nano 33 IoT, Nano 33 BLE Sense |
| Portal Sentinel (PS) | Nano 33 IoT, Nano 33 BLE Sense |
| Radio RX | STX882 Superheterodyne ASK Receiver |
| Radio TX | STX882 ASK Transmitter |
| MOSFET | IRLZ44NPBF |
| Alarm | ES-626 Siren |
| Alarm power supply | - |
| Sentinel power supple | - |
| Guard battery holder | - |

### Radio Comms


|  |  |
| --- | --- |
| Title | STX882, SRX882 ASK Transmitter & Receiver |
| **Description** | Transmitter and Receiver package |
| **Key properties** | 1. Low power |
| **Link** | https://quartzcomponents.com/products/433mhz-100-meters-stx882-ask-transmitter-module-srx882-superheterodyne-receiver-module-antenna |

|  |  |
| --- | --- |
| Title | #### STX882 ASK Transmitter |
| **Description** | Just the transmitter |
| **Key properties** | 1. Low power |
| **Link** | <https://www.nicerf.com/ask-modules/433mhz-ask-transmitter-module-stx882.html> |

### Microcontrollers

|  |  |
| --- | --- |
| Title | Nano 33 IoT |
| **Description** | SKU - ABX00027 |
| **Subcomponents** | IMU: [LSM6DS3TR-C](https://www.st.com/en/mems-and-sensors/lsm6ds3tr-c.html#documentation) MCU: SAMD21 Cortex®-M0+ 32bit low power ARM MCUWiFi + Bluetooth: Nina W102 uBloxProcessor: SAMD21G18A 48MHz |
| **Key properties** | 1. Ultra-low power IMU 2. Bluetooth + WiFi |
| **Link** | https://docs.arduino.cc/hardware/nano-33-iot/#tech-specs |

|  |  |
| --- | --- |
| Title | Nano 33 BLE Sense |
| **Description** | SKU - ABX00031 |
| **Subcomponents** | IMU: [LSM9DS1](https://www.st.com/en/mems-and-sensors/lsm9ds1.html) MCU: nRF52840Bluetooth: NINA-B306Processor: nRF52840 64MHz |
| **Key properties** | 1. Low power IMU (9D) 2. Only Bluetooth 3. Large sensor suite (mic, gesture, light, proximity, barometric pressure, temperature, humidity) |
| **Link** | https://docs.arduino.cc/hardware/nano-33-ble-sense/#tech-specs |

### Sirens

|  |  |
| --- | --- |
| Title | ES-626 Siren |
| **Description** | 12V, 110dBStandby current - 320 mAAlarm current - 700 mARated power: 15W/20W |
| **Key to anatomy diagram** | 1. ﻿Power supply - [https://www.electropi.in/12v-1amp-dc-adaptor?gad\_source=1&gad\_campaignid=23329895947&gclid=Cj0KCQjw9ZLSBhCcARIsAEhGKgMEJgfIoyDQ9II2g4OOKrflaxcw3\_q1YD55OqRRRPN8MJAeuSVvnkcaAq\_rEALw\_wcB](https://www.electropi.in/12v-1amp-dc-adaptor?gad_source=1&gad_campaignid=23329895947&gclid=Cj0KCQjw9ZLSBhCcARIsAEhGKgMEJgfIoyDQ9II2g4OOKrflaxcw3_q1YD55OqRRRPN8MJAeuSVvnkcaAq_rEALw_wcB)  2. MOFSET - <https://www.digikey.in/en/products/detail/infineon-technologies/IRLZ44NPBF/811808> |
| **Link** | https://electronicspices.com/product/es-626-12v-110db-horn-alarm-siren-electric-wired-1-tone-alarm-speaker-system-indooroutdoor-loudspeaker-horn?srsltid=AfmBOorKkSTjkFBDyQI-htDgDZ1GaqmCQVHYW-zW0hwE1nWPR-1Fm54fGWQ |

|  |  |
| --- | --- |
| Title | Mini Siren |
| **Description** | 12V, 120dBCurrent - 320 mA |
| **Link** | https://robu.in/product/dc-12v-wired-mini-horn-siren-home-security-sound-alarm-system-120db-anti-theft-speaker-buzzer |

### Power & Electronics

|  |  |
| --- | --- |
| Title | #### MOSFET |
| **Description** | IRLZ44NPBF |
| **Key properties** | 1. Commonly used 2. Can easily handle 12 V, 1A |
| **Link** | <https://www.digikey.in/en/products/detail/infineon-technologies/IRLZ44NPBF/811808> |

|  |  |
| --- | --- |
| Title | #### Power supply (siren) |
| **Description** |  |
| **Key properties** | 1. 12 V 2. 1A |
| **Link** | [https://www.electropi.in/12v-1amp-dc-adaptor?gad\_source=1&gad\_campaignid=23329895947&gclid=Cj0KCQjw9ZLSBhCcARIsAEhGKgMEJgfIoyDQ9II2g4OOKrflaxcw3\_q1YD55OqRRRPN8MJAeuSVvnkcaAq\_rEALw\_wcB](https://www.electropi.in/12v-1amp-dc-adaptor?gad_source=1&gad_campaignid=23329895947&gclid=Cj0KCQjw9ZLSBhCcARIsAEhGKgMEJgfIoyDQ9II2g4OOKrflaxcw3_q1YD55OqRRRPN8MJAeuSVvnkcaAq_rEALw_wcB) |

Arduino IDE Libraries:

These are the libraries that need to be installed before compiling the code:
1. RH_ASK.h – RadioHead by Mike McCauley
2. LSM6DS3-SOLDERED.h – Soldered LSM6DS3 Arduino library
3. RTCZero.h – RTCZero by Arduino
