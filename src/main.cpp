/*
 * Author: Jakub Jelinek
 * Project: ITk IoT Infrastructure
*/

#include <SimpleCLI.h> // for instructions of use see https://github.com/spacehuhn/SimpleCLI
#include <stdio.h>
#include <EEPROM.h>

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

// Variables
String influxdb_url; // Database IP address
String wifi_ssid;
String wifi_pass;


void writeStringToEEPROM(int address, const String &data);
String readStringFromEEPROM(int addrOffset);

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
  
  // put your setup code here, to run once:

}

void loop() {
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
