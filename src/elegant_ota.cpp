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

#include "elegant_ota_namespace.h"
#include "common_namespace.h"
#include <AsyncElegantOTA.h>
using namespace common;
namespace elegant_OTA {
    AsyncWebServer server(80);
    AsyncWebSocket ws("/ws");

    int outputGPIOs[NUM_OUTPUTS] = {2, 4, 12, 14};
    // Initialize SPIFFS
    void initSPIFFS() {
        if (!SPIFFS.begin(true)) {
            Serial.println("An error has occurred while mounting SPIFFS");
        }
        Serial.println("SPIFFS mounted successfully");
    }

    String getOutputStates(){
        JSONVar myArray;
        for (int i =0; i<NUM_OUTPUTS; i++){
            myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
            myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
        }
        String jsonString = JSON.stringify(myArray);
        return jsonString;
    }

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char*)data, "states") == 0) {
        notifyClients(getOutputStates());
        }
        else{
        int gpio = atoi((char*)data);
        digitalWrite(gpio, !digitalRead(gpio));
        notifyClients(getOutputStates());
        }
    }
    }

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
        switch (type) {
            case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
            case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
            case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
            break;
        }
    }

    void ota_setup() {
        // Set GPIOs as outputs
        for (int i =0; i<NUM_OUTPUTS; i++){
            pinMode(outputGPIOs[i], OUTPUT);
        }
        initSPIFFS();
        initWiFi();
        initWebSocket();

        // Route for root / web page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/index.html", "text/html",false);
        });

        server.serveStatic("/", SPIFFS, "/");

        // Start ElegantOTA
        AsyncElegantOTA.begin(&server);
        
        // Start server
        server.begin();
    }
}