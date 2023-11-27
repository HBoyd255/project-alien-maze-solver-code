#ifndef DRIVE_H
#define DRIVE_H

#include <Arduino.h>

#include "motor.h"

class Drive {
   public:
    Drive()
        : leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                    LEFT_MOTOR_ENCODER_PIN, true),
          rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                     RIGHT_MOTOR_ENCODER_PIN, false){};
    void setup();

    void checkEncoders();

    void move(int steps);

    void setSteps(int stepsLeft, int stepsRight);

    void rotate(int angle);

    bool hasStepsRemaining();

    void setVelocity(int velocity);

    void stop();

   private:
    Motor leftMotor;
    Motor rightMotor;
};

#endif  // DRIVE_H