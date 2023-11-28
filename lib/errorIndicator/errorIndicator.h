#ifndef ERROR_INDICATOR_H
#define ERROR_INDICATOR_H

#include <Arduino.h>

#include "drive.h"
#include "pixels.h"

/**
 * @brief The ErrorIndicator class represents an error indicator module.
 *
 * This class is responsible for setting up the error indicator,
 * setting the pixels and drive pointers, and indicating when an error occurs.
 */
class ErrorIndicator {
   public:
    /**
     * @brief Default constructor for the ErrorIndicator class.
     */
    ErrorIndicator(){};

    /**
     * @brief Sets up the error indicator.
     */
    void setup();

    /**
     * @brief Sets the pixels pointer.
     *
     * @param pixelsPtr A pointer to the Pixels object.
     */
    void assignPixels(Pixels* pixelsPtr);

    /**
     * @brief Sets the drive pointer.
     *
     * @param drivePtr A pointer to the Drive object.
     */
    void assignDrive(Drive* drivePtr);

    /**
     * @brief Indicates that an error has occurred.
     *
     * @param errorMessage The error message to be displayed to.
     */
    void errorOccurred(String errorMessage);

   private:
    Pixels* pixelsPtr = NULL;  // Pointer to the Pixels object.
    Drive* drivePtr = NULL;    // Pointer to the Drive object.
};

#endif  // ERROR_INDICATOR_H