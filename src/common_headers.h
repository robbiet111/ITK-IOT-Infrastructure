/***
Author: Dima Maneuski
Contributors: None
Project: Common headers for TTOGO ESP32 GLADD IoT 
***/

// Wifi
#include <WiFiMulti.h>
// InfluxDB2.0
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
// Periphery
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <Button2.h>

//#include <WiFi.h>

// Logo
#include "UoG_DM_240x135.h"
#include "team_logo.h"
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
#define TFT_BACKLIGHT_ON HIGH

// Wifi and database definitions
// Dima's house WiFi
// #define WIFI_SSID 				"Dima's lair"
// #define WIFI_PASS 				"frBdshqz4nsd"
// #define INFLUXDB_URL 			"http://192.168.0.52:8086" 						
// #define INFLUXDB_TOKEN        	"jYtyIjiGsORacfLrC3uivo3WDxeiTJLZvF1CSC2sM1EM_t60xqtJtUaifV61aF_SfNwqSLgGiWEhqc3kUgakhA==" // Home token
// #define INFLUXDB_ORG          	"Home"                           			// options: PPE, Home
// #define INFLUXDB_BUCKET       	"HomeEnvironment"                         	// options: GLADD, HomeEnvironment
// #define TZ_INFO               	"CET-1CEST,M3.5.0,M10.5.0/3"

// ######### OLD IDIOT SERVER ##########
// #define INFLUXDB_URL 			"http://10.130.100.22:8086"					// IoT WiFi
// //#define INFLUXDB_URL 			"http://194.36.1.55:8086"					// PPE WiFi
// #define WIFI_SSID 				"PPE_IoT"
// #define WIFI_PASS 				"JffMphpofe7o"
// #define INFLUXDB_TOKEN        	"gnLr5gMb_eEQsZ7corAeW8QRGKoGNhXRhA-uda2Svl4hznAl8zkSU5QxAFqEjzgiKAAwJcF5oi-EFZmAUaSU-A==" // PPE token
// #define INFLUXDB_ORG          	"PPE"                           			// options: PPE, Home
// #define INFLUXDB_BUCKET       	"GLADD"                         			// options: GLADD, HomeEnvironment
// #define TZ_INFO               	"CET-1CEST,M3.5.0,M10.5.0/3"

// ######### NEW IDIOT SERVER ##########
#define INFLUXDB_URL 			"http://10.130.100.23:8086"					// IoT WiFi
//#define INFLUXDB_URL 			"http://194.36.1.20:8086"					// PPE WiFi
#define WIFI_SSID 				"PPE_IoT"
#define WIFI_PASS 				"JffMphpofe7o"
#define INFLUXDB_TOKEN        	"6PdYm2oqhOxaA1yRGhpFIypIaNX_etiNNL300Io1NN-OJLD8Apjw1TMQvfq2D3R_KQ5vr-9NB8IVKwuzIrWhlA==" // PPE token
#define INFLUXDB_ORG          	"PPE"                           			// options: PPE, Home
#define INFLUXDB_BUCKET       	"GLADD"                         			// options: GLADD, HomeEnvironment
#define TZ_INFO               	"CET-1CEST,M3.5.0,M10.5.0/3"
