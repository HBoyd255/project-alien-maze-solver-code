#ifndef BUMPERS_H
#define BUMPERS_H

#include <Arduino.h>

#define BUMPERS_SHIFT_REG_DATA A7
#define BUMPERS_SHIFT_REG_LOAD A6
#define BUMPERS_SHIFT_REG_CLOCK A3

#define BUMPERS_INTERRUPT_PIN D8

class Bumper {
   public:
    Bumper();
    void setup();
    void assignCallback(voidFuncPtr function);
    byte read();
};

#endif  // BUMPERS_H
