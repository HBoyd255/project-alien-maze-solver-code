
#ifndef INFRARED_H
#define INFRARED_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @class Infrared
 * @brief Class for reading the data from an infrared sensor.
 *
 * The Infrared class provides methods for setting up and reading the data from
 *  an infrared sensor.
 *
 *
 */
class Infrared {
   public:
    /**
     * @brief Constructor for the Infrared class.
     *
     * @param index The index of the infrared sensor.
     */
    Infrared(uint8_t index);
    
    
    
    /**
     * @brief Initializes the infrared sensor.
     *
     * This function sets up the necessary configurations and parameters for the
     * infrared sensor. It should be called once during the setup phase of the
     * program.
     */
    void setup();
    /**
     * @brief Reads the distance from the infrared sensor in millimeters.
     *
     * @return The distance in millimeters.
     */
    uint16_t read();

   private:
    uint8_t index;        // The index of the infrared sensor
    uint8_t shiftValue;  // The shift value of the infrared sensor
    /**
     * @brief Grabs the I2C bus multiplexer.
     *
     * This function grabs the I2C bus multiplexer and sets it to the channel of
     * the infrared sensor.
     */
    void grabMultiplexer();
};

#endif  // INFRARED_H