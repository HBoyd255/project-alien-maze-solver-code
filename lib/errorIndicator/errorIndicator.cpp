/**
 * @file errorIndicator.cpp
 * @brief Definitions for the ErrorIndicator class, responsible for halting
 * operations upon an error, and drawing the users attention to the serial
 * monitor.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#include "errorIndicator.h"

/**
 * @brief Construct a new Error Indicator object
 */
ErrorIndicator::ErrorIndicator() : _hasBegun(false) {}

/**
 * @brief Initialises the Error Indicator object, by setting the pin number
 * of led to flash and the baud rate of the serial monitor.
 *
 * @param serialBaudRate The baud rate of the serial monitor.
 * @param ledPin The pin number of led to flash.
 */
void ErrorIndicator::begin(uint32_t serialBaudRate, uint8_t ledPin) {
    this->_serialBaudRate = serialBaudRate;
    this->_ledPin = ledPin;

    // Set the has begun flag to true.
    this->_hasBegun = true;
}

/**
 * @brief Adds a function to call when an error occurs, to stop ongoing
 * operations, such as stopping the motors.
 *
 * @param haltCallback_P A pointer to the function to call when an error
 * occurs.
 */
void ErrorIndicator::addHaltCallback(voidFuncPtr haltCallback_P) {
    this->_haltCallback_P = haltCallback_P;
}

/**
 * @brief Adds a function to call when an error occurs, to draw the users
 * attention to the serial monitor. This will be ran constantly, until the
 * user has opened the serial monitor.
 *
 * @param drawAttentionCallback_P A pointer to the function to call when an
 * error occurs and the serial monitor is closed.
 */
void ErrorIndicator::addDrawAttentionCallback(
    voidFuncPtr drawAttentionCallback_P) {
    this->_drawAttentionCallback_P = drawAttentionCallback_P;
}

/**
 * @brief Adds a function to call after an error has occurred, when the user
 * has opened the serial monitor.
 *
 * @param attentionDrawnCallback_P A pointer to the function to call after
 * an error has occurred and the user has opened the serial monitor.
 */
void ErrorIndicator::addAttentionDrawnCallback_P(
    voidFuncPtr attentionDrawnCallback_P) {
    this->_attentionDrawnCallback_P = attentionDrawnCallback_P;
}

/**
 * @brief Permanently stops the program, halts ongoing operations, and
 * attemps to draw the users attention to the serial monitor.
 *
 * This function can only be escaped by reseting the program.
 *
 * @param errorMessage The error message to display to the serial
 * monitor.
 */
void ErrorIndicator::errorOccurred(String file, int line, String errorMessage) {
    // If the halt function has been provided, call to stop and critical
    // operation.
    if (this->_haltCallback_P != NULL) {
        this->_haltCallback_P();
    }

    // If this instance has not been initialised using ErrorIndicator::begin,
    // hope that the serial Port has been initialised and spam the received
    // error message to it. This case should be avoided if possible.
    while (!this->_hasBegun) {
        Serial.println(
            "Make sure to call ErrorIndicator::begin before using "
            "ErrorIndicator::errorOccurred.");
        this->_printError(file, line, errorMessage);
    }

    // Begin the serial monitor and set the LED pin to output, this method of
    // initialising the serial monitor and LED pin inside this instance is
    // typically bad practice, as it can interfere with other modules that may
    // be using these resources. However, as this function a last resort,
    // and all other operations have been halted, this function takes priority.
    Serial.begin(this->_serialBaudRate);
    pinMode(this->_ledPin, OUTPUT);

    // A boolean representing if the serial motor is available.
    bool serialAvailable = false;

    // A boolean representing if the serial motor was available in the previous
    // loop.
    bool oldSerialAvailability = false;

    // A boolean representing if the serial motor was connected since the last
    // loop.
    bool serialNewlyAvailable = false;

    while (true) {
        // Flash the LED each second.
        digitalWrite(this->_ledPin, millis() / 500 % 2);

        // Update the availability status of the serial monitor.
        serialAvailable = (bool)Serial;
        serialNewlyAvailable = serialAvailable && !oldSerialAvailability;
        oldSerialAvailability = serialAvailable;

        // If the serial monitor is unavailable, attempt to grab the users
        // attention.
        if (!serialAvailable) {
            // If the grab attention function has been provided, call it.
            if (this->_drawAttentionCallback_P != NULL) {
                this->_drawAttentionCallback_P();
            };
        } else if (serialNewlyAvailable) {
            // If the has attention function has been provided, call it.
            if (this->_attentionDrawnCallback_P != NULL) {
                this->_attentionDrawnCallback_P();
            };

            this->_printError(file, line, errorMessage);
        }
    }
}

/**
 * @brief Prints the error message to the serial monitor.
 *
 * @param file The file in which the error occurred.
 * @param line The line number at which the error occurred.
 * @param errorMessage The error message to display to the serial monitor.
 */
void ErrorIndicator::_printError(String file, int line, String errorMessage) {
    Serial.println(
        "----------------------------------------"
        "----------------------------------------");
    Serial.println("Error Occurred at " + file + ":" + String(line) + ":");
    Serial.println(errorMessage);
    Serial.println("Please address this issue and reset the program.");
    Serial.println();
}

// Definition of the global ErrorIndicator instance.
ErrorIndicator ErrorIndicator_G;
