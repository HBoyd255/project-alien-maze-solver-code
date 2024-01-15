
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
             int distanceFromCentre);

    void setup(voidFuncPtr PTR);

    int16_t read();
    int16_t readSafe();
    int readFromRobotCenter(bool getOldReading = false);

    //     int getMostResentDistance();
    //     int secMost();
    //

    bool brickAppeared(int range, int requiredDistanceChange);
    bool brickDisappeared(int range, int requiredDistanceChange);

    int16_t average();
    String getValueHistoryAsString();

    void poll();
    void routineFunction();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    uint8_t _index;       // The index of the infrared sensor
    uint8_t _shiftValue;  // The shift value of the infrared sensor

    int _distanceFromCentre;

    int _mostRecentValue = -1;
    int _secondMostRecentValue = -1;

    History _valueHistory;
    // TODO update this to use a passive schedule.
    Schedule _historyUpdater;

    void _setMultiplexer();
};

#endif  // INFRARED_H