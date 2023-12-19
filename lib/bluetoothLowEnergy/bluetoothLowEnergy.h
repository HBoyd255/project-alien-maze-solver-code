

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

#include "errorIndicator.h"

class BluetoothLowEnergy {
   public:
    BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                       const char* mainServiceUUID, const char* bumperUUID,
                       const char* rangeUUID, const char* positionUUID

    );
    void setup(const char* deviceName, const char* macAddress);

    void updateBumper(uint8_t value);
    void updateRangeSensors(uint16_t left, uint16_t front, uint16_t right);
    void updatePosition(int16_t x, int16_t y, int16_t angle);
    void poll();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    BLEService _mainService;
    BLEByteCharacteristic _bumperCharacteristic;
    BLECharacteristic _rangeSensorsCharacteristic;
    BLECharacteristic _positionCharacteristic;
};

#endif  // BLUETOOTH_LOW_ENERGY_H
