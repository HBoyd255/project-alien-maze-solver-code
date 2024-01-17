

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
                       const char* brickUUID, const char* needyUUID,
                       const char* cornerUUID);

    void setup(const char* deviceName, const char* macAddress);
    void sendRobotPose(Pose robotPose);
    void sendBrick(Brick brickToSend, int brickNumber);
    void sendBrickList(BrickList brickListToSend);

    void sendCorner(Position cornerPosition, uint8_t index);

    void poll();

    static bool newlyConnected();
    static bool newlyDisconnected();
    static bool needsData();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    BLEService _mainService;
    BLECharacteristic _robotPoseCharacteristic;
    BLECharacteristic _brickCharacteristic;
    BLECharacteristic _needyCharacteristic;
    BLECharacteristic _cornerCharacteristic;

    // TODO fix the inline variable issue;
    static inline bool newlyConnectedFlag = false;
    static inline bool newlyDisconnectedFlag = false;
    static inline bool needsDataFlag = false;

    static void _onConnect(BLEDevice central);
    static void _onDisconnect(BLEDevice central);
    static void _onNeedsData(BLEDevice central,
                             BLECharacteristic characteristic);
};

#endif  // BLUETOOTH_LOW_ENERGY_H
