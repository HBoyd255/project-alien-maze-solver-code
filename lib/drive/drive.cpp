

#include "drive.h"

Drive::Drive()
    : leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_PIN, true),
      rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_PIN, false) {}

void Drive::setup() {
    this->leftMotor.setup();
    this->rightMotor.setup();
}

void Drive::checkEncoders() {
    this->leftMotor.checkEncoder();
    this->rightMotor.checkEncoder();
}

void Drive::move(int steps_CM) {
    this->leftMotor.setSteps(steps_CM);
    this->rightMotor.setSteps(steps_CM);
}
void Drive::rotate(int angle) {
    int steps = angle * 200 / 90;

    this->leftMotor.setSteps(steps);
    this->rightMotor.setSteps(-steps);
}

bool Drive::stepsRemaining() {
    return this->leftMotor.stepsRemaining() ||
           this->rightMotor.stepsRemaining();
}