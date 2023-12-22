

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

#include "angleAndPosition.h"
#include "errorIndicator.h"

class BluetoothLowEnergy {
   public:
    BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                       const char* mainServiceUUID, const char* obstacleUUID,
                       const char* robotPoseUUID);

    void setup(const char* deviceName, const char* macAddress);

    void sendObstaclePosition(Position obstaclePosition, uint8_t priority);
    void sendRobotPose(Pose robotPose);

    void poll();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    BLEService _mainService;
    BLECharacteristic _obstaclePositionCharacteristic;
    BLECharacteristic _robotPoseCharacteristic;
};

#endif  // BLUETOOTH_LOW_ENERGY_H
