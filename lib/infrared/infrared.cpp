/**
 * @file infrared.cpp
 * @brief Definitions of the class responsible for taking readings from the
 * GP2Y0E02B Infrared sensor, while utilizing the PCA9846 multiplexer.
 *
 * The data sheet for the GP2Y0E02B IR sensor can be found here,
 * https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0e02_03_appl_e.pdf.
 *
 *
 * The data sheet for the PCA9846 Multiplexer can be found here,
 * https://www.nxp.com/docs/en/data-sheet/PCA9846.pdf.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2024-02-01
 * @copyright Copyright (c) 2024
 */

#include "infrared.h"

#include <Wire.h>

#include "errorIndicator.h"

// Multiplexer
// https://www.nxp.com/docs/en/data-sheet/PCA9846.pdf
// Multiplexer address, shifted to provide 7-bit version
#define MULTIPLEXER_SLAVE_ADDRESS 0xEE >> 1
#define MULTIPLEXER_CHANNEL_COUNT 4

// Infrared sensor
// Address for IR sensor, shifted to provide 7-bit version
#define IR_SLAVE_ADDRESS 0x80 >> 1

// https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0e02_03_appl_e.pdf#page=16
// Address  | Register Name  | Reg Field    | Default   | R/W   | Description

// 0x35     | Shift Bit      | [2:0]        | 0x02      | R/W   |  0x01 =
// MaximumDisplay 128cm  0x02=Maximum Display 64cm
#define IR_SHIFT_REG_ADDRESS 0x35

// 0x5E     |  Distance[11:4] | [7:0]       | -         | R     |
// 0x5F     |  Distance[ 3:0] | [3:0]       | -         | R     |
// Distance Value =(Distance[11:4]*16+Distance[3:0])/16/2^n n : Shift Bit
// (Register 0x35)
#define IR_DISTANCE_REG_ADDRESS 0x5E

// The number of past values of the distance sensor to store.
#define MAX_HISTORY 10

// The period to wait between taking readings.
#define POLL_PERIOD 10

#define MAX_IR_RANGE 639

/**
 * @brief Construct a new Infrared object from its given index on the
 * multiplexer's bus and the distance from the robots centre.
 *
 * @param index The index of the sensor on the multiplexer.
 * @param distanceFromCentre The distance in millimeters that this sensor is
 * from the centre of the robot.
 */
Infrared::Infrared(uint8_t index, int distanceFromCentre)
    : _index(index),
      _distanceFromCentre(distanceFromCentre),
      _valueHistory(MAX_HISTORY),
      _historyUpdater(POLL_PERIOD) {}

/**
 * @brief Sets up the sensor by setting up the I2C communication.
 */
void Infrared::setup() {
    // Initialise I2C communication.
    Wire.begin();

    // Connect the current sensor to to the I2C bus.
    this->_setMultiplexer();

    // Start the I2C transmission to the slave address.
    Wire.beginTransmission(IR_SLAVE_ADDRESS);

    // Set the sensor to 64cm mode.
    Wire.write(IR_SHIFT_REG_ADDRESS);

    // End the transmission.
    Wire.endTransmission();

    // Request 1 byte from the slave address.
    Wire.requestFrom(IR_SLAVE_ADDRESS, 1);

    // If I2C communication is not available, raise a critical error.
    if (!Wire.available()) {
        String errorMessage =
            "Cannot initialize IR sensor " + String(this->_index) + ".";
        ErrorIndicator_G.errorOccurred(errorMessage);
    }

    // Read in the shift value from the I2C bus.
    this->_shiftValue = Wire.read();
}

/**
 * @brief Read the distance from the sensor.
 *
 * @return (int16_t) The distance read from the sensor, in millimeters.
 * A return value of -1 indicates a reading error, typically reading exceeds
 * the max value.
 */
int16_t Infrared::read() {
    // Connect the current sensor to to the I2C bus.
    this->_setMultiplexer();

    // Start the I2C transmission to the slave address.
    Wire.beginTransmission(IR_SLAVE_ADDRESS);

    // Set the sensor to 64cm mode.
    Wire.write(IR_SHIFT_REG_ADDRESS);

    // End the transmission.
    Wire.endTransmission();

    // Request 2 bytes from the sensor.
    Wire.requestFrom(IR_SLAVE_ADDRESS, 2);

    // If data cannot be read from the sensor, raise a critical error.
    if (Wire.available() < 2) {
        String errorMessage =
            "Cannot read IR sensor " + String(this->_index) + ".";
        ErrorIndicator_G.errorOccurred(errorMessage);
    }

    // read the first byte in as the most significant part
    uint8_t high = Wire.read();
    //  read the second byte in as the least significant part
    uint8_t low = Wire.read();

    // The given formula for calculating distance is:
    // distance(cm) = (high * 16 + low)/16/(int)pow(2,shift);
    // Which can be simplified to:
    // distance(mm) = (((high << 4) | low) * 10) >> (4 + shiftValue);

    uint16_t distance = (((high << 4) | low) * 10) >> (4 + this->_shiftValue);

    // if the distance is out of range, return -1.
    return (distance < MAX_IR_RANGE) ? distance : -1;
}

/**
 * @brief Read the distance from the sensor, but removes anomalous results.
 *
 * @return (int16_t) The distance read from the sensor, in millimeters.
 * A return value of -1 indicates a reading error, typically reading exceeds
 * the max value.
 */
int16_t Infrared::readSafe() {
    // In this context, errors mostly just mean the reading was out of
    // range(exceeded 64cm).

    // How many errors is to many errors.
    const uint8_t errorThreshold = 1;

    // Check how many errors have occurred withing the recorded history of the
    // sensor.
    uint8_t errorCount = this->_valueHistory.countErrors();

    // If few enough errors have occurred, return the most recent reading.
    // If too many errors have occurred, return -1;
    return (errorCount < errorThreshold) ? this->_valueHistory.getLast() : -1;
}

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
int Infrared::readFromRobotCenter(bool getOldReading) {
    int measuredDistance;

    if (getOldReading) {
        measuredDistance = this->_secondMostRecentValue;
    } else {
        measuredDistance = this->_mostRecentValue;
    }

    // If the read value is an error,
    if (measuredDistance == -1) {
        // return an error value.
        return -1;
    }

    // Offset the reading by the sensors distance to the robot's centre.
    int totalDistance = measuredDistance + this->_distanceFromCentre;

    return totalDistance;
}

/**
 * @brief Checks if the robot has seen a starting corner of a brick, based
 * on the rate of change that the sensor has read. A sudden drop in distance
 * indicated that a corner has been seen.
 *
 * @param range The range that the newly seen reading has to be withing to
 * be considered valid.
 * @param requiredDistanceChange The amount of change that must occur for a
 * reading to be considered valid.
 * @return (true) If a starting corner has been seen.
 * @return (false) If a starting corner has not been seen.
 */
bool Infrared::seenStartingCorner(int range, int requiredDistanceChange) {
    int mostRecentValue = this->_mostRecentValue;
    if (mostRecentValue == -1) {
        mostRecentValue = MAX_IR_RANGE;
    }

    int secondMostRecentValue = this->_secondMostRecentValue;
    if (secondMostRecentValue == -1) {
        secondMostRecentValue = MAX_IR_RANGE;
    }

    int distanceChange = mostRecentValue - secondMostRecentValue;

    bool wallInRange = (mostRecentValue < range);
    bool enoughChange = (distanceChange < -(requiredDistanceChange));

    bool brickAppeared = (wallInRange && enoughChange);

    return brickAppeared;
}

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
bool Infrared::seenEndingCorner(int range, int requiredDistanceChange) {
    int mostRecentValue = this->_mostRecentValue;
    if (mostRecentValue == -1) {
        mostRecentValue = MAX_IR_RANGE;
    }

    int secondMostRecentValue = this->_secondMostRecentValue;
    if (secondMostRecentValue == -1) {
        secondMostRecentValue = MAX_IR_RANGE;
    }

    int distanceChange = mostRecentValue - secondMostRecentValue;

    bool wallWasInRange = (secondMostRecentValue < range);
    bool enoughChange = (distanceChange > requiredDistanceChange);

    bool brickDisappeared = (wallWasInRange && enoughChange);

    return brickDisappeared;
}

/**
 * @brief Returns the median value from the reading history, this removed
 * anomalous values, at the cost of introducing latency.
 *
 * @return (int16_t) The median value from the reading history.
 * A return value of -1 indicates a reading error, typically reading exceeds
 * the max value.
 */
int16_t Infrared::average() { return this->_valueHistory.getMedian(); }

/**
 * @brief Updates the value history, if enough time has passed.
 */
void Infrared::poll() {
    // If enough time has passed since this function last ran,
    if (this->_historyUpdater.isReadyToRun()) {
        // add a new reading to the value history.
        this->_valueHistory.add(this->read());

        // Update the most recent and second most recent values.
        this->_secondMostRecentValue = this->_mostRecentValue;
        this->_mostRecentValue = this->readSafe();
    }
}

/**
 * @brief Connects this sensor the the I2C bus by writing its index to the
 * multiplexer.
 */
void Infrared::_setMultiplexer() {
    // Connect to the multiplexer via I2C.
    Wire.beginTransmission(MULTIPLEXER_SLAVE_ADDRESS);
    // Write the index of this sensor to the multiplexer.
    Wire.write(1 << this->_index);
    // End the transmission.
    Wire.endTransmission();
}
