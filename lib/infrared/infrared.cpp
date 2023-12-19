

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

void setMultiplexer(uint8_t channel) {
    if (channel >= MULTIPLEXER_CHANNEL_COUNT) {
        Serial.println("Invalid, there are only");
        Serial.println(MULTIPLEXER_CHANNEL_COUNT);
        Serial.println("Channels.");
        return;
    }

    Wire.beginTransmission(MULTIPLEXER_SLAVE_ADDRESS);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

Infrared::Infrared(ErrorIndicator* errorIndicatorPtr, uint8_t index) {
    this->_errorIndicatorPtr = errorIndicatorPtr;
    this->index = index;
}

void Infrared::setup() {
    Wire.begin();

    grabMultiplexer();

    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_SHIFT_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 1);

    if (!Wire.available()) {
        String errorMessage =
            "Cannot read sensor at index " + String(this->index) + ".";
        this->_errorIndicatorPtr->errorOccurred(errorMessage);
    }
    this->shiftValue = Wire.read();
}

uint16_t Infrared::read() {
    grabMultiplexer();

    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_DISTANCE_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 2);

    // TODO add a timeout and a call to the errorIndicator.
    while (Wire.available() < 2) {
        Serial.print("Cannot read from sensor ");
        Serial.print(this->index);
        Serial.println(".");
    }

    uint8_t high = Wire.read();
    uint8_t low = Wire.read();

    // The given formula for calculating distance is:
    // distance(cm) = (high * 16 + low)/16/(int)pow(2,shift);
    // Which can be simplified to:
    // distance(mm) = (((high << 4) | low) * 10) >> (4 + shiftValue);

    uint16_t distance = (((high << 4) | low) * 10) >> (4 + this->shiftValue);

    return distance;
}

void Infrared::grabMultiplexer() { setMultiplexer(this->index); }