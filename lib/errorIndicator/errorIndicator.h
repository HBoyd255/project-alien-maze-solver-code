#ifndef ERROR_INDICATOR_H
#define ERROR_INDICATOR_H

#include <Arduino.h>

#include "drive.h"
#include "pixels.h"

class ErrorIndicator {
   public:
    ErrorIndicator(uint8_t ledPin);
    void setup();
    void assignPixels(Pixels* pixelsPtr);
    void assignDrive(Drive* drivePtr);
    void errorOccurred(String errorMessage);

   private:
    uint8_t _ledPin;
    Pixels* pixelsPtr = NULL;
    Drive* drivePtr = NULL;
};

#endif  // ERROR_INDICATOR_H