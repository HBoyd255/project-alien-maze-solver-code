#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf

#define uint16_t unsigned short

class Ultrasonic {
   public:
    Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout,
               uint16_t maxRange);
    void setup(voidFuncPtr isr);
    int16_t read();
    void isr();

   private:
    int16_t _pulseDuration(uint32_t timeout);
    uint8_t _triggerPin;
    uint8_t _echoPin;
    uint32_t _timeout;

    uint32_t _pinDownTime = 0;
    uint32_t _pinUpTime = 0;

    uint16_t _maxRange;
};

#endif  // ULTRASONIC_H