#ifndef BUMPER_CONTROL_H
#define BUMPER_CONTROL_H

#include <Arduino.h>

class Bumper {
   public:
    Bumper(uint8_t data, uint8_t load, uint8_t clock);
    uint8_t getInstant();

   private:
    uint8_t data;
    uint8_t load;
    uint8_t clock;

    uint8_t currentValue;
    uint8_t previousValue;
};

#endif  // BUMPER_CONTROL_H
