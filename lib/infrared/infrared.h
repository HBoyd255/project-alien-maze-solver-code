/**
 * @file infrared.h
 * @brief Declaration of the class responsible for taking readings from the
 * GP2Y0E02B Infrared sensor, while utilizing the PCA9846 multiplexer.
 *
 * The data sheet for the GP2Y0E02B IR sensor can be found here,
 * https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0e02_03_appl_e.pdf.
 *
 *
 * The data sheet for the PCA9846 Multiplexer can be found here,
 * https://www.nxp.com/docs/en/data-sheet/PCA9846.pdf.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-02-01
 * @copyright Copyright (c) 2024
 */
#ifndef INFRARED_H
#define INFRARED_H

#include <Arduino.h>

#include "history.h"
#include "schedule.h"

/**
 * @brief Responsible for reading valued from the GP2Y0E02B Infrared sensor,
 * and controlling the PCA9846 I2C multiplexer that connect the sensor to the
 * bus.
 *
 * Sensor data sheet -
 * https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0e02_03_appl_e.pdf
 *
 * Multiplexer data sheet - https://www.nxp.com/docs/en/data-sheet/PCA9846.pdf
 */
class Infrared {
   public:
    /**
     * @brief Construct a new Infrared object from its given index on the
     * multiplexer's bus and the distance from the robots centre.
     *
     * @param index The index of the sensor on the multiplexer.
     * @param distanceFromCentre The distance in millimeters that this sensor is
     * from the centre of the robot.
     */
    Infrared(uint8_t index, int distanceFromCentre);

    /**
     * @brief Sets up the sensor by setting up the I2C communication.
     */
    void setup();

    /**
     * @brief Read the distance from the sensor.
     *
     * @return (int16_t) The distance read from the sensor, in millimeters.
     * A return value of -1 indicates a reading error, typically reading exceeds
     * the max value.
     */
    int16_t read();

    /**
     * @brief Read the distance from the sensor, but removes anomalous results.
     *
     * @return (int16_t) The distance read from the sensor, in millimeters.
     * A return value of -1 indicates a reading error, typically reading exceeds
     * the max value.
     */
    int16_t readSafe();

    /**
     * @brief Measures the distance from the centre of the robot to the
     * object seen by the sensor.
     *
     * @param getOldReading Whether to get the most recent or second most recent
     * reading, this functionality makes it a lot easier to measure the rate of
     * change of the sensor.
     * @return (int) The distance from the centre of the robot to the object
     * seen by the sensor.
     */
    int readFromRobotCenter(bool getOldReading = false);

    /**
     * @brief Checks if the robot has seen a starting corner of a brick, based
     * on the rate of change that the sensor has read. A sudden drop in distance
     * indicated that a corner has been seen.
     *
     * @param range The range that the newly seen reading has to be withing to
     * be considered valid.
     * @param requiredDistanceChange The amount the value must have dropped by
     * to be considered valid.
     * @return (true) If a starting corner has been seen.
     * @return (false) If a starting corner has not been seen.
     */
    bool seenStartingCorner(int range, int requiredDistanceChange);

    /**
     * @brief Checks if the robot has seen an ending corner of a brick, based
     * on the rate of change that the sensor has read. A sudden rise in distance
     * indicated that a ending corner has been seen.
     *
     * @param range The range that the seen reading bust have been before
     * rising.
     * @param requiredDistanceChange The amount the value must have rose by
     * to be considered valid.
     * @return (true) If a ending corner has been seen.
     * @return (false) If a ending corner has not been seen.
     */
    bool seenEndingCorner(int range, int requiredDistanceChange);

    /**
     * @brief Returns the median value from the reading history, this removed
     * anomalous values, at the cost of introducing latency.
     *
     * @return (int16_t) The median value from the reading history.
     * A return value of -1 indicates a reading error, typically reading exceeds
     * the max value.
     */
    int16_t average();

    /**
     * @brief Updates the value history, if enough time has passed.
     */
    void poll();

   private:
    /**
     * @brief The index of this sensor on the Multiplexer's bus.
     */
    uint8_t _index;

    /**
     * @brief The distance of this sensor to the centre of the robot.
     */
    int _distanceFromCentre;

    /**
     * @brief An instance of the History class to capture a list of the most
     * recent values from the sensor.
     */
    History _valueHistory;

    /**
     * @brief An instance of the PassiveSchedule class to keep track of how
    often the valueHistory should be updated.
     */
    PassiveSchedule _historyUpdater;

    /**
     * @brief The shift value of the infrared sensor.
     */
    uint8_t _shiftValue;

    /**
     * @brief The most recent reading from the readSafe() function.
     */
    int _mostRecentValue = -1;

    /**
     * @brief The second most recent reading from the readSafe() function.
     */
    int _secondMostRecentValue = -1;

    /**
     * @brief Connects this sensor the the I2C bus by writing its index to the
     * multiplexer.
     */
    void _setMultiplexer();
};

#endif  // INFRARED_H