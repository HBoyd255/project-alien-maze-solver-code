
#ifndef INFRARED_H
#define INFRARED_H

#include <Arduino.h>
#include <Wire.h>

// TODO swap this for forward declaration
#include "history.h"
#include "schedule.h"

// FD
class PassiveSchedule;

class Infrared {
   public:
    Infrared(uint8_t index, int distanceFromCentre);

    void setup();

    int16_t read();
    int16_t readSafe();
    int readFromRobotCenter(bool getOldReading = false);

    bool brickAppeared(int range, int requiredDistanceChange);
    bool brickDisappeared(int range, int requiredDistanceChange);

    int16_t average();
    String getValueHistoryAsString();

    void poll();

   private:
    uint8_t _index;  // The index of the infrared sensor
    History _valueHistory;
    PassiveSchedule _historyUpdater;
    int _distanceFromCentre;
    uint8_t _shiftValue;  // The shift value of the infrared sensor

    int _mostRecentValue = -1;
    int _secondMostRecentValue = -1;

    void _setMultiplexer();
};

#endif  // INFRARED_H