

#include "drive.h"

Drive::Drive(Motor* leftMotorPtr, Motor* rightMotorPtr) {
    this->leftMotorPtr = leftMotorPtr;
    this->rightMotorPtr = rightMotorPtr;
}

void Drive::setVelocity(int velocity) {
    this->leftMotorPtr->setVelocity(velocity);
    this->rightMotorPtr->setVelocity(velocity);
}

void Drive::stop() {
    this->leftMotorPtr->stop();
    this->rightMotorPtr->stop();
}