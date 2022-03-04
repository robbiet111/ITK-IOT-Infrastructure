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