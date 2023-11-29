#ifndef DRIVE_H
#define DRIVE_H

#include <Arduino.h>

#include "motor.h"

class Drive {
   public:
    Drive(Motor* leftMotorPtr, Motor* rightMotorPtr);

    void setVelocity(int velocity);

    void stop();

   private:
    Motor* leftMotorPtr;
    Motor* rightMotorPtr;
};

#endif  // DRIVE_H