#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf

#define uint16_t unsigned short

class Ultrasonic {
   public:
    Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout);
    void setup(voidFuncPtr isr);
    uint16_t read();
    void isr();

   private:
    uint16_t myPulseFunction(uint32_t timeout);
    uint8_t triggerPin;
    uint8_t echoPin;
    uint32_t timeout;

    uint32_t pinDownTime = 0;
    uint32_t pinUpTime = 0;
};

#endif  // ULTRASONIC_H