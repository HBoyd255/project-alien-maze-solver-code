

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

#include "errorIndicator.h"
#include "systemInfo.h"

class BluetoothLowEnergy {
   public:
    BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr);
    void setup();
    void updateRangeSensors(uint16_t leftSensor, uint16_t frontSensor,
                            uint16_t rightSensor);
    void updateBumper(uint8_t value);
    void updatePosition(uint16_t x, uint16_t y, uint16_t angle);
    void poll();

   private:
    ErrorIndicator* errorIndicator;
};

#endif  // BLUETOOTH_LOW_ENERGY_H
