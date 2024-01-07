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
     * @param defaultSpeed (int) The percentage of the motor's max speed to use
     * as default.
     */
    Drive(Motor* leftMotorPtr, Motor* rightMotorPtr, int defaultSpeed);

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
    void setVelocity(int linearVelocity, int rotationalVelocity = 0);

    /**
     * @brief Drives the robot forwards at the default speed, allowing for an
     * optional slight offset in rotational velocity.
     *
     * A positive offsetRotationalVelocity with turn the robot counter-clockwise
     *
     * @param offsetRotationalVelocity
     */
    void forwards(int offsetRotationalVelocity = 0);

    /**
     * @brief Drives the robot backwards at the default speed, allowing for an
     * optional slight offset in rotational velocity.
     *
     * A positive offsetRotationalVelocity with turn the robot counter-clockwise
     *
     * @param offsetRotationalVelocity
     */
    void backwards(int offsetRotationalVelocity = 0);

    /**
     * @brief Starts rotating the robot counter-clockwise, by setting the right
     * motor to the default speed and the left motor to the negative default
     * speed.
     */
    void turnLeft();

    /**
     * @brief Starts rotating the robot clockwise, by setting the right motor to
     * the negative default speed and the left motor to the default speed.
     */
    void turnRight();

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
     * @brief The percentage of the motor's max speed to use as default.
     */
    int _defaultSpeed;
};

#endif  // DRIVE_H