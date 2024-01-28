/**
 * @file bumper.cpp
 * @brief Definitons of the Bumper class, responsible for reading the state
 * of the 8 buttons that make up the bumper around the circumference of the
 * robot.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */
#include "bumper.h"

#include <Arduino.h>

#include "binary.h"

// The number of buttons that make up the bumper
#define BUMPER_BUTTON_COUNT 8

// The angle that separates each button.
#define DEGREES_PER_BUMPER DEGREES_PER_ROTATION / BUMPER_BUTTON_COUNT;

/**
 * @brief Construct a new Bumper object.
 *
 * @param dataPin The pin connected to the data pin on the shift register.
 * @param loadPin The pin connected to the load pin on the shift register.
 * @param clockPin The pin connected to the clock pin on the shift register.
 * @param interruptPin The pin connected to the xor gate that changes when
 * any button is pressed.
 * @param angleOffset  The offset in degrees that the bumper buttons have
 * been rotated around from the position where the front button would be at
 * 0 degrees.
 */
Bumper::Bumper(uint8_t dataPin, uint8_t loadPin, uint8_t clockPin,
               uint8_t interruptPin, Angle rotationOffset)
    : _dataPin(dataPin),
      _loadPin(loadPin),
      _clockPin(clockPin),
      _interruptPin(interruptPin),
      _rotationOffset(rotationOffset),
      _segmentOffset(rotationOffset.segmentIndex(BUMPER_BUTTON_COUNT)) {}

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
 * @param isr_P The pointer to the interrupt service routine.
 */
void Bumper::setup(voidFuncPtr isr_P) {
    pinMode(this->_dataPin, INPUT);
    pinMode(this->_loadPin, OUTPUT);
    pinMode(this->_clockPin, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(this->_interruptPin), isr_P, CHANGE);
}

/**
 * @brief Reads the current state of the 8 buttons, as a single byte.
 *
 * @return (uint8_t) The state of each button in the bumper, where the lsb
 * is the value of the front button, then the ascending bits represent the
 * subsequent buttons rotating clockwise.
 */
uint8_t Bumper::read() {
    // Gets the raw data from the shift register
    uint8_t rawData = this->_readRaw();

    // Rotates the data so that the value of the front button is the lsb of the
    // byte.
    uint8_t rotatedData = rotateLeft(rawData, this->_segmentOffset);

    return rotatedData;
}

/**
 * @brief The interrupt service routine that is called when any of the 8
 * buttons are pressed, responsible for raising the _interruptFlag.
 */
void Bumper::isr() {
    this->_interruptFlag = true;
    // TODO add a way to stop the motors instantly when a button has been
    // pressed.
}

/**
 * @brief Check if the _interruptFlag has been raised, and then sets the
 * _interruptFlag to false.
 *
 * @return (true) If the _interruptFlag has been raised.
 * @return (false) If the flag has not been raised.
 */
bool Bumper::hasFlagBeenRaised() {
    if (this->_interruptFlag) {
        this->_interruptFlag = false;
        return true;
    }
    return false;
}

/**
 * @brief Reads the raw data from the shift register.
 *
 * @return (uint8_t) The raw data read from the shift register.
 */
uint8_t Bumper::_readRaw() {
    // Create a byte for storing the received data from the shift register.
    uint8_t shiftRegisterContents = 0;

    // Pulse the load pin to load the button states into the shift
    // register.
    digitalWrite(this->_loadPin, LOW);
    delayMicroseconds(1);
    digitalWrite(this->_loadPin, HIGH);
    delayMicroseconds(1);

    // For each button,
    for (uint8_t i = 0; i < BUMPER_BUTTON_COUNT; i++) {
        // Shift the received data up by one.
        shiftRegisterContents <<= 1;

        // Load the available bit from the shift register into the lsb of the
        // received data byte.
        shiftRegisterContents |= digitalRead(this->_dataPin);

        // Pulse the clock pin to shift the data inside the shift register.
        digitalWrite(this->_clockPin, LOW);
        delayMicroseconds(1);
        digitalWrite(this->_clockPin, HIGH);
        delayMicroseconds(1);
    }
    return shiftRegisterContents;
}