/**
 * @file drive.cpp
 * @brief Definition  of the Drive class, an interface for the
 * Motor class, to simplify controlling the robot.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-01-04
 * @copyright Copyright (c) 2024
 */
#include "drive.h"

#include "motor.h"

/**
 * @brief Construct a new Drive object.
 *
 * @param leftMotor_P Pointer to the left motor object.
 * @param rightMotor_P Pointer to the right motor object.
 * @param defaultSpeed (int) The percentage of the motor's max seed to use
 * as default.
 */
Drive::Drive(Motor* leftMotor_P, Motor* rightMotor_P, int defaultSpeed)
    : _leftMotor_P(leftMotor_P),
      _rightMotor_P(rightMotor_P),
      _defaultSpeed(defaultSpeed) {}

/**
 * @brief Sets the speed of both motors based on a bipolar percentage of
 * their maximum speeds, allowing for both linear and rotational velocities.
 *
 * The speeds are calculated as
 * - Left motor velocity = linearVelocity - rotationalVelocity
 * - right motor velocity = linearVelocity + rotationalVelocity
 *
 * both values being constrained to a range of [-100,100]
 *
 * A positive linearVelocity moves the robot forwards.
 * A positive rotationalVelocity moved the robot counter-clockwise.
 *
 * rotationalVelocity is set to 0 be default.
 *
 * A positive value of rotationalVelocity will rotate the robot
 * counter-clockwise.
 *
 * @param linearVelocity The speed to set the motors in the same direction
 * as a bipolar percentage [-100,100].
 * @param rotationalVelocity The speed to set the motors in the opposite
 * direction as a bipolar percentage [-100,100].
 */
void Drive::setVelocity(int linearVelocity, int rotationalVelocity) {
    int leftVelocity = linearVelocity - rotationalVelocity;
    leftVelocity = constrain(leftVelocity, -100, 100);

    int rightVelocity = linearVelocity + rotationalVelocity;
    rightVelocity = constrain(rightVelocity, -100, 100);

    this->_leftMotor_P->setVelocity(leftVelocity);
    this->_rightMotor_P->setVelocity(rightVelocity);
}

/**
 * @brief Drives the robot forwards at the default speed, allowing for an
 * optional slight offset in rotational velocity.
 *
 * A positive offsetRotationalVelocity with turn the robot counter-clockwise
 *
 * @param offsetRotationalVelocity
 */
void Drive::forwards(int offsetRotationalVelocity) {
    this->setVelocity(this->_defaultSpeed, offsetRotationalVelocity);
}

/**
 * @brief Drives the robot backwards at the default speed, allowing for an
 * optional slight offset in rotational velocity.
 *
 * A positive offsetRotationalVelocity with turn the robot counter-clockwise
 *
 * @param offsetRotationalVelocity
 */
void Drive::backwards(int offset) {
    this->setVelocity(-(this->_defaultSpeed), offset);
}

/**
 * @brief Starts rotating the robot counter-clockwise, by setting the right
 * motor to the default speed and the left motor to the negative default
 * speed.
 */
void Drive::turnLeft() {
    // Set the linear velocity to 0, and the rotational velocity to the default
    // speed.
    this->setVelocity(0, this->_defaultSpeed);
}

/**
 * @brief Starts rotating the robot clockwise, by setting the right motor to
 * the negative default speed and the left motor to the default speed.
 */
void Drive::turnRight() {
    // Set the linear velocity to 0, and the rotational velocity to the negative
    // default speed.
    this->setVelocity(0, -this->_defaultSpeed);
}

/**
 * @brief Spins the robot clockwise as full speed.
 * This function should be used sparingly.
 */
void Drive::fullSpeedSpinLeft() {
    this->_leftMotor_P->setVelocity(-100);
    this->_rightMotor_P->setVelocity(100);
}

/**
 * @brief Stops the robot by setting both motors to 0% speed.
 */
void Drive::stop() { this->setVelocity(0); }