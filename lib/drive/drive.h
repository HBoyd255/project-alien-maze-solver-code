/**
 * @file drive.h
 * @brief Declaration of the Drive class, an interface for the Motor class, to
 * simplify controlling the robot.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2024-01-04
 * @copyright Copyright (c) 2024
 */
#ifndef DRIVE_H
#define DRIVE_H

// Forward declaration of the Motor class.
class Motor;

/**
 * @brief Drive class, to simplify controlling the motors.
 */
class Drive {
   public:
    /**
     * @brief Construct a new Drive object.
     *
     * @param leftMotorPtr Pointer to the left motor object.
     * @param rightMotorPtr Pointer to the right motor object.
     * @param defaultSpeed (int) The percentage of the motor's max spreed to use
     * as default.
     */
    Drive(Motor* leftMotorPtr, Motor* rightMotorPtr, int defaultSpeed);

    /**
     * @brief Set the Velocity of both motors.
     *
     * @param velocity A positive or negative percentage of the motors max
     * speed, a negative value is a reverse speed, in the range [-100,100].
     */
    void setVelocity(int velocity);

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
    void setRotationalVelocity(int rotationalVelocity);

    /**
     * @brief Drives the robot forwards by setting both motors to the default
     * speed.
     */
    void forwards();

    /**
     * @brief Drives the robot backwards by setting both motors to the negative
     * default speed.
     */
    void backwards();

    /**
     * @brief Starts rotating the robot counter-clockwise, by setting the right
     * motor to the default speed and the left motor to the negative default
     * speed.
     */
    void left();

    /**
     * @brief Starts rotating the robot clockwise, by setting the right motor to
     * the negative default speed and the left motor to the default speed.
     */
    void right();

    /**
     * @brief Stops the robot by setting both motors to 0% speed.
     */
    void stop();

   private:
    /**
     * @brief A pointer to the left motor object.
     */
    Motor* _leftMotorPtr;
    /**
     * @brief A pointer to the right motor object.
     */
    Motor* _rightMotorPtr;

    /**
     * @brief The percentage of the motor's max spreed to use as default.
     */
    int _defaultSpeed;
};

#endif  // DRIVE_H