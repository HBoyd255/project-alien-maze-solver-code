/**
 * @file errorIndicator.h
 * @brief Declaration for the ErrorIndicator class, responsible for halting
 * operations upon an error, and drawing the users attention to the serial
 * monitor.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#ifndef ERROR_INDICATOR_H
#define ERROR_INDICATOR_H

#include <Arduino.h>

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
     * @param serialBaudRate The baud rate of the serial monitor.
     * @param ledPin The pin number of led to flash.
     */
    void begin(uint32_t serialBaudRate, uint8_t ledPin);

    /**
     * @brief Adds a function to call when an error occurs, to stop ongoing
     * operations, such as stopping the motors.
     *
     * @param haltCallback_P A pointer to the function to call when an error
     * occurs.
     */
    void addHaltCallback(voidFuncPtr haltCallback_P);

    /**
     * @brief Adds a function to call when an error occurs, to draw the users
     * attention to the serial monitor. This will be ran constantly, until the
     * user has opened the serial monitor.
     *
     * @param drawAttentionCallback_P A pointer to the function to call when an
     * error occurs and the serial monitor is closed.
     */
    void addDrawAttentionCallback(voidFuncPtr drawAttentionCallback_P);

    /**
     * @brief Adds a function to call after an error has occurred, when the user
     * has opened the serial monitor.
     *
     * @param attentionDrawnCallback_P A pointer to the function to call after
     * an error has occurred and the user has opened the serial monitor.
     */
    void addAttentionDrawnCallback_P(voidFuncPtr attentionDrawnCallback_P);

    /**
     * @brief Permanently stops the program, halts ongoing operations, and
     * attemps to draw the users attention to the serial monitor.
     *
     * This function can only be escaped by reseting the program.
     *
     * @param errorMessage The error message to display to the serial
     * monitor.
     */
    void errorOccurred(String file, int line, String errorMessage);

   private:
    /**
     * @brief A boolean to keep track of wether this instance has been
     * initialised uring ErrorIndicator::begin().
     */
    bool _hasBegun;

    /**
     * @brief The baud rate of the serial monitor.
     */
    uint32_t _serialBaudRate;

    /**
     * @brief The  pin of the LED to flash upon error.
     */
    uint8_t _ledPin;

    /**
     * @brief A pointer to the function to call when an error occurs.
     */
    voidFuncPtr _haltCallback_P = NULL;

    /**
     * @brief A pointer to the function to call when an error occurs and the
     * serial monitor is closed.
     */
    voidFuncPtr _drawAttentionCallback_P = NULL;

    /**
     * @brief A pointer to the function to call after an error has occurred and
     * the user has opened the serial monitor.
     */
    voidFuncPtr _attentionDrawnCallback_P = NULL;

    /**
     * @brief Prints the error message to the serial monitor.
     *
     * @param file The file in which the error occurred.
     * @param line The line number at which the error occurred.
     * @param errorMessage The error message to display to the serial monitor.
     */
    void _printError(String file, int line, String errorMessage);
};

// Declaration of the global ErrorIndicator instance.
extern ErrorIndicator ErrorIndicator_G;

#endif  // ERROR_INDICATOR_H