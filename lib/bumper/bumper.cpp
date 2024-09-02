/**
 * @file bumper.cpp
 * @brief Definitons of the Bumper class, responsible for reading the state
 * of the 8 buttons that make up the bumper around the circumference of the
 * robot.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */

#include "bumper.h"

#include <Arduino.h>

#include "binary.h"

// The number of buttons that make up the bumper.
#define BUMPER_BUTTON_COUNT 8

/**
 * @brief Construct a new Bumper object, then sets the data pin to input,
 * and the load and clock pin to output.
 *
 * @param dataPin The pin connected to the data pin on the shift register.
 * @param loadPin The pin connected to the load pin on the shift register.
 * @param clockPin The pin connected to the clock pin on the shift register.
 * @param bitOffset The number of bits that the data should be rotated by.
 */
Bumper::Bumper(unsigned char dataPin, unsigned char loadPin,
               unsigned char clockPin, unsigned char bitOffset)
    : _dataPin(dataPin),
      _loadPin(loadPin),
      _clockPin(clockPin),
      _bitOffset(bitOffset) {
    pinMode(this->_dataPin, INPUT);
    pinMode(this->_loadPin, OUTPUT);
    pinMode(this->_clockPin, OUTPUT);
}

/**
 * @brief Reads the current state of the 8 buttons, as a single byte.
 *
 * @return (unsigned char). The state of each button in the bumper, where the
 * lsb is the value of the front button, then the ascending bits represent the
 * subsequent buttons rotating clockwise
 */
unsigned char Bumper::read() {
    // Gets the raw data from the shift register
    unsigned char rawData = this->_readRaw();

    // Rotates the data so that the value of the front button is the lsb of the
    // byte.
    unsigned char rotatedData = rotateLeft(rawData, this->_bitOffset);

    return rotatedData;
}

/**
 * @brief Reads the raw data from the shift register.
 *
 * @return (unsigned char) The raw data read from the shift register.
 */
unsigned char Bumper::_readRaw() {
    // Create a byte for storing the received data from the shift register.
    unsigned char shiftRegisterContents = 0;

    // Pulse the load pin to load the button states into the shift
    // register.
    digitalWrite(this->_loadPin, LOW);
    delayMicroseconds(1);
    digitalWrite(this->_loadPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->_loadPin, LOW);

    // For each button,
    for (int i = 0; i < BUMPER_BUTTON_COUNT; i++) {
        // Shift the received data up by one.
        shiftRegisterContents <<= 1;

        // Load the available bit from the shift register into the lsb of the
        // received byte.
        shiftRegisterContents |= digitalRead(this->_dataPin);

        // Pulse the clock pin to shift the data inside the shift register.
        digitalWrite(this->_clockPin, LOW);
        delayMicroseconds(1);
        digitalWrite(this->_clockPin, HIGH);
        delayMicroseconds(1);
    }
    return shiftRegisterContents;
}