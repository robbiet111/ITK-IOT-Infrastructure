/*
 * Author: Jakub Jelinek
 * Project: ITk IoT Infrastructure
*/

#include <SimpleCLI.h> // for instructions of use see https://github.com/spacehuhn/SimpleCLI
#include <stdio.h>
#include <EEPROM.h>
#include "common_headers.h"

// Number of bytes needed in ROM, max should be 512
/**
 * EEPROM_SIZE =
 *    influxdb_url = "http://xxx.xxx.xxx.xxx:xxxx" -> 27 characters
 *    WIFI_SSID max 32 character long string
 *    WIFI_PASS
 *        - WEP - Maximum key length is 16 characters
 *        - WPA-PSK/WPA2-PSK - Maximum key length is 63 characters
 *        -> 16 characters allowed now
 *  Note: For each string, one byte is reserved for its length
 */
#define EEPROM_SIZE 80
/**
 * EEPROM contents
 * 0 - 28 influxdb_url
 * 29 - 61 wifi_ssid
 * 62-78 wifi_pass
 */
#define INFLUXDB_URL_ADDR 0
#define WIFI_SSID_ADDR 29
#define WIFI_PASS_ADDR 62

SimpleCLI cli;
Command dbip;
Command ssid;
Command pass;
Command info;
Command run;
Command otau;

// Variables stored in ROM
String influxdb_url; // Database IP address
String wifi_ssid;
String wifi_pass;

// Setting up Adafruit MAX31865 module 
#include <Adafruit_MAX31865.h>
// Use software SPI: Adafruit_MAX31865(CS, DI, DO, CLK)
// NodeMCU pins connected to: Adafruit_MAX31865(D0=GPIO16, D1=GPIO5, D2=GPIO4, D3=GPIO0)
Adafruit_MAX31865 max_ada = Adafruit_MAX31865(33, 25, 26, 27);
// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF 430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL 100.0

#define ESP32_HOST_NAME       "UNIT_11"

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

enum running_mode {measure=0, enter_cli=1, update=2};
running_mode current_mode = measure;
bool refreshScreen = true;

WiFiMulti WiFiMulti;
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
char buff[512];
int vref = 1100;
int btnClick = true;
int db_counter = 0;
byte rcvData;


/**
 * 
 * CLI Part
 * 
 */
void writeStringToEEPROM(int address, const String &data);
String readStringFromEEPROM(int addrOffset);
void sensor_setup();
void ota_setup();

void writeStringToEEPROM(int address, const String &data)
{
  byte len = data.length();
  EEPROM.write(address, len);

  for (int i = 0; i < len; i++)
  {
    EEPROM.write(address + 1 + i, data[i]);
  }
  EEPROM.commit();
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}

// Callback function for dbip command
void dbipCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();
    const char * value = argVal.c_str();

    // Validate Value
    int l1,l2,l3,l4, port;
    if (sscanf(value, "http://%d.%d.%d.%d:%d", &l1, &l2, &l3, &l4, &port) == 5) {
      influxdb_url = argVal;
      writeStringToEEPROM(INFLUXDB_URL_ADDR, influxdb_url);
      // Print new ip
      Serial.print("> New influx DB url: "+influxdb_url+"\n");
    } else {
      Serial.print("Error: invalid ip address \"" + argVal+"\"\n");
    }
}

void ssidCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();

    wifi_ssid = argVal;
    writeStringToEEPROM(WIFI_SSID_ADDR, wifi_ssid);
    // Print new ssid
    Serial.print("> New wifi ssid: "+wifi_ssid+"\n");
}

void passCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();

    wifi_pass = argVal;
    writeStringToEEPROM(WIFI_PASS_ADDR, wifi_pass);
    // Print new ssid
    Serial.print("> New wifi password: "+wifi_pass+"\n");
}

void infoCallback(cmd* c) {
    Command cmd(c); // Create wrapper object
    Serial.println("\nSystem information:");
    Serial.print("\tConnected to wifi with ssid: "+wifi_ssid+"\n");
    Serial.printf("\tUsed password of length %d\n",wifi_pass.length());
    Serial.print("\tUsed password: "+wifi_pass+"\n");
    Serial.print("\tDatabase url: "+influxdb_url+"\n");
}

void runCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    current_mode = measure;
    sensor_setup();
    btnClick = true;
}

void otauCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    current_mode = update;
    ota_setup();
    btnClick = true;
}

// Callback in case of an error
void errorCallback(cmd_error* e) {
    CommandError cmdError(e); // Create wrapper object

    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    if (cmdError.hasCommand()) {
        Serial.print("Did you mean \"");
        Serial.print(cmdError.getCommand().toString());
        Serial.println("\"?");
    }
}

void checkCLI() {
  if (refreshScreen) {
    Serial.println("Entering CLI");
    tft.setTextSize(2);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("> CLI", tft.width() / 2, tft.height() / 2);
    refreshScreen = false;
  }
  // Check if user typed something into the serial monitor
  if (Serial.available()) {
      // Read out string from the serial monitor
      String input = Serial.readStringUntil('\n');

      // Parse the user input into the CLI
      if (input.length() > 1) {
        cli.parse(input);
      }
  }
  
  if (cli.errored()) {
      CommandError cmdError = cli.getError();
      Serial.print("ERROR: ");
      Serial.println(cmdError.toString());
      if (cmdError.hasCommand()) {
          Serial.print("Did you mean \"");
          Serial.print(cmdError.getCommand().toString());
          Serial.println("\"?");
      }
  }
}

/**
 * 
 * End of CLI Part
 * 
 */


/**
 * 
 * OTA Part
 * 
 */
const char* host = "esp32";
WebServer server(80);

void ota_setup() {
// Wait for connection
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    Serial.print("Uploading firmware...\n");
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        Serial.print("Successfully uploaded!");
      } else {
        Update.printError(Serial);
        Serial.print("Failed uploading.");
      }
    }
  });
  Serial.println("Starting server");
  server.begin();

}
/**
 *  
 * End of OTA Part
 * 
 */

/**
 * 
 * Experiment Part
 * 
 */
void MeasureDataMAX31865PT100() {
	static uint64_t timeStamp = 0;
	if (millis() - timeStamp > 1000) { // measure data every 1 seconds
		timeStamp = millis();

    uint16_t rtd = max_ada.readRTD();
    float ratio = rtd;
    ratio /= 32768;
    float myTemp = (RREF*ratio / 1250);
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

			tft.setTextSize(2);
			tft.fillScreen(TFT_BLACK);
			tft.setTextDatum(MC_DATUM);
			tft.drawString(String("T: ") + String(temperature) + " *C", tft.width() / 2, tft.height() / 2);

		}		
	}	  
}

/**
 * 
 * End of Experiment Part
 * 
 */

/**
 * 
 * Setup Part
 * 
 */
void button_init() {
  btn1.setClickHandler([](Button2 & b) {
    if (current_mode ==  measure) {
      current_mode = update;
      ota_setup();
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
      ota_setup();
    }
    btnClick = true;
  });
}

void sensor_setup() {
  Serial.println(wifi_ssid+" "+wifi_pass);
  WiFiMulti.addAP(wifi_ssid.c_str(), wifi_pass.c_str());
  delay(200);
  Serial.println("");
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifi_ssid);

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Connecting to WiFi", tft.width() / 2, tft.height() / 2 - 16);
  tft.drawString(String(wifi_ssid), tft.width() / 2, tft.height() / 2 + 16);

  int counter = 0;

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    counter += 1;
    if (counter == 5) {
      Serial.println("Entering CLI");
      current_mode = enter_cli;
      tft.fillScreen(TFT_BLACK);
      tft.setTextSize(2);
      tft.setTextDatum(MC_DATUM);
      tft.drawString("Connection failed", tft.width() / 2, tft.height() / 2-16);
      tft.drawString("Entering CLI", tft.width() / 2, tft.height() / 2+16);
      delay(2500);
      break;
    }
  }
  if (current_mode == measure) {
    WiFi.setHostname(ESP32_HOST_NAME);
    Serial.print("WiFi connected: "); Serial.println(WiFi.SSID());
    Serial.print("Hostname: ");       Serial.println(WiFi.getHostname());
    Serial.print("IP address: ");     Serial.println(WiFi.localIP()); char buf[16]; sprintf(buf, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
    Serial.print("MAC address: ");    Serial.println(WiFi.macAddress());

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("WiFi connected: " + String(WiFi.SSID()), tft.width() / 2, tft.height() / 2 - 24);
    tft.drawString("Hostname: " + String(WiFi.getHostname()), tft.width() / 2, tft.height() / 2 - 8);
    tft.drawString("IP address: " + String(buf), tft.width() / 2, tft.height() / 2 + 8);
    tft.drawString("MAC address: " + String(WiFi.macAddress()), tft.width() / 2, tft.height() / 2 + 24);
    delay(5000);
    tft.fillScreen(TFT_BLACK);
  }
}

void setup() {

  Serial.begin(115200);

  // Initialise EEPROM and read from it
  EEPROM.begin(EEPROM_SIZE);
  influxdb_url = readStringFromEEPROM(INFLUXDB_URL_ADDR);
  wifi_ssid = readStringFromEEPROM(WIFI_SSID_ADDR);
  wifi_pass = readStringFromEEPROM(WIFI_PASS_ADDR);

  // Simple CLI
  cli.setOnError(errorCallback); // Set error Callback

  // Create the command with callback function which updates influxdb_url
  dbip = cli.addSingleArgCmd("dbip", dbipCallback);
  ssid = cli.addSingleArgCmd("ssid", ssidCallback);
  pass = cli.addSingleArgCmd("pass", passCallback);
  info = cli.addCmd("info", infoCallback);
  run = cli.addCmd("run", runCallback);
  otau = cli.addCmd("otau", otauCallback);
  
  // Sensor setup
  max_ada.begin(MAX31865_2WIRE);  // set to 2WIRE or 4WIRE as necessary 

  rcvData = 255;

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);

  if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }

  button_init();
  
  tft.fillScreen(TFT_BLACK);

  sensor_setup();
}

void loop() {
  if (btnClick) {
    if (current_mode == enter_cli) {
      checkCLI();
    } else if (current_mode == measure) {
      MeasureDataMAX31865PT100();
    } else if (current_mode == update) {
      server.handleClient();
    } else {
      Serial.println("ELSE");
    }
  }
  btn1.loop();
  btn2.loop();
}
