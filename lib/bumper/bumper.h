#ifndef BUMPER_H
#define BUMPER_H

#include <Arduino.h>

#include "systemInfo.h"

class Bumper {
   public:
    Bumper(uint8_t dataPin, uint8_t loadPin, uint8_t clockPin,
           uint8_t interruptPin);
    void setup();
    void assignCallback(voidFuncPtr function);
    uint8_t read();

   private:
    uint8_t dataPin;
    uint8_t loadPin;
    uint8_t clockPin;
    uint8_t interruptPin;
};

#endif  // BUMPER_H
