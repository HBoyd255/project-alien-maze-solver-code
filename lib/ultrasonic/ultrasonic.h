#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf




class Ultrasonic {
   public:
    Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout);
    void setup();
    uint16_t read();

   private:
    uint8_t triggerPin;
    uint8_t echoPin;
    uint32_t timeout;
};

#endif  // ULTRASONIC_H