#ifndef OTA_NAMESPACE_H
#define OTA_NAMESPACE_H

#include "common_namespace.h"
using namespace common;

namespace OTA {
    extern String style;
    extern String loginIndex;
    extern String serverIndex;
    extern const char* host;
    void ota_setup();
}
#endif