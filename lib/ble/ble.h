#ifndef BLE_H
#define BLE_H

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
    void poll();

   private:
    ErrorIndicator* errorIndicator;
};

#endif  // BLE_H
