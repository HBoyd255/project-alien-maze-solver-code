#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

#define LEFT_MOTOR_DIRECTION_PIN A0
#define RIGHT_MOTOR_DIRECTION_PIN A1

#define LEFT_MOTOR_SPEED_PIN D9
#define RIGHT_MOTOR_SPEED_PIN D10

#define LEFT_ENCODER_PIN D2
#define RIGHT_ENCODER_PIN D3

enum Directions { backwards, forwards };

class motor {
   public:
    motor(int direction_pin, int speed_pin, int encoder_pin,
          bool rotation_direction_inverted);
    void setup();
    void setSpeedAndDir(int formatted_speed, bool direction);
    void setVelocity(int formatted_velocity);
    void stop();
    void takeStep();

    volatile int steps_remaining;

   private:
    int direction_pin;
    int speed_pin;
    int encoder_pin;

    bool rotation_direction_inverted;

    volatile bool enabled;
};
#endif  // MOTORS_H
