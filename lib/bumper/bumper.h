#ifndef BUMPER_H
#define BUMPER_H

/**
 * @file bumper.h
 * @brief Declaration of the Bumper class, responsible for reading the state of
 * the 8 buttons that make up the bumper around the circumference of the robot.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */

#include <Arduino.h>

#include "angle.h"

/**
 * @brief The Bumper class, responsible for reading the state of the 8 buttons
 * that make up the bumper around the circumference of the robot.
 *
 */
class Bumper {
   public:
    /**
     * @brief Construct a new Bumper object.
     *
     * @param dataPin The pin connected to the data pin on the shift register.
     * @param loadPin The pin connected to the load pin on the shift register.
     * @param clockPin The pin connected to the clock pin on the shift register.
     * @param interruptPin The pin connected to the xor gate that changes when
     * any button is pressed.
     * @param rotationOffset The offset in degrees that the bumper buttons have
     * been rotated around from the position where the front button would be at
     * 0 degrees.
     */
    Bumper(uint8_t dataPin, uint8_t loadPin, uint8_t clockPin,
           uint8_t interruptPin, Angle rotationOffset);

    /**
     * @brief Sets up the class by setting the data pin to input, and the load
     * and clock pin to output, then attaching the interrupt pin to the provided
     * irs function.
     *
     * To use this class properly the provided isr pointer should be just call
     * the isr() method of the same class via a lamda function. This is just to
     * get around an issue with arduino's attachInterrupt.
     *
     * This should look like OBJECT_NAME.setup([]() { OBJECT_NAME.isr();});
     *
     * This isn't the cleanest workaround but works for the intended purpose.
     *
     * @param isrPtr The pointer to the interrupt service routine.
     */
    void setup(voidFuncPtr isrPtr);

    /**
     * @brief Reads the current state of the 8 buttons, as a single byte.
     *
     * @return (uint8_t) The state of each button in the bumper, where the lsb
     * is the value of the front button, then the ascending bits represent the
     * subsequent buttons rotating clockwise.
     */
    uint8_t read();

    /**
     * @brief The interrupt service routine that is called when any of the 8
     * buttons are pressed, responsible for raising the _interruptFlag.
     */
    void isr();

    /**
     * @brief Check if the _interruptFlag has been raised, and then sets the
     * _interruptFlag to false.
     *
     * @return (true) If the _interruptFlag has been raised.
     * @return (false) If the flag has not been raised.
     */
    bool hasFlagBeenRaised();

   private:
    /**
     * @brief The pin connected to the data pin on the shift register.
     */
    uint8_t _dataPin;
    /**
     * @brief The pin connected to the load pin on the shift register.
     */
    uint8_t _loadPin;
    /**
     * @brief The pin connected to the clock pin on the shift register.
     */
    uint8_t _clockPin;
    /**
     * @brief The pin connected to the xor gate that changes when any button is
     * pressed.
     */
    uint8_t _interruptPin;

    /**
     * @brief the flag the indicates if any of the buttons have changed state.
     */
    bool _interruptFlag;

    /**
     * @brief The offset in degrees that the bumper buttons have been rotated
     * around from the position where the front button would be at 0 degrees.
     */
    Angle _rotationOffset;

    /**
     * @brief The number of bits to rotate the raw data by.
     */
    uint8_t _segmentOffset;

    /**
     * @brief Reads the raw data from the shift register.
     *
     * @return (uint8_t) The raw data read from the shift register.
     */
    uint8_t _readRaw();
};

#endif  // BUMPER_H
