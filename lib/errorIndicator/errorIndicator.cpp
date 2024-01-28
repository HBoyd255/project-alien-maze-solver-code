/**
 * @file errorIndicator.cpp
 * @brief DDefinitions for the ErrorIndicator class, responsible for halting
 * operations upon an error, and drawing the users attention to the serial
 * monitor.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#include "errorIndicator.h"

// The period to wait while flashing the leds
#define ERROR_INDICATOR_DELAY 500

/**
 * @brief Construct a new Error Indicator object
 *
 * @param ledPin (uint8_t) The pin of the LED to flash upon error, typically
 * the built in led.
 * @param serialBaudRate (uint32_t) the baud rate of the serial monitor, so
 * that the class can reopen the serial monitor if needed.
 */
ErrorIndicator::ErrorIndicator(uint8_t ledPin, uint32_t serialBaudRate)
    : _ledPin(ledPin), _serialBaudRate(serialBaudRate) {}

/**
 * @brief Sets up the class by setting the led pin to output.
 */
void ErrorIndicator::setup() { pinMode(this->_ledPin, OUTPUT); }

/**
 * @brief Assigns a Pixels object to pointer the class, used for flashing
 * the lights to grab the users attention.
 *
 * @param pixels_P (Pixels*) A pointer to a preexisting Pixels class.
 */
void ErrorIndicator::assignPixels(Pixels* pixels_P) {
    this->_pixels_P = pixels_P;
}

/**
 * @brief Assigns a Pixels object pointer to the class, used for halting the
 * motors when an error occurs.
 *
 * @param drive_P (Drive*) A pointer to a preexisting Drive class.
 */
void ErrorIndicator::assignDrive(Drive* drive_P
) {
    this->_drive_P = drive_P;
}

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
void ErrorIndicator::errorOccurred(String errorMessage) {
    // If a pointer to the drive class has been given, halt the motors.
    if (this->_drive_P != NULL) {
        this->_drive_P->stop();
    }

    // Incase serial communication has not been initialized.
    Serial.begin(this->_serialBaudRate);

    // A boolean representing if the serial motor is connected.
    bool serialAvailability = (bool)Serial;
    // A boolean representing is last state of the serialAvailability boolean,
    // used for detecting the point at which the serial monitor becomes
    // available.
    bool oldSerialAvailability = serialAvailability;

    // A boolean representing if the pixels class has been provided.
    bool pixelsAvailability = this->_pixels_P != NULL;

    // A boolean used for flipping the LED on and off.
    bool ledToggle = false;

    // This loop will not end until the program is reset.
    while (true) {
        // If the pixels class has been provided and the serial monitor is
        // unavailable.
        if (pixelsAvailability && !serialAvailability) {
            // Clear the LEDs.
            this->_pixels_P->clear();

            // Get the number of LEDs that the Pixels class represents.
            uint16_t ledCount = this->_pixels_P->getLedCount();

            // Set every other pixel to bright red, offset by the ledToggle
            // boolean.
            for (int i = 0; i < (ledCount / 2); i++) {
                this->_pixels_P->setPixel((i * 2) + ledToggle, 255, 0, 0);
            }
            // Show all the LEDs.
            this->_pixels_P->show();

            // If the pixels class has been provided and the serial monitor is
            // available, then turn off all the Pixels.
        } else if (pixelsAvailability && serialAvailability) {
            _pixels_P->clear(true);
        }

        // if the Serial monitor has only became available since the last loop.
        if (serialAvailability && !oldSerialAvailability) {
            // Display the error message to the serial monitor.
            Serial.println("Error Occurred!");
            Serial.println(errorMessage);
        }

        // Toggle the LED, regardless of if the serial monitor is available.
        digitalWrite(this->_ledPin, ledToggle);

        // Wait for a predefined period.
        delay(ERROR_INDICATOR_DELAY);

        // Toggle the ledToggle boolean.
        ledToggle = !ledToggle;

        // Check if the serial monitor has became available.
        oldSerialAvailability = serialAvailability;
        serialAvailability = (bool)Serial;
    }
}
