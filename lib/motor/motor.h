#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

#include "systemInfo.h"

class Motor {
   public:
    Motor(int directionPin, int speedPin, int encoderPin,
          bool rotationInverted);
    void setup();

    void setSpeedAndDir(int formattedSpeed, bool direction);
    void setVelocity(int formattedVelocity);

    void stop();

    void takeStep();

    void setSteps(int steps);

    void checkEncoder();

    bool hasStepsRemaining();

   private:
    int directionPin;
    int speedPin;
    int encoderPin;

    bool rotationInverted;

    volatile int stepsRemaining;
    bool oldEncoderState;
};
#endif  // MOTORS_H
