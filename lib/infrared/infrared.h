
#ifndef INFRARED_H
#define INFRARED_H

#include <Arduino.h>
#include <Wire.h>

#include "errorIndicator.h"

class Infrared {
   public:
    Infrared(ErrorIndicator* errorIndicatorPtr, uint8_t index,
             uint16_t maxRange);

    void setup();

    int16_t read();
    int16_t sample();
    void poll();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    uint8_t _index;       // The index of the infrared sensor
    uint8_t _shiftValue;  // The shift value of the infrared sensor
    uint16_t _maxRange;

    

    void _grabMultiplexer();
};

#endif  // INFRARED_H