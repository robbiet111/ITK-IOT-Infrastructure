/*
 * Authors: Dima Maneuski, Jakub Jelinek
 * Project: ITk IoT Infrastructure
*/

#include <SimpleCLI.h> // for instructions of use see https://github.com/spacehuhn/SimpleCLI
#include <stdio.h>
#include <EEPROM.h>
#include "common_namespace.h"
#include "cli_namespace.h"
// #include "ota_namespace.h"
#include "elegant_ota_namespace.h"
#include "experiment_namespace.h"
using namespace common;

// Setting up Adafruit MAX31865 module 
// #include <Adafruit_MAX31865.h>
// Use software SPI: Adafruit_MAX31865(CS, DI, DO, CLK)
// NodeMCU pins connected to: Adafruit_MAX31865(D0=GPIO16, D1=GPIO5, D2=GPIO4, D3=GPIO0)
// Adafruit_MAX31865 max_ada = Adafruit_MAX31865(33, 25, 26, 27);
// // The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
// #define RREF 430.0
// // The 'nominal' 0-degrees-C resistance of the sensor
// // 100.0 for PT100, 1000.0 for PT1000

// #define RNOMINAL 100.0

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

  // Initialise EEPROM and read from it
  influxdb_url = CLI::readStringFromEEPROM(INFLUXDB_URL_ADDR);
  wifi_ssid = CLI::readStringFromEEPROM(WIFI_SSID_ADDR);
  wifi_pass = CLI::readStringFromEEPROM(WIFI_PASS_ADDR);

  // Simple CLI
  CLI::cli.setOnError(CLI::errorCallback); // Set error Callback

  // Create the command with callback function which updates influxdb_url
  CLI::dbip = CLI::cli.addSingleArgCmd("dbip", CLI::dbipCallback);
  CLI::ssid = CLI::cli.addSingleArgCmd("ssid", CLI::ssidCallback);
  CLI::pass = CLI::cli.addSingleArgCmd("pass", CLI::passCallback);
  CLI::info = CLI::cli.addCmd("info", CLI::infoCallback);
  CLI::run = CLI::cli.addCmd("run", CLI::runCallback);
  CLI::otau = CLI::cli.addCmd("otau", CLI::otauCallback);
  
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

  if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
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
