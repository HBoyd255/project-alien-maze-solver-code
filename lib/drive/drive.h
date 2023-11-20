#ifndef DRIVE_H
#define DRIVE_H

#include <Arduino.h>

#include "motor.h"

class Drive {
   public:
    Drive();
    void setup();

    void checkEncoders();

    void move(int steps);

    void rotate(int angle);

    bool stepsRemaining();


   private:
    Motor leftMotor;
    Motor rightMotor;
};

#endif  // DRIVE_H