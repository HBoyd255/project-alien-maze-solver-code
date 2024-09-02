#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

class Motor {
   public:
    Motor(uint8_t directionPin, uint8_t speedPin, uint8_t encoderChannelA,
          uint8_t encoderChannelB, bool rotationInverted);
    void setup(voidFuncPtr isr_P);

    void setVelocity(int8_t formattedVelocity);

    void stop();

    int32_t getDistanceTraveled();

    void isr();

   private:
    void setSpeedAndDir(uint8_t formattedSpeed, bool direction);
    uint8_t _directionPin;
    uint8_t _speedPin;
    uint8_t _encoderChannelA;
    uint8_t _encoderChannelB;

    volatile int32_t _encoderSteps = 0;

    bool _rotationInverted;
};
#endif  // MOTORS_H
