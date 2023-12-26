

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

#include "errorIndicator.h"
#include "obstacles.h"

class BluetoothLowEnergy {
   public:
    BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                       const char* mainServiceUUID, const char* obstacleUUID,
                       const char* robotPoseUUID);

    void setup(const char* deviceName, const char* macAddress);

    void sendObstacle(Obstacle obstacle);
    void sendRobotPose(Pose robotPose);

    void poll();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    BLEService _mainService;
    BLECharacteristic _obstacleCharacteristic;
    BLECharacteristic _robotPoseCharacteristic;
};

#endif  // BLUETOOTH_LOW_ENERGY_H
