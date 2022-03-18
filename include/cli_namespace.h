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

#ifndef CLI_NAMESPACE_H
#define CLI_NAMESPACE_H
#include <stdio.h>
#include <SimpleCLI.h>
#include <EEPROM.h>

#include "common_namespace.h"
using namespace common;

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

namespace CLI {
    extern SimpleCLI cli;
    extern Command dbip;
    extern Command ssid;
    extern Command pass;
    extern Command info;
    extern Command run;
    extern Command otau;
    


    inline void writeStringToEEPROM(int address, const String &data) {
        byte len = data.length();
        EEPROM.write(address, len);

        for (int i = 0; i < len; i++)
        {
            EEPROM.write(address + 1 + i, data[i]);
        }
        EEPROM.commit();
    }

    inline String readStringFromEEPROM(int addrOffset) {
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
    inline void dbipCallback(cmd* c) {
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

    inline void ssidCallback(cmd* c) {
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

    inline void passCallback(cmd* c) {
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

    inline void infoCallback(cmd* c) {
        Command cmd(c); // Create wrapper object
        Serial.println("\nSystem information:");
        Serial.print("\tConnected to wifi with ssid: "+wifi_ssid+"\n");
        Serial.printf("\tUsed password of length %d\n",wifi_pass.length());
        Serial.print("\tUsed password: "+wifi_pass+"\n");
        Serial.print("\tDatabase url: "+influxdb_url+"\n");
    }

    inline void runCallback(cmd* c) {
        Command cmd(c); // Create wrapper object

        current_mode = measure;
        Serial.println();
        sensor_setup();
        btnClick = true;
    }

    void otauCallback(cmd* c);

    // Callback in case of an error
    inline void errorCallback(cmd_error* e) {
        CommandError cmdError(e); // Create wrapper object

        Serial.print("ERROR: ");
        Serial.println(cmdError.toString());

        if (cmdError.hasCommand()) {
            Serial.print("Did you mean \"");
            Serial.print(cmdError.getCommand().toString());
            Serial.println("\"?");
        }
    }

    void begin();

    void checkCLI();
}
#endif