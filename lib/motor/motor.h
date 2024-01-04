#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

class Motor {
   public:
    Motor(uint8_t directionPin, uint8_t speedPin, uint8_t encoderChannelA,
          uint8_t encoderChannelB, bool rotationInverted);
    void setup(voidFuncPtr isrPtr);

    void setVelocity(int8_t formattedVelocity);

    void stop();

    void isr();

    /// @brief getter that returns the linear travel distance of the motor in
    // In millimetres.
    /// @return the steps taken
    int32_t getDistanceTraveled();

   private:
    void setSpeedAndDir(uint8_t formattedSpeed, bool direction);
    uint8_t _directionPin;
    uint8_t _speedPin;
    uint8_t _encoderChannelA;
    uint8_t _encoderChannelB;

    volatile int32_t _encoderSteps = 0;

    bool _rotationInverted;

    bool oldEncoderState;
};
#endif  // MOTORS_H
