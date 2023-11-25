
#include "motor.h"

Motor::Motor(int directionPin, int speedPin, int encoderPin,
             bool rotationInverted) {
    this->directionPin = directionPin;
    this->speedPin = speedPin;
    this->encoderPin = encoderPin;

    this->rotationInverted = rotationInverted;
    this->stepsRemaining = 0;
}

void Motor::setup() {
    pinMode(this->directionPin, OUTPUT);
    pinMode(this->speedPin, OUTPUT);
    pinMode(this->encoderPin, INPUT);
}

void Motor::setSpeedAndDir(int formattedSpeed, bool direction) {
    // Make sure that the provided value is within the range of 0 to 100,
    // return 1 if not to signify an error.
    if ((formattedSpeed > 100) || (formattedSpeed < 0)) {
        Serial.println("Speed must be between 0 and 100");
        return;
    }

    // If the speed is less than 5, set it to 0, this is to prevent the motor
    // from just whining when it gets a small value.
    if (formattedSpeed < 10) {
        formattedSpeed = 0;
    }

    // scale the speed from a directional value from -100 to 100, to a
    // directionless value from 0 to 255, ready to be fed into the pwm signal.
    int scaledSpeed = abs(formattedSpeed) * 2.55;

    // fix that 100 gets rounded to 254, and set it to the desired 255.
    if (scaledSpeed == 254) {
        scaledSpeed = 255;
    }

    // If the target motor is the left motor, reverse the direction, because to
    // Drive forwards, both motors need to rotate in the opposite direction.
    direction ^= this->rotationInverted;

    digitalWrite(this->directionPin, direction);
    analogWrite(this->speedPin, scaledSpeed);
}

void Motor::setVelocity(int formattedVelocity) {
    // Make sure that the provided value is withing the range of -100 to
    // 100, return 1 if not to signify an error.
    if ((formattedVelocity > 100) || (formattedVelocity < -100)) {
        Serial.println("Velocity must be between -100 and 100");
        return;
    }

    // Set the direction to forwards("1") if the speed is greater than 0,
    // Direction represents the drive direction, not the rotation direction
    bool direction = formattedVelocity > 0;

    this->setSpeedAndDir(abs(formattedVelocity), direction);
}

void Motor::stop() {
    this->setSpeedAndDir(0, 0);
    this->stepsRemaining = 0;
}

void Motor::takeStep() {
    // If there are steps remaining, take a step.
    if (this->hasStepsRemaining()) {
        // reduce stepsRemaining by 1, or increase it by 1 if its
        // negative, either way, take one step closer to 0.
        this->stepsRemaining += (this->stepsRemaining > 0) ? -1 : 1;

        // If the steps remaining is less than 5, set it to 0, this is to
        // prevent the motor from stuttering when it gets close to the target
        // position.
        if (abs(this->stepsRemaining) < 5) {
            this->stepsRemaining = 0;
        }

        // Calculate how fast to set the motor, the speed is proportional to
        // the number of steps remaining, and is capped at 100.
        int velocity = constrain(this->stepsRemaining, -100, 100);

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
    this->stepsRemaining = steps;
    this->takeStep();
}

void Motor::checkEncoder() {
    if (digitalRead(this->encoderPin) != this->oldEncoderState) {
        this->takeStep();
    }
    this->oldEncoderState = digitalRead(this->encoderPin);
}

bool Motor::hasStepsRemaining() { return (bool)this->stepsRemaining; }