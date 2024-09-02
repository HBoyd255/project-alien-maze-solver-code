/**
 * @file bumper.h
 * @brief Declaration of the Bumper class, responsible for reading the state of
 * the 8 buttons that make up the bumper around the circumference of the robot.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */

#ifndef BUMPER_H
#define BUMPER_H

/**
 * @brief The Bumper class, responsible for reading the state of the 8 buttons
 * that make up the bumper around the circumference of the robot.
 *
 */
class Bumper {
   public:
    /**
     * @brief Construct a new Bumper object, then sets the data pin to input,
     * and the load and clock pin to output.
     *
     * @param dataPin The pin connected to the data pin on the shift register.
     * @param loadPin The pin connected to the load pin on the shift register.
     * @param clockPin The pin connected to the clock pin on the shift register.
     * @param bitOffset The number of bits that the data should be rotated by.
     */
    Bumper(unsigned char dataPin, unsigned char loadPin, unsigned char clockPin,
           unsigned char bitOffset);
           
    /**
     * @brief Reads the current state of the 8 buttons, as a single byte.
     *
     * @return (unsigned char). The state of each button in the bumper, where
     * the lsb is the value of the front button, then the ascending bits
     * represent the subsequent buttons rotating clockwise.
     */
    unsigned char read();

   private:
    /**
     * @brief The pin connected to the data pin on the shift register.
     */
    unsigned char _dataPin;
    /**
     * @brief The pin connected to the load pin on the shift register.
     */
    unsigned char _loadPin;
    /**
     * @brief The pin connected to the clock pin on the shift register.
     */
    unsigned char _clockPin;

    /**
     * @brief The number of bits that the data should be rotated by.
     */
    unsigned char _bitOffset;

    /**
     * @brief Reads the raw data from the shift register.
     *
     * @return (unsigned char) The raw data read from the shift register.
     */
    unsigned char _readRaw();
};

#endif  // BUMPER_H
