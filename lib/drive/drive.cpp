

#include "drive.h"

Drive::Drive()
    : leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_PIN, true),
      rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_PIN, false) {}

void Drive::setup() {
    this->leftMotor.setup();
    this->rightMotor.setup();

    this->leftMotor.connectISR([]() {leftMotor.ISR(); });
    this->rightMotor.connectISR([]() {rightMotor.ISR(); });
}

void Drive::move(int steps) {
    this->leftMotor.setSteps(steps);
    this->rightMotor.setSteps(steps);
}

void Drive::checkISR() {
    this->leftMotor.checkISR();
    this->rightMotor.checkISR();
}