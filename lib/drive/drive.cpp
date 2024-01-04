/**
 * @file drive.cpp
 * @brief Definition  of the Drive class, an interface for the
 * Motor class, to simplify controlling the robot.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2024-01-04
 * @copyright Copyright (c) 2024
 */
#include "drive.h"

#include "motor.h"

/**
 * @brief Construct a new Drive object.
 *
 * @param leftMotorPtr Pointer to the left motor object.
 * @param rightMotorPtr Pointer to the right motor object.
 * @param defaultSpeed (int) The percentage of the motor's max spreed to use
 * as default.
 */
Drive::Drive(Motor* leftMotorPtr, Motor* rightMotorPtr, int defaultSpeed)
    : _leftMotorPtr(leftMotorPtr),
      _rightMotorPtr(rightMotorPtr),
      _defaultSpeed(defaultSpeed) {}

/**
 * @brief Set the Velocity of both motors.
 *
 * @param velocity A positive or negative percentage of the motors max
 * speed, a negative value is a reverse speed, in the range [-100,100].
 */
void Drive::setVelocity(int velocity) {
    this->_leftMotorPtr->setVelocity(velocity);
    this->_rightMotorPtr->setVelocity(velocity);
}

/**
 * @brief Set the Velocity of both motors in opposite directions, in order
 * to rotate the robot.
 *
 * A positive value of rotationalVelocity will rotate the robot
 * counter-clockwise.
 *
 * @param rotationalVelocity A positive or negative percentage of the motors
 * max speed, a negative value is a reverse speed, in the range [-100,100].
 */
void Drive::setRotationalVelocity(int rotationalVelocity) {
    this->_leftMotorPtr->setVelocity(-rotationalVelocity);
    this->_rightMotorPtr->setVelocity(rotationalVelocity);
}

/**
 * @brief Drives the robot forwards by setting both motors to the default
 * speed.
 */
void Drive::forwards() { this->setVelocity(this->_defaultSpeed); }

/**
 * @brief Drives the robot backwards by setting both motors to the negative
 * default speed.
 */
void Drive::backwards() { this->setVelocity(-this->_defaultSpeed); }

/**
 * @brief Starts rotating the robot counter-clockwise, by setting the right
 * motor to the default speed and the left motor to the negative default
 * speed.
 */
void Drive::left() { this->setRotationalVelocity(this->_defaultSpeed); }

/**
 * @brief Starts rotating the robot clockwise, by setting the right motor to
 * the negative default speed and the left motor to the default speed.
 */
void Drive::right() { this->setRotationalVelocity(-this->_defaultSpeed); }

/**
 * @brief Stops the robot by setting both motors to 0% speed.
 */
void Drive::stop() { this->setVelocity(0); }