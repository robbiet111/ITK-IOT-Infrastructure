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

// OTA
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

/* Style */
String style =
"<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
"input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
"#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
"#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
"form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
".btn{background:#3498db;color:#fff;cursor:pointer}</style>";
/* Login page */
String loginIndex = 
"<form name=loginForm>"
"<h1>ESP32 Login</h1>"
"<input name=userid placeholder='User ID'> "
"<input name=pwd placeholder=Password type=Password> "
"<input type=submit onclick=check(this.form) class=btn value=Login></form>"
"<script>"
"function check(form) {"
"if(form.userid.value=='admin' && form.pwd.value=='admin')"
"{window.open('/serverIndex')}"
"else"
"{alert('Error Password or Username')}"
"}"
"</script>" + style;
 
/* Server Index Page */
String serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
"<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
"<label id='file-input' for='file'>   Choose file...</label>"
"<input type='submit' class=btn value='Update'>"
"<br><br>"
"<div id='prg'></div>"
"<br><div id='prgbar'><div id='bar'></div></div><br></form>"
"<script>"
"function sub(obj){"
"var fileName = obj.value.split('\\\\');"
"document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
"};"
"$('form').submit(function(e){"
"e.preventDefault();"
"var form = $('#upload_form')[0];"
"var data = new FormData(form);"
"$.ajax({"
"url: '/update',"
"type: 'POST',"
"data: data,"
"contentType: false,"
"processData:false,"
"xhr: function() {"
"var xhr = new window.XMLHttpRequest();"
"xhr.upload.addEventListener('progress', function(evt) {"
"if (evt.lengthComputable) {"
"var per = evt.loaded / evt.total;"
"$('#prg').html('progress: ' + Math.round(per*100) + '%');"
"$('#bar').css('width',Math.round(per*100) + '%');"
"}"
"}, false);"
"return xhr;"
"},"
"success:function(d, s) {"
"console.log('success!') "
"},"
"error: function (a, b, c) {"
"}"
"});"
"});"
"</script>" + style;