/*
  Rui Santos
  Complete project details
   - Arduino IDE: https://RandomNerdTutorials.com/esp32-ota-over-the-air-arduino/
   - VS Code: https://RandomNerdTutorials.com/esp32-ota-over-the-air-vs-code/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#ifndef ELEGANT_OTA_NAMESPACE_H
#define ELEGANT_OTA_NAMESPACE_H

#include "common_namespace.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
using namespace common;

// Set number of outputs
#define NUM_OUTPUTS  4

namespace elegant_OTA {
    // Create AsyncWebServer object on port 80
    extern AsyncWebServer server;

    // Create a WebSocket object
    extern AsyncWebSocket ws;


    // Assign each GPIO to an output
    extern int outputGPIOs[NUM_OUTPUTS];

    inline void notifyClients(String state) {
        ws.textAll(state);
    }
    // Initialize SPIFFS
    void initSPIFFS();

    // Initialize WiFi
    inline void initWiFi() {
        Serial.print("Connecting to WiFi ..");
        while (common::wifimulti.run() != WL_CONNECTED) {
            Serial.print('.');
            delay(1000);
        }
        Serial.print(" Connected to ");
        Serial.println(wifi_ssid);
        Serial.println("You can update the firmware at:\n(Use Ctrl+click to open the url in a browser.)");
        Serial.print("http://");
        Serial.print(WiFi.localIP());
        Serial.println("/update");
    }

    String getOutputStates();

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len);

    inline void initWebSocket() {
        ws.onEvent(onEvent);
        server.addHandler(&ws);
    }

    void ota_setup();
}
#endif