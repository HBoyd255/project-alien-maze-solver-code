#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

#include "schedule.h"

// https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf

class Ultrasonic {
   public:
    Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout,
               uint16_t maxRange);
    void setup(voidFuncPtr isrPtr);
    int16_t readBlocking();
    int16_t read();

    void isr();

    void poll();

   private:
    uint8_t _triggerPin;
    uint8_t _echoPin;
    uint32_t _timeout;

    uint32_t _triggerTime = 0;
    uint32_t _pinUpTime = 0;
    uint32_t _pinDownTime = 0;
    int16_t _pulseWidth = 0;

    uint16_t _maxRange;
    int16_t _waitForNewPulse();

    void _trigger();

    int16_t _pulseWidthToDistance(int16_t reflectionDuration);
};

#endif  // ULTRASONIC_H