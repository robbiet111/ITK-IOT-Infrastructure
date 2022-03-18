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

#include <SimpleCLI.h> // for instructions of use see https://github.com/spacehuhn/SimpleCLI
#include <stdio.h>
#include <EEPROM.h>
#include "common_namespace.h"
#include "cli_namespace.h"
#include "elegant_ota_namespace.h"
#include "experiment_namespace.h"
using namespace common;


#define TAG_UNIT_ID_NAME      "unit_id"
#define TAG_UNIT_ID_VALUE     "11"

#define TAG_INSTRUMENT_NAME   "instrument"
#define TAG_INSTRUMENT_VALUE  "ttgo_esp32"

#define TAG_LOCATION_NAME     "location"
#define TAG_LOCATION_VALUE    "undefined_location"

#define TAG_SENSOR_NAME       "sensor"
#define TAG_SENSOR_VALUE      "MAX31865_PT100"

#define TAG_CHANNEL_NAME      "channel"
#define TAG_CHANNEL_VALUE     "0"

#define FIELD_NAME_T          "temperature"


Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
char buff[512];
int vref = 1100;
int db_counter = 0;
byte rcvData;

/**
 * 
 * Setup Part
 * 
 */


void button_init() {
  btn1.setClickHandler([](Button2 & b) {
    if (current_mode ==  measure) {
      current_mode = update;
      refreshScreen = true;
      elegant_OTA::ota_setup();
    } else if (current_mode == enter_cli){
      current_mode = measure;
      sensor_setup();
    } else if (current_mode == update) {
      current_mode = enter_cli;
      refreshScreen = true;
    }
    btnClick = true;
  });

  btn2.setClickHandler([](Button2 & b) {
    if (current_mode ==  update) {
      current_mode = measure;
      sensor_setup();
    } else if (current_mode == measure){
      current_mode = enter_cli;
      refreshScreen = true;
    } else if (current_mode == enter_cli) {
      current_mode = update;
      refreshScreen = true;
      elegant_OTA::ota_setup();
    }
    btnClick = true;
  });
}



void setup() {

  Serial.begin(115200);


  EEPROM.begin(EEPROM_SIZE);

  CLI::begin();

  // Sensor setup
  experiment::begin();

  rcvData = 255;

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  
  tft.setSwapBytes(true);

  if (TFT_BL > 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
  }

  tft.setRotation(0);
  tft.fillScreen(TFT_RED);
  delay(2000);
  tft.fillScreen(TFT_BLUE);
  delay(2000);
  tft.fillScreen(TFT_GREEN);
  delay(2000);

  tft.setRotation(1);

  button_init();
  
  tft.fillScreen(TFT_BLACK);

  sensor_setup();
}

void loop() {
  if (btnClick) {
    if (current_mode == enter_cli) {
      CLI::checkCLI();
    } else if (current_mode == measure) {
      experiment::measure();
    } else if (current_mode == update) {
      elegant_OTA::ws.cleanupClients();
    }
  }
  btn1.loop();
  btn2.loop();
}
