// Simple example of LVGL for WT32-SC01
// Downloaded from here:
// https://github.com/littleyoda/WT32-SC01-Example-lvgl-arduino-platformio-

/**
 * Main setup
 * 
 */
bool enterCLI = false;
bool shouldSetUp = true;

//LVGL
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <TFT_eSPI.h> 
#include "FT62XXTouchScreen.h"
// #include "bigger_logo.h"
TFT_eSPI tft = TFT_eSPI();
FT62XXTouchScreen touchScreen = FT62XXTouchScreen(TFT_WIDTH, PIN_SDA, PIN_SCL);
#include "lvgl.h"
#include "esp_freertos_hooks.h"
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *screenMain;
lv_obj_t *label;
lv_obj_t *status_label;
lv_obj_t *measurement_label;
lv_obj_t *info_label;
uint16_t lastx = 0;
uint16_t lasty = 0;

static void event_handler_btn(lv_obj_t * obj, lv_event_t event){
    if(event == LV_EVENT_CLICKED) {
        if (obj == btn1 && enterCLI) {
          enterCLI=false;
          // sensor_setup();
          shouldSetUp=true;
        } else if (obj == btn2){
          shouldSetUp = false;
          enterCLI=true;
          lv_label_set_text(status_label, "Status: CLI");
          lv_label_set_text(measurement_label, "");
        }
    }
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

bool my_input_read2(lv_indev_drv_t * drv, lv_indev_data_t*data) {
  TouchPoint touchPos = touchScreen.read();
  if (touchPos.touched) {
    Serial.println(String(touchPos.xPos) + ": " + String(touchPos.yPos));
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchPos.xPos;
    data->point.y = touchPos.yPos;
    lastx = touchPos.xPos;
    lasty = touchPos.yPos;
  } else {
    data->state = LV_INDEV_STATE_REL;
    data->point.x = lastx;
    data->point.y = lasty;
     
  }
  return false;
}
static void lv_tick_task(void)
{
 lv_tick_inc(portTICK_RATE_MS);
}

//CLI
#include <SimpleCLI.h> // for instructions of use see https://github.com/spacehuhn/SimpleCLI
#include <stdio.h>
#include <EEPROM.h>
// #include "common_headers.h"
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
bool refreshScreen = true;

SimpleCLI cli;
Command dbip;
Command ssid;
Command pass;
Command info;
// Variables stored in ROM
String influxdb_url; // Database IP address
String wifi_ssid;
String wifi_pass;
void writeStringToEEPROM(int address, const String &data);
String readStringFromEEPROM(int addrOffset);
void sensor_setup();

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
    lv_label_set_text(status_label, "Status: CLI");
    refreshScreen = false;
  }
  // Check if user typed something into the serial monitor
  if (Serial.available()) {
      //Serial.print("Database ip is: " + influxdb_url+"\n");
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
 * EXPERIMENT 
 * 
 */
// Wifi
//#include <WiFi.h>
#include <WiFiMulti.h>
// InfluxDB2.0
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
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

InfluxDBClient client;
Point sensor("data");
WiFiMulti WiFiMulti;
char buff[512];
int vref = 1100;
int btnClick = true;
int db_counter = 0;
byte rcvData;

void MeasureDataMAX31865PT100() {
	static uint64_t timeStamp = 0;
	if (millis() - timeStamp > 1000) { // measure data every 1 seconds
		timeStamp = millis();

    uint16_t rtd = max_ada.readRTD();
    //Serial.print("RTD value: "); Serial.println(rtd);
    float ratio = rtd;
    ratio /= 32768;
    //Serial.print("Ratio = "); Serial.println(ratio,8);
    //Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
    float myTemp = (RREF*ratio / 1250);
    //Serial.print("Temperature = ");
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
			Serial.print("Temperature: "); Serial.print(temperature); Serial.println("°C");
			sensor.clearFields();
			sensor.addField(FIELD_NAME_T, float(temperature));
			
      lv_label_set_text_fmt(measurement_label, "Temperature: %.2f°C",temperature);

			db_counter++;
			if (db_counter == 60) {
				if (!client.writePoint(sensor)) {
				  Serial.print("InfluxDB write failed: ");
				  Serial.println(client.getLastErrorMessage());
				  //Serial.println("Calling ESP hard reset...");
				  //ESP.restart();
				} else {
				  Serial.println("InfluxDB write OK");
				}
				db_counter = 0;
			}
		}		
	}	  
}

void sensor_setup() {
  if (shouldSetUp) {
    Serial.println("So it begins.");
    lv_label_set_text(status_label, "Status: Running Sensor Setup");
    // // InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
    Serial.println(wifi_ssid+" "+wifi_pass);
    WiFiMulti.addAP(wifi_ssid.c_str(), wifi_pass.c_str());
    delay(200);
    Serial.println("");
    Serial.print("Connecting to WiFi: ");
    Serial.println(wifi_ssid);
    lv_label_set_text_fmt(status_label, "Status: Connecting to WiFi\n%s",String(wifi_ssid));
    int counter = 0;

    while (WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
      counter += 1;
      if (counter == 5) {
        Serial.println("Entering CLI");
        enterCLI = true;
        lv_label_set_text(status_label, "Status: Connection failed");
        delay(2500);
        lv_label_set_text(status_label, "Status: Entering CLI");
        delay(2500);
        break;
      }
    }

   vTaskDelay(5000/portTICK_PERIOD_MS);
    if (!enterCLI) {
      WiFi.setHostname(ESP32_HOST_NAME);
      Serial.print("WiFi connected: "); Serial.println(WiFi.SSID());
      Serial.print("Hostname: ");       Serial.println(WiFi.getHostname());
      Serial.print("IP address: ");     Serial.println(WiFi.localIP()); char buf[16]; sprintf(buf, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
      Serial.print("MAC address: ");    Serial.println(WiFi.macAddress());

      lv_label_set_text_fmt(info_label, "WiFi connected: %s\nHostname: %s\nIP address: %s\nMAC address: %s\n",String(WiFi.SSID()),String(WiFi.getHostname()),String(buf),String(WiFi.macAddress()));
      delay(5000);
      lv_label_set_text(status_label, "Status: Running Measurement");
    }
    refreshScreen = true;
    shouldSetUp = false;
  }

}



void my_task_handler(void * parameters) {
  Serial.println("Setup was fine");
  for(;;) {
    if (enterCLI) {
      Serial.println("Check cli");
      checkCLI();
    } else {
      Serial.println("not entering cli");
      if (shouldSetUp) {
      Serial.println("setup");
        sensor_setup();
      } else {
        Serial.println("measure");
        MeasureDataMAX31865PT100();
        vTaskDelay(1000/portTICK_PERIOD_MS);
      }
    }
  }
}


/**
 * 
 * BODY
 * 
 */

void setup() {

  lv_disp_drv_t disp_drv;
  lv_indev_drv_t indev_drv;

  Serial.begin(115200);
  lv_init();
  // Setup tick hook for lv_tick_task
  esp_err_t err = esp_register_freertos_tick_hook((esp_freertos_tick_cb_t)lv_tick_task); 

  // Enable TFT
  tft.begin();
  tft.setRotation(1);

  // Enable Backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL,1);

  // Start TouchScreen
  touchScreen.begin();


  // Display Buffer
  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  // Init Display
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 480;
  disp_drv.ver_res = 320;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  // Init Touchscreen
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_input_read2;
  lv_indev_drv_register(&indev_drv);

  // Screen Object
  screenMain = lv_obj_create(NULL, NULL);

  // Text
  // label = lv_label_create(screenMain, NULL);
  // lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  // lv_label_set_text(label, "Press a button");
  // lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  // lv_obj_set_size(label, 240, 40);
  // lv_obj_set_pos(label, 0, 15);



  status_label = lv_label_create(screenMain, NULL);
  lv_label_set_long_mode(status_label, LV_LABEL_LONG_BREAK);
  lv_label_set_text(status_label, "Status: Running Setup");
  lv_label_set_align(status_label, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_size(status_label, 240, 40);
  lv_obj_set_pos(status_label, 0, 15);


  measurement_label = lv_label_create(screenMain, NULL);
  lv_label_set_long_mode(measurement_label, LV_LABEL_LONG_BREAK);
  lv_label_set_text(measurement_label, "This is a measurement");
  lv_label_set_align(measurement_label, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_size(measurement_label, 240, 40);
  lv_obj_set_pos(measurement_label, 0, 75);

  info_label = lv_label_create(screenMain, NULL);
  lv_label_set_long_mode(info_label, LV_LABEL_LONG_BREAK);
  lv_label_set_text(info_label, "");
  lv_label_set_align(info_label, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_size(info_label, 240, 40);
  lv_obj_set_pos(info_label, 240, 15);


  // Button 1
  btn1 = lv_btn_create(screenMain, NULL);
  lv_obj_set_event_cb(btn1, event_handler_btn);
  lv_obj_set_width(btn1, 70);
  lv_obj_set_height(btn1, 32);
  lv_obj_set_pos(btn1, 32, 100);
  lv_obj_t * label1 = lv_label_create(btn1, NULL);
  lv_label_set_text(label1, "Run");


  // Button 2
  btn2 = lv_btn_create(screenMain, NULL);
  lv_obj_set_event_cb(btn2, event_handler_btn);
  lv_obj_set_width(btn2, 70);
  lv_obj_set_height(btn2, 32);
  lv_obj_set_pos(btn2, 142, 100);
  lv_obj_t * label2 = lv_label_create(btn2, NULL);
  lv_label_set_text(label2, "CLI");

  // Screen load
  lv_scr_load(screenMain);


  // Initialise EEPROM and read from it
  EEPROM.begin(EEPROM_SIZE);
  influxdb_url = readStringFromEEPROM(INFLUXDB_URL_ADDR);
  wifi_ssid = readStringFromEEPROM(WIFI_SSID_ADDR);
  wifi_pass = readStringFromEEPROM(WIFI_PASS_ADDR);
  wifi_ssid = "Kuba";
  wifi_pass="jirikara";
  delay(3000);
  Serial.println(wifi_ssid+" "+wifi_pass);

  // Create the command with callback function which updates influxdb_url
  dbip = cli.addSingleArgCmd("dbip", dbipCallback);
  ssid = cli.addSingleArgCmd("ssid", ssidCallback);
  pass = cli.addSingleArgCmd("pass", passCallback);
  info = cli.addCmd("info", infoCallback);
  // run = cli.addCmd("run", runCallback);
  
  // Sensor setup
  max_ada.begin(MAX31865_2WIRE);  // set to 2WIRE or 4WIRE as necessary 

  rcvData = 255;

  xTaskCreatePinnedToCore(
    my_task_handler,
    "My attempt for a task",
    5000,
    NULL,
    1,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE
  );
  //Sensor Setup
  // sensor_setup();
}

void loop() {
  lv_task_handler();
  //   if (enterCLI) {
  //     checkCLI();
  //   } else {
  //     Serial.println("Measure");
  //     // MeasureDataMAX31865PT100();
  //   }
  delay(5);
}
