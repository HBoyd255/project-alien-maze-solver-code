/**
 * @file motor.h
 * @brief Definitions for the Motor class, used to control a motor with an
 * encoder.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-09-02
 * @copyright Copyright (c) 2024
 */
#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

/**
 * @brief Motor class, used to control a motor with an encoder.
 *
 */
class Motor {
   public:
    /**
     * @brief Construct a new Motor object.
     *
     * @param directionPin The pin connected to the motor's direction pin.
     * @param speedPin The pin connected to the motor's speed pin.
     * @param encoderChannelA The pin connected to encoder channel A.
     * @param encoderChannelB The pin connected to  encoder channel B.
     * @param rotationInverted A boolean representing whether the motor's
     * rotation is inverted.
     */
    Motor(uint8_t directionPin, uint8_t speedPin, uint8_t encoderChannelA,
          uint8_t encoderChannelB, bool rotationInverted);

    /**
     * @brief Sets up the motor object, setting up the interrupt service
     * routine for the encoder, and setting the pin modes.
     *
     * @param isr_P A pointer to the interrupt service routine.
     */
    void setup(voidFuncPtr isr_P);

    /**
     * @brief Sets the velocity of the motor.
     *
     * @param formattedVelocity The velocity to set the motor to, as a bipolar
     * percentage [-100,100].
     */
    void setVelocity(int8_t formattedVelocity);

    /**
     * @brief Stops the motor.
     *
     */
    void stop();

    /**
     * @brief Get the distance traveled linearly by the motor in millimeters,
     * assuming 47mm wheels.
     *
     * @return (int32_t) The distance traveled by the motor in millimeters.
     */
    int32_t getDistanceTraveled();

    /**
     * @brief The interrupt service routine for the encoder.
     *
     */
    void isr();

   private:
    /**
     * @brief Sets the speed and direction of the motor.
     *
     * @param formattedSpeed A uint8_t representing the speed of the motor as a
     * percentage.
     * @param direction A boolean representing the direction of the motor.
     */
    void _setSpeedAndDir(uint8_t formattedSpeed, bool direction);

    /**
     * @brief The pin connected to the motor's direction pin.
     */
    uint8_t _directionPin;

    /**
     * @brief The pin connected to the motor's speed pin.
     */
    uint8_t _speedPin;

    /**
     * @brief The pin connected to encoder channel A.
     */
    uint8_t _encoderChannelA;

    /**
     * @brief The pin connected to encoder channel B.
     */
    uint8_t _encoderChannelB;

    /**
     * @brief The number of encoder steps the motor has taken.
     */
    volatile int32_t _encoderSteps = 0;

    /**
     * @brief A boolean representing whether the motor's rotation is inverted.
     */
    bool _rotationInverted;
};
#endif  // MOTORS_H
