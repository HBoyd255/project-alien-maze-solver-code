

#include "drive.h"

#include "motor.h"

Drive::Drive(Motor* leftMotorPtr, Motor* rightMotorPtr)
    : _leftMotorPtr(leftMotorPtr), _rightMotorPtr(rightMotorPtr) {}

void Drive::setVelocity(int velocity) {
    this->_leftMotorPtr->setVelocity(velocity);
    this->_rightMotorPtr->setVelocity(velocity);
}

void Drive::setRotationalVelocity(int rotationalVelocity) {
    this->_leftMotorPtr->setVelocity(-rotationalVelocity);
    this->_rightMotorPtr->setVelocity(rotationalVelocity);
}

void Drive::forwards() { this->setVelocity(this->_minSpeed); }

void Drive::back() { this->setVelocity(-this->_minSpeed); }

void Drive::left() { this->setRotationalVelocity(this->_minSpeed); }

void Drive::right() { this->setRotationalVelocity(-this->_minSpeed); }

void Drive::stop() { this->setVelocity(0); }