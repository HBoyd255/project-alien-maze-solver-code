

#include "errorIndicator.h"

#define ERROR_INDICATOR_DELAY 500

/**
 * @brief Sets up the error indicator.
 *
 * This function initializes the error indicator by setting the pin mode of the
 * built-in LED to OUTPUT.
 */
void ErrorIndicator::setup() { pinMode(LED_BUILTIN, OUTPUT); }

/**
 * @brief Sets the pointer to the Pixels object used by the ErrorIndicator.
 *
 * @param pixelsPtr A pointer to the Pixels object.
 */
void ErrorIndicator::assignPixels(Pixels* pixelsPtr) {
    this->pixelsPtr = pixelsPtr;
}
/**
 * @brief Sets the drive pointer for the ErrorIndicator.
 *
 * @param drivePtr A pointer to the Drive object.
 */
void ErrorIndicator::assignDrive(Drive* drivePtr) { this->drivePtr = drivePtr; }

/**
 * @brief Handles the occurrence of an error and displays an error message.
 *
 * This function will display an error message on the serial monitor and flash
 * the built-in LED is the serial monitor has not been connected.
 *
 * if an instance of the drive class has been provided, the robot will be
 * immobilized.
 *
 * If an instance of the pixels class has been provided, the pixels will be used
 * to flash their LEDs.
 *
 * @param errorMessage The error message to be displayed.
 */
void ErrorIndicator::errorOccurred(String errorMessage) {
    // If an instance of the drive class has been provided, immobilize the
    // robot.
    if (this->drivePtr != NULL) {
        this->drivePtr->stop();
    }

    // Two booleans to keep track of whether the serial monitor is available
    // and whether it was available in the previous iteration of the loop.
    bool serialAvailability = (bool)Serial;
    bool oldSerialAvailability = serialAvailability;

    // Boolean to keep track of whether the pixels class has been provided.
    bool pixelsAvailability = this->pixelsPtr != NULL;

    // Initialize the serial monitor if it is available.
    Serial.begin(SERIAL_BAUD_RATE);

    // Boolean to keep track of whether the LED should be on or off.
    bool ledToggle = false;

    // Loop forever
    while (true) {
        // If the pixels class has been provided, and the serial monitor is
        // unavailable, alternate flashing the odd and even LEDs.
        if (pixelsAvailability && !serialAvailability) {
            this->pixelsPtr->clear();
            for (int i = 0; i < 8; i++) {
                this->pixelsPtr->setPixel((i * 2) + ledToggle, 255, 0, 0);
            }
            this->pixelsPtr->show();

            // If the pixels class has been provided, and the serial monitor is
            // available, clear the pixels.
        } else if (pixelsAvailability && serialAvailability) {
            pixelsPtr->clear(true);
        }

        // If the serial monitor has just been connected, print the error
        // message
        if (serialAvailability && !oldSerialAvailability) {
            Serial.println("Error Occurred!");
            Serial.println(errorMessage);
        }

        // Toggle the built-in LED regardless of whether the serial monitor is
        // available.
        digitalWrite(LED_BUILTIN, ledToggle);

        // Delay for a predetermined amount of time.
        delay(ERROR_INDICATOR_DELAY);

        // Toggle the boolean that determines whether the LED should be on or
        // off.
        ledToggle = !ledToggle;

        // Update the booleans that keeps track of whether the serial monitor is
        // available.
        oldSerialAvailability = serialAvailability;
        serialAvailability = (bool)Serial;
    }
}
