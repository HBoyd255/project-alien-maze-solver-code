
#ifndef INFRARED_H
#define INFRARED_H

#include <Arduino.h>
#include <Wire.h>

#include "errorIndicator.h"
#include "history.h"
#include "schedule.h"

class Infrared {
   public:
    Infrared(ErrorIndicator* errorIndicatorPtr, uint8_t index,
             uint16_t maxRange);

    void setup(voidFuncPtr PTR);

    int16_t read();
    int16_t readSafe();

    int16_t average();
    String getValueHistoryAsString();

    void poll();
    void routineFunction();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    uint8_t _index;       // The index of the infrared sensor
    uint8_t _shiftValue;  // The shift value of the infrared sensor
    uint16_t _maxRange;

    History _valueHistory;
    Schedule _historyUpdater;

    void _setMultiplexer();
};

#endif  // INFRARED_H