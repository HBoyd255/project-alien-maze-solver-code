
#ifndef INFRARED_H
#define INFRARED_H

#include <Arduino.h>
#include <Wire.h>

#include "errorIndicator.h"

class Infrared {
   public:
    Infrared(ErrorIndicator* errorIndicatorPtr, uint8_t index);

    void setup();

    uint16_t read();

   private:  // TODO add Leading _ to variable names
    ErrorIndicator* _errorIndicatorPtr;
    uint8_t index;       // The index of the infrared sensor
    uint8_t shiftValue;  // The shift value of the infrared sensor

    void grabMultiplexer();
};

#endif  // INFRARED_H