

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

// Forward declaration
class ErrorIndicator;
class Pose;
class Position;
class Brick;
class BrickList;

class BluetoothLowEnergy {
   public:
    BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                       const char* mainServiceUUID, const char* robotPoseUUID,
                       const char* brickUUID, const char* cornerUUID);

    void setup(const char* deviceName, const char* macAddress);
    void sendRobotPose(Pose robotPose);
    void sendBrick(Brick brickToSend, int brickNumber);
    void sendBrickList(BrickList brickListToSend);

    void sendCorner(Position cornerPosition, uint8_t index);

    void poll();

   private:
    ErrorIndicator* _errorIndicator_P;
    BLEService _mainService;
    BLECharacteristic _robotPoseCharacteristic;
    BLECharacteristic _brickCharacteristic;
    BLECharacteristic _cornerCharacteristic;
};

#endif  // BLUETOOTH_LOW_ENERGY_H
