

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

// Forward declaration
class ErrorIndicator;
class Pose;

class BluetoothLowEnergy {
   public:
    BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                       const char* mainServiceUUID, const char* robotPoseUUID,
                       const char* sensorUUID, const char* needyUUID,
                       const char* goToUUID);

    void setup(const char* deviceName, const char* macAddress);
    void sendRobotPose(Pose robotPose);
    void sendSensor(uint8_t sensorType, Pose sensorPose, int16_t sensorValue);

    void poll();

    static bool newlyConnected();
    static bool newlyDisconnected();
    static bool needsData();

   private:
    ErrorIndicator* _errorIndicatorPtr;
    BLEService _mainService;
    BLECharacteristic _robotPoseCharacteristic;
    BLECharacteristic _sensorCharacteristic;
    BLECharacteristic _needyCharacteristic;
    BLECharacteristic _goToCharacteristic;

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
