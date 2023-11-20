#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

#include "systemInfo.h"

#define FAST_MODE true

class Motor {
   public:
    Motor(int direction_pin, int speed_pin, int encoder_pin,
          bool rotation_direction_inverted);
    void setup();

    void setSpeedAndDir(int formatted_speed, bool direction);
    void setVelocity(int formatted_velocity);

    void stop();

    void takeStep();

    void setSteps(int steps);

    void checkEncoder();

    bool stepsRemaining();

   private:
    int direction_pin;
    int speed_pin;
    int encoder_pin;

    bool rotation_direction_inverted;

    volatile int steps_remaining;

    bool old_encoder_state;

    volatile bool enabled;
};
#endif  // MOTORS_H
