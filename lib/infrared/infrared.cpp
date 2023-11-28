

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

/**
 * Sets the multiplexer channel to connect a given sensor to the I2C bus.
 *
 * @param channel The channel number to set.
 */
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

/**
 * @brief Constructs an Infrared object with the specified index.
 *
 * @param index The index of the infrared sensor.
 */
Infrared::Infrared(uint8_t index) { this->index = index; }

/**
 * @brief Initializes the infrared sensor.
 *
 * This function sets up the necessary configurations and parameters for the
 * infrared sensor. It should be called once during the setup phase of the
 * program.
 */
void Infrared::setup() {
    // Initialize the I2C bus
    Wire.begin();

    // Set the I2C bus multiplexer to the channel of this infrared sensor
    grabMultiplexer();

    // Read the shift value from the sensor
    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_SHIFT_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 1);

    // Wait until the data is available, then read it
    while (!Wire.available()) {
        Serial.print("value not received for sensor ");
        Serial.print(this->index);
        Serial.println(".");
    }
    this->shiftValue = Wire.read();
}

/**
 * @brief Reads the distance from the infrared sensor.
 *
 * This function reads the distance from the infrared sensor by communicating
 * with the sensor via I2C.
 * It calculates the distance using the given formula and returns the distance
 * in millimeters.
 *
 * @return The distance in millimeters.
 */
uint16_t Infrared::read() {
    grabMultiplexer();

    Wire.beginTransmission(IR_SLAVE_ADDRESS);
    Wire.write(IR_DISTANCE_REG_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(IR_SLAVE_ADDRESS, 2);

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

    // int distance = high >> this->shiftValue;
    return distance;
}

/**
 * @brief Sets the multiplexer to the channel of the infrared sensor.
 *
 * This function sets the multiplexer to the channel of the infrared sensor.
 * This is necessary because the multiplexer is shared between all infrared
 * sensors.
 */
void Infrared::grabMultiplexer() { setMultiplexer(this->index); }