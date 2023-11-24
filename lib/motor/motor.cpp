
#include "motor.h"

Motor::Motor(int direction_pin, int speed_pin, int encoder_pin,
             bool rotation_direction_inverted) {
    this->direction_pin = direction_pin;
    this->speed_pin = speed_pin;
    this->encoder_pin = encoder_pin;

    this->rotation_direction_inverted = rotation_direction_inverted;

    this->enabled = true;
    this->steps_remaining = 0;
}

void Motor::setup() {
    pinMode(this->direction_pin, OUTPUT);
    pinMode(this->speed_pin, OUTPUT);
    pinMode(this->encoder_pin, INPUT);
}

// TODO Add the bit that makes it not whine when given small values

void Motor::setSpeedAndDir(int formatted_speed, bool direction) {
    // Make sure that the provided value is within the range of 0 to 100,
    // return 1 if not to signify an error.
    if ((formatted_speed > 100) || (formatted_speed < 0)) {
        Serial.println("Speed must be between 0 and 100");
        return;
    }

    // scale the speed from a directional value from -100 to 100, to a
    // directionless value from 0 to 255, ready to be fed into the pwm signal.
    int scaled_speed = abs(formatted_speed) * 2.55;

    // fix that 100 gets rounded to 254, and set it to the desired 255.
    if (scaled_speed == 254) {
        scaled_speed = 255;
    }

    // If the target motor is the left motor, reverse the direction, because to
    // Drive forwards, both motors need to rotate in the opposite direction.
    direction ^= this->rotation_direction_inverted;

    digitalWrite(this->direction_pin, direction);
    analogWrite(this->speed_pin, scaled_speed);
}

void Motor::setVelocity(int formatted_velocity) {
    // Make sure that the provided value is withing the range of -100 to
    // 100, return 1 if not to signify an error.
    if ((formatted_velocity > 100) || (formatted_velocity < -100)) {
        Serial.println("Velocity must be between -100 and 100");
        return;
    }

    // Set the direction to forwards("1") if the speed is greater than 0,
    // Direction represents the drive direction, not the rotation direction
    bool direction = formatted_velocity > 0;

    this->setSpeedAndDir(abs(formatted_velocity), direction);
}

//TODO FIX
void Motor::stop() {}

void Motor::takeStep() {
    // If the motor is enabled and there are still steps remaining, keep
    // stepping.
    if (this->enabled && this->steps_remaining) {
        // reduce left_steps_remaining by 1, or increase it by 1 if its
        // negative, either way, take one step closer to 0.
        this->steps_remaining += (this->steps_remaining > 0) ? -1 : 1;

        // If the steps remaining is less than 5, set it to 0, this is to
        // prevent the motor from stuttering when it gets close to the target
        // position.
        if (abs(this->steps_remaining) < 5) {
            this->steps_remaining = 0;
        }

        // Calculate how fast to set the motor, the speed is proportional to
        // the number of steps remaining, and is capped at 100.
        int velocity = constrain(this->steps_remaining, -100, 100);

        if (FAST_MODE) {
            velocity = 10 * velocity;
            velocity = constrain(velocity, -100, 100);
        }

        // Set the motor to the calculated velocity.
        this->setVelocity(velocity);
    }
    // if the motor is disabled, or there are no steps remaining, stop the
    // motor.
    else {
        this->stop();
    }
}

void Motor::setSteps(int steps) {
    this->steps_remaining = steps;
    this->takeStep();
}

void Motor::checkEncoder() {
    if (digitalRead(this->encoder_pin) != this->old_encoder_state) {
        this->takeStep();
    }
    this->old_encoder_state = digitalRead(this->encoder_pin);
}

bool Motor::stepsRemaining() { return (bool)this->steps_remaining; }