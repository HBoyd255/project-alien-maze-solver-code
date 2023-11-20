#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

#include "systemInfo.h"

enum Directions { backwards, forwards };

class Motor {
   public:
    Motor(int direction_pin, int speed_pin, int encoder_pin,
          bool rotation_direction_inverted);
    void setup();
    void connectISR(voidFuncPtr function);

    void setSpeedAndDir(int formatted_speed, bool direction);
    void setVelocity(int formatted_velocity);
    void stop();

    void takeStep();

    void setSteps(int steps);

    void ISR();
    void checkISR();


   private:
    bool isr_flag;
    int direction_pin;
    int speed_pin;
    int encoder_pin;

    volatile int steps_remaining;
    bool rotation_direction_inverted;

    volatile bool enabled;
};
#endif  // MOTORS_H
