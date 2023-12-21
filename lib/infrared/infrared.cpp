

#include "infrared.h"

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

Infrared::Infrared(ErrorIndicator* errorIndicatorPtr, uint8_t index,
                   uint16_t maxRange)
    : _errorIndicatorPtr(errorIndicatorPtr),
      _index(index),
      _maxRange(maxRange),
      _valueHistory(MAX_HISTORY) {}

void Infrared::setup(voidFuncPtr routineFunctionPtr) {
    Wire.begin();

    this->_setMultiplexer();

    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_SHIFT_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 1);

    if (!Wire.available()) {
        String errorMessage =
            "Cannot read sensor at index " + String(this->_index) + ".";
        this->_errorIndicatorPtr->errorOccurred(errorMessage);
    }
    this->_shiftValue = Wire.read();

    this->_historyUpdater.setup(routineFunctionPtr, POLL_PERIOD);
}

int16_t Infrared::read() {
    this->_setMultiplexer();

    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_DISTANCE_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 2);

    // TODO add a timeout and a call to the errorIndicator.
    if (Wire.available() < 2) {
        this->_errorIndicatorPtr->errorOccurred("NO CAN READ");
        // Serial.print("Cannot read from sensor ");
        // Serial.print(this->_index);
        // Serial.println(".");
    }

    uint8_t high = Wire.read();
    uint8_t low = Wire.read();

    // The given formula for calculating distance is:
    // distance(cm) = (high * 16 + low)/16/(int)pow(2,shift);
    // Which can be simplified to:
    // distance(mm) = (((high << 4) | low) * 10) >> (4 + shiftValue);

    uint16_t distance = (((high << 4) | low) * 10) >> (4 + this->_shiftValue);

    return (distance < this->_maxRange) ? distance : -1;
}

int16_t Infrared::readSafe() {
    const uint8_t errorThreshold = 1;

    uint8_t errorCount = this->_valueHistory.countErrors();

    return (errorCount < errorThreshold) ? this->_valueHistory.getLast() : -1;
}

void Infrared::routineFunction() { this->_valueHistory.add(this->read()); }

void Infrared::poll() { this->_historyUpdater.poll(); }

int16_t Infrared::average() { return this->_valueHistory.getMedian(); }

String Infrared::getValueHistoryAsString() {
    return (String)this->_valueHistory;
}

void Infrared::_setMultiplexer() {
    Wire.beginTransmission(MULTIPLEXER_SLAVE_ADDRESS);
    Wire.write(1 << this->_index);
    Wire.endTransmission();
}