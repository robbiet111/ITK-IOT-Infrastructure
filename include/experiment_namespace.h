#include <Adafruit_MAX31865.h>
#include "common_namespace.h"

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF 430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL 100.0

namespace experiment {
    extern Adafruit_MAX31865 max_ada;
    inline void begin() {
        max_ada.begin(MAX31865_2WIRE);  // set to 2WIRE or 4WIRE as necessary 
    }
    void measure();
}