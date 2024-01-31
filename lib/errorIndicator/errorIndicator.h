/**
 * @file errorIndicator.h
 * @brief Declaration for the ErrorIndicator class, responsible for halting
 * operations upon an error, and drawing the users attention to the serial
 * monitor.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#ifndef ERROR_INDICATOR_H
#define ERROR_INDICATOR_H

#include <Arduino.h>

#include "drive.h"
#include "pixels.h"

/**
 * @brief ErrorIndicator class, responsible for halting operations upon an
 * error, and drawing the user's attention to the serial monitor.
 *
 * This class should be used by making calls to the global ErrorIndicator_G
 * instance.
 */
class ErrorIndicator {
   public:
    /**
     * @brief Construct a new Error Indicator object
     */
    ErrorIndicator();

    /**
     * @brief Initialises the Error Indicator object, by setting the pin number
     * of led to flash and the baud rate of the serial monitor.
     *
     * @param ledPin The pin number of led to flash.
     * @param serialBaudRate The baud rate of the serial monitor.
     */
    void begin(uint8_t ledPin, uint32_t serialBaudRate);

    /**
     * @brief Assigns a Pixels object to pointer the class, used for
     * flashing the lights to grab the users attention.
     *
     * @param pixels_P (Pixels*) A pointer to a preexisting Pixels class.
     */
    void assignPixels(Pixels* pixels_P);

    /**
     * @brief Assigns a Pixels object pointer to the class, used for halting the
     * motors when an error occurs.
     *
     * @param drive_P (Drive*) A pointer to a preexisting Drive class.
     */
    void assignDrive(Drive* drive_P);

    /**
     * @brief Permanently stops the program, flashes the given LED, flashes the
     * Pixels LEDs(if available), stops the motors(if available) and prints and
     * error message to the serial monitor.
     *
     * This can only be escaped by reseting the program.
     *
     * @param errorMessage (String) The error message to display to the serial
     * monitor.
     */
    void errorOccurred(String errorMessage);

   private:
    /**
     * @brief A boolean to keep track of wether this instance has been
     * initialised uring ErrorIndicator::begin().
     */
    bool _hasBegun;

    /**
     * @brief The  pin of the LED to flash upon error.
     */
    uint8_t _ledPin;
    /**
     * @brief The baud rate of the serial monitor.
     */
    uint32_t _serialBaudRate;
    /**
     * @brief A pointer to a preexisting Pixels class.
     */
    Pixels* _pixels_P = NULL;
    /**
     * @brief  A pointer to a preexisting Drive class.
     */
    Drive* _drive_P = NULL;
};

// Declaration of the global ErrorIndicator instance.
extern ErrorIndicator ErrorIndicator_G;

#endif  // ERROR_INDICATOR_H