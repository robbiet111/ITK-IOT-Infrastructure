/*
 * Author: Jakub Jelinek
 * Project: ITk IoT Infrasctrucutre
*/

#include <SimpleCLI.h> // for instructions of use see https://github.com/spacehuhn/SimpleCLI
#include <stdio.h>

SimpleCLI cli;
Command dbip;

String database_ip = "10.198.46.12"; // Random local ip address

// Callback function for dbip command
void dbipCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();
    const char * value = argVal.c_str();

    // Validate Value
    int l1,l2,l3,l4;
    if (sscanf(value, "%d.%d.%d.%d", &l1, &l2, &l3, &l4) == 4) {
      database_ip = argVal;

      // Print new ip
      Serial.print("> "+database_ip+"\n");
    } else {
      Serial.print("Error: invalid ip address \"" + argVal+"\"\n");
    }
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

  cli.setOnError(errorCallback); // Set error Callback

  // Create the command with callback function which updates database_ip
  dbip = cli.addSingleArgCmd("dbip", dbipCallback);
  
  // put your setup code here, to run once:

}

void loop() {
  // Check if user typed something into the serial monitor
  if (Serial.available()) {
      Serial.print("Database ip is: " + database_ip+"\n");
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
