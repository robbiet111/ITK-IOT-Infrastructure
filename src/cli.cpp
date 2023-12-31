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

#include "cli_namespace.h"
#include "elegant_ota_namespace.h"
#include "common_namespace.h"

namespace CLI {
    SimpleCLI cli;
    Command dbip;
    Command ssid;
    Command pass;
    Command info;
    Command run;
    Command otau;
    void otauCallback(cmd* c) {
        Command cmd(c); // Create wrapper object

        current_mode = update;
        elegant_OTA::ota_setup();
        btnClick = true;
    }

    void begin() {
        // Initialise EEPROM and read from it
        common::influxdb_url = readStringFromEEPROM(INFLUXDB_URL_ADDR);
        common::wifi_ssid = readStringFromEEPROM(WIFI_SSID_ADDR);
        common::wifi_pass = readStringFromEEPROM(WIFI_PASS_ADDR);

        // Simple CLI
        cli.setOnError(errorCallback); // Set error Callback

        // Create the commands with callback functions
        dbip = cli.addSingleArgCmd("dbip", dbipCallback);
        ssid = cli.addSingleArgCmd("ssid", ssidCallback);
        pass = cli.addSingleArgCmd("pass", passCallback);
        info = cli.addCmd("info", infoCallback);
        run = cli.addCmd("run", runCallback);
        otau = cli.addCmd("otau", otauCallback);
    }

    void checkCLI() {
        if (refreshScreen) {
            Serial.println("Entering CLI");
            clear_display(2);
            put_on_display("> CLI", 0);
            refreshScreen = false;
        }
        // Check if user typed something into the serial monitor
        if (Serial.available()) {
            // Read out string from the serial monitor
            String input = Serial.readStringUntil('\n');

            // Parse the user input into the CLI
            if (input.length() > 1) {
                CLI::cli.parse(input);
            }
        }
        
        if (CLI::cli.errored()) {
            CommandError cmdError = CLI::cli.getError();
            Serial.print("ERROR: ");
            Serial.println(cmdError.toString());
            if (cmdError.hasCommand()) {
                Serial.print("Did you mean \"");
                Serial.print(cmdError.getCommand().toString());
                Serial.println("\"?");
            }
        }
    }
}