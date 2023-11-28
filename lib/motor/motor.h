#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

#include "systemInfo.h"

class Motor {
   public:
    Motor(uint8_t directionPin, uint8_t speedPin, uint8_t encoderPin,
          bool rotationInverted);
    void setup();

    void setSpeedAndDir(uint8_t formattedSpeed, bool direction);
    void setVelocity(int8_t formattedVelocity);

    void stop();

    void takeStep();

    void setSteps(int steps);

    void checkEncoder();

    bool hasStepsRemaining();

   private:
    uint8_t directionPin;
    uint8_t speedPin;
    uint8_t encoderPin;

    bool rotationInverted;

    volatile uint16_t stepsRemaining;
    bool oldEncoderState;
};
#endif  // MOTORS_H
