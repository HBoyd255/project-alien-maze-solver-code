

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

#include "errorIndicator.h"

class BluetoothLowEnergy {
   public:
    BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr);
    void setup(const char* deviceName);

    void updateRangeSensors(uint16_t left, uint16_t front, uint16_t right);
    void updateBumper(uint8_t value);
    void updatePosition(uint16_t x, uint16_t y, uint16_t angle);
    void poll();

   private:
    const char* deviceName;
    ErrorIndicator* errorIndicator;
};

#endif  // BLUETOOTH_LOW_ENERGY_H
