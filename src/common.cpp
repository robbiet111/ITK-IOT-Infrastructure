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

#include "common_namespace.h"

namespace common {
    String influxdb_url;
    String wifi_ssid;
    String wifi_pass;
    running_mode current_mode = measure;
    bool refreshScreen = true;
    int btnClick = true;
    TFT_eSPI tft = TFT_eSPI(135, 240);
    WiFiMulti wifimulti;
    WebServer server(80);
    void sensor_setup() {
        common::wifimulti.addAP(wifi_ssid.c_str(), wifi_pass.c_str());
        delay(200);


        common::clear_display(2);
        tft.drawString("Connecting to WiFi", tft.width() / 2, tft.height() / 2 - 16);
        tft.drawString(String(wifi_ssid), tft.width() / 2, tft.height() / 2 + 16);

        int counter = 0;

        Serial.println("Connecting to WiFi: ");
        Serial.println(String(wifi_ssid));
        while (common::wifimulti.run() != WL_CONNECTED) {
            delay(100);
            counter += 1;
            if (counter == 5) {
            current_mode = enter_cli;
            common::clear_display(2);
            tft.drawString("Connection failed", tft.width() / 2, tft.height() / 2-16);
            tft.drawString("Entering CLI", tft.width() / 2, tft.height() / 2+16);
            delay(2500);
            break;
            }
        }
        Serial.println();
        if (current_mode == measure) {
            WiFi.setHostname(ESP32_HOST_NAME);
            Serial.print("WiFi connected: "); Serial.println(WiFi.SSID());
            Serial.print("Hostname: ");       Serial.println(WiFi.getHostname());
            Serial.print("IP address: ");     Serial.println(WiFi.localIP()); char buf[16]; sprintf(buf, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
            Serial.print("MAC address: ");    Serial.println(WiFi.macAddress());

            common::clear_display(1);
            tft.drawString("WiFi connected: " + String(WiFi.SSID()), tft.width() / 2, tft.height() / 2 - 24);
            tft.drawString("Hostname: " + String(WiFi.getHostname()), tft.width() / 2, tft.height() / 2 - 8);
            tft.drawString("IP address: " + String(buf), tft.width() / 2, tft.height() / 2 + 8);
            tft.drawString("MAC address: " + String(WiFi.macAddress()), tft.width() / 2, tft.height() / 2 + 24);
            delay(5000);
            tft.fillScreen(TFT_BLACK);
        }
    }
}