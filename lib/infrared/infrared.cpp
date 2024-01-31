

#include "infrared.h"
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

Infrared::Infrared(uint8_t index, int distanceFromCentre)
    : _index(index),
      _valueHistory(MAX_HISTORY),
      _historyUpdater(POLL_PERIOD),
      _distanceFromCentre(distanceFromCentre) {}

void Infrared::setup() {
    Wire.begin();

    this->_setMultiplexer();

    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_SHIFT_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 1);

    if (!Wire.available()) {
        String errorMessage =
            "Cannot initialize IR sensor " + String(this->_index) + ".";
        ErrorIndicator_G.errorOccurred(errorMessage);
    }
    this->_shiftValue = Wire.read();
}

int16_t Infrared::read() {
    this->_setMultiplexer();

    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_DISTANCE_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 2);

    // TODO add a timeout and a call to the errorIndicator.
    if (Wire.available() < 2) {
        String errorMessage =
            "Cannot read IR sensor " + String(this->_index) + ".";
        ErrorIndicator_G.errorOccurred(errorMessage);
    }

    uint8_t high = Wire.read();
    uint8_t low = Wire.read();

    // The given formula for calculating distance is:
    // distance(cm) = (high * 16 + low)/16/(int)pow(2,shift);
    // Which can be simplified to:
    // distance(mm) = (((high << 4) | low) * 10) >> (4 + shiftValue);

    uint16_t distance = (((high << 4) | low) * 10) >> (4 + this->_shiftValue);

    return (distance < MAX_IR_RANGE) ? distance : -1;
}

int16_t Infrared::readSafe() {
    const uint8_t errorThreshold = 1;

    uint8_t errorCount = this->_valueHistory.countErrors();

    return (errorCount < errorThreshold) ? this->_valueHistory.getLast() : -1;
}

int Infrared::readFromRobotCenter(bool getOldReading) {
    int measuredDistance;

    if (getOldReading) {
        measuredDistance = this->_secondMostRecentValue;
    } else {
        measuredDistance = this->_mostRecentValue;
    }

    if (measuredDistance == -1) {
        return -1;
    }

    int totalDistance = measuredDistance + this->_distanceFromCentre;

    // Serial.print(" For a total of:");
    // Serial.print(totalDistance);

    return totalDistance;
}

bool Infrared::brickAppeared(int range, int requiredDistanceChange) {
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

    bool justAppeared = (wallInRange && enoughChange);

    return justAppeared;
}

bool Infrared::brickDisappeared(int range, int requiredDistanceChange) {
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

    bool justDisappeared = (wallWasInRange && enoughChange);

    return justDisappeared;
}

void Infrared::poll() {
    if (this->_historyUpdater.isReadyToRun()) {
        this->_valueHistory.add(this->read());

        this->_secondMostRecentValue = this->_mostRecentValue;
        this->_mostRecentValue = this->readSafe();
    }
}

int16_t Infrared::average() { return this->_valueHistory.getMedian(); }

String Infrared::getValueHistoryAsString() {
    return (String)this->_valueHistory;
}

void Infrared::_setMultiplexer() {
    Wire.beginTransmission(MULTIPLEXER_SLAVE_ADDRESS);
    Wire.write(1 << this->_index);
    Wire.endTransmission();
}

// int Infrared::getMostResentDistance() { return this->_mostRecentValue; }
// int Infrared::secMost() { return this->_secondMostRecentValue; }
