/***
Author: Dima Maneuski
Contributors: None
Project: Common headers for TTOGO ESP32 GLADD IoT 
***/

#include <Arduino.h>
// Wifi
#include <WiFiMulti.h>
// InfluxDB2.0
// #include <InfluxDbClient.h>
// #include <InfluxDbCloud.h>
// Periphery
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <Button2.h>

//#include <WiFi.h>

// Common definitions
#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif
#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif
#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23
#define TFT_BL              4   // Display backlight control pin
#define BUTTON_1            35
#define BUTTON_2            0

// OTA
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>