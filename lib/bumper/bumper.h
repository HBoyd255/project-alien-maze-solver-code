#ifndef BUMPER_H
#define BUMPER_H

#include <Arduino.h>

#include "systemInfo.h"

//TODO - Add comments
class Bumper {
   public:
    Bumper(int dataPin, int loadPin, int clockPin, int interruptPin);
    void setup();
    void assignCallback(voidFuncPtr function);
    byte read();

    private:
    int dataPin;
    int loadPin;
    int clockPin;
    int interruptPin;
};

#endif  // BUMPER_H
