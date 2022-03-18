/***
  Authors:
    - Dima Maneuski
    - Jakub Jelinek - 2478625J@student.gla.ac.uk / jakubjjelinek@gmail.com
  Project: ITk IoT Infrastructure - Aims to develop a platform supporting a CLI and OTA updates on the TTGO Esp32 board.

  Change log:
  Version: 1.0.0 [18/03/2022]
  First full release.

  This code is licensed under MIT license (see LICENSE for details)
***/

#include "experiment_namespace.h"
#include "common_namespace.h"
using namespace common;

namespace experiment {
    Adafruit_MAX31865 max_ada = Adafruit_MAX31865(33, 25, 26, 27);

    void measure() {
        static uint64_t timeStamp = 0;
        if (millis() - timeStamp > 1000) { // measure data every 1 seconds
            timeStamp = millis();

        uint16_t rtd = max_ada.readRTD();
        float ratio = rtd;
        ratio /= 32768;
        float temperature = 0.0;
        temperature = max_ada.temperature(RNOMINAL, RREF);
        
        // Check and print any faults
        uint8_t fault = max_ada.readFault();
        if (fault) {
        Serial.print("Fault 0x"); Serial.println(fault, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH) {
            Serial.println("RTD High Threshold"); 
        }
        if (fault & MAX31865_FAULT_LOWTHRESH) {
            Serial.println("RTD Low Threshold"); 
        }
        if (fault & MAX31865_FAULT_REFINLOW) {
            Serial.println("REFIN- > 0.85 x Bias"); 
        }
        if (fault & MAX31865_FAULT_REFINHIGH) {
            Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
        }
        if (fault & MAX31865_FAULT_RTDINLOW) {
            Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
        }
        if (fault & MAX31865_FAULT_OVUV) {
            Serial.println("Under/Over voltage"); 
        }
        max_ada.clearFault();
        }
            else {
                Serial.print("data: "); Serial.print(temperature); Serial.println(" °C");

                clear_display(2);
                put_on_display(String("T: ") + String(temperature) + " *C", 0);

            }		
        }	  
    }
}