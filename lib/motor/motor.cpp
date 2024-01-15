#include "motor.h"

Motor::Motor(uint8_t directionPin, uint8_t speedPin, uint8_t encoderChannelA,
             uint8_t encoderChannelB, bool rotationInverted)
    : _directionPin(directionPin),
      _speedPin(speedPin),
      _encoderChannelA(encoderChannelA),
      _encoderChannelB(encoderChannelB),
      _rotationInverted(rotationInverted)

{}

/**
 * @brief Sets up the motor class by
 *
 * @param isrPtr Pointer to the
 */
void Motor::setup(voidFuncPtr isrPtr) {
    pinMode(this->_directionPin, OUTPUT);
    pinMode(this->_speedPin, OUTPUT);
    pinMode(this->_encoderChannelA, INPUT);
    pinMode(this->_encoderChannelB, INPUT);

    attachInterrupt(digitalPinToInterrupt(this->_encoderChannelA), isrPtr,
                    CHANGE);
}

void Motor::setSpeedAndDir(uint8_t formattedSpeed, bool direction) {
    // Make sure that the provided value is within the range of 0 to 100,
    // return 1 if not to signify an error.
    if (formattedSpeed > 100) {
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
    direction ^= this->_rotationInverted;

    digitalWrite(this->_directionPin, direction);
    analogWrite(this->_speedPin, scaledSpeed);
}

void Motor::setVelocity(int8_t formattedVelocity) {
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

void Motor::stop() { this->setSpeedAndDir(0, 0); }

void Motor::resetTimer() { this->_lastMoveTime = millis(); }

void Motor::isr() {
    if (digitalRead(this->_encoderChannelA) !=
        (digitalRead(this->_encoderChannelB) ^ this->_rotationInverted)) {
        this->_encoderSteps++;
    } else {
        this->_encoderSteps--;
    }
    this->resetTimer();
}

int32_t Motor::getDistanceTraveled() {
    // One rotation is 300 steps
    // and the wheel circumference is 147.65mm
    // so 1 step is like 0.5mm
    // meaning that the distance in millimeters can be approximated by dividing
    // the steps by 2.

    // further testing showed that 200 mm is 390 steps, so distance can be
    // calculated by dividing the steps by 1.95.

    return this->_encoderSteps / 1.95;
}

long Motor::timeSinceLastMoved() { return millis() - this->_lastMoveTime; }
