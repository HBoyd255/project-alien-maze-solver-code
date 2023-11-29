#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

#include "systemInfo.h"

class Motor {
   public:
    Motor(uint8_t directionPin, uint8_t speedPin, uint8_t encoderAPin,
          uint8_t encoderBPin, bool rotationInverted);
    void setup(voidFuncPtr isrPtr);

    void setSpeedAndDir(uint8_t formattedSpeed, bool direction);
    void setVelocity(int8_t formattedVelocity);

    void stop();

    void isr();

    int32_t getStepsInMillimeters();

   private:
    uint8_t directionPin;
    uint8_t speedPin;
    uint8_t encoderAPin;
    uint8_t encoderBPin;

    volatile int32_t steps;

    bool rotationInverted;

    bool oldEncoderState;
};
#endif  // MOTORS_H
