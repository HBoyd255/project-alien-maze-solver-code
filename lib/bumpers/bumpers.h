#ifndef BUMPERS_H
#define BUMPERS_H

#include <Arduino.h>

#include "systemInfo.h"

class Bumper {
   public:
    Bumper(){};
    void setup();
    void assignCallback(voidFuncPtr function);
    byte read();
};

#endif  // BUMPERS_H
