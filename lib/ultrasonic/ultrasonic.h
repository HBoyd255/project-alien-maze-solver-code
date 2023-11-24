#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf




class Ultrasonic {
   public:
    Ultrasonic(int triggerPin, int echoPin, unsigned long timeout);
    void setup();
    unsigned int read();

   private:
    int triggerPin;
    int echoPin;
    unsigned long timeout;
};

#endif  // ULTRASONIC_H