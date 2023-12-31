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

#ifndef COMMON_NAMESPACE_H
#define COMMON_NAMESPACE_H
#include "common_headers.h"


#define ESP32_HOST_NAME       "UNIT_11"

namespace common {

    extern String influxdb_url;
    extern String wifi_ssid;
    extern String wifi_pass;
    enum running_mode {measure=0, enter_cli=1, update=2};
    extern running_mode current_mode;

    extern bool refreshScreen;

    extern int btnClick;

    extern WiFiMulti wifimulti;
    extern WebServer server;

    extern TFT_eSPI tft;
    inline void clear_display(int font_size) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(font_size);
        tft.setTextDatum(MC_DATUM);
    }

    inline void put_on_display(String text, int pos) {
        tft.drawString(text, tft.width() / 2, tft.height() / 2 + pos);
    }

    void sensor_setup();
}
#endif