

#include "bluetoothLowEnergy.h"

/**
 * @brief CompressedPose struct, a slimmed down version of the Pose comprised
 * of 3 16 bit integers.
 *
 * This struct is designed to simplify sending data over bluetooth, as it is
 * just 6 bytes that can be interpreted easily on the other end.
 */
struct CompressedPoseStruct {
    int16_t x;      // X position in millimeters.
    int16_t y;      // Y position in millimeters.
    int16_t angle;  // The angle in degrees.
};

struct CompressedObstacleStruct {
    int16_t x;  // X position in millimeters.
    int16_t y;  // Y position in millimeters.
    int16_t priority;
};

// BLERead and BLENotify are bitmask flags, the logical or combines them into
// one flag, for conciseness.
#define BLE_READ_NOTIFY BLERead | BLENotify

BluetoothLowEnergy::BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                                       const char* mainServiceUUID,
                                       const char* obstacleUUID,
                                       const char* robotPoseUUID)
    : _errorIndicatorPtr(errorIndicatorPtr),
      _mainService(mainServiceUUID),
      _obstaclePositionCharacteristic(obstacleUUID, BLE_READ_NOTIFY,
                                      sizeof(CompressedObstacleStruct)),
      _robotPoseCharacteristic(robotPoseUUID, BLE_READ_NOTIFY,
                               sizeof(CompressedPoseStruct)) {}

void BluetoothLowEnergy::setup(const char* deviceName, const char* macAddress) {
    // Set a boolean indicating if the error indicator object is available
    bool errorIndicatorAvailable = (this->_errorIndicatorPtr != NULL);

    if (!BLE.begin()) {
        if (errorIndicatorAvailable) {
            String errorMessage = "BLE initialisation has failed.";
            _errorIndicatorPtr->errorOccurred(errorMessage);
        }
    }

    if (BLE.address() != macAddress) {
        if (errorIndicatorAvailable) {
            String errorMessage =
                "The BLE MAC address read from the device is not the same as "
                "the one defined in systemInfo.h.\nPlease update the "
                "BLE_MAC_ADDRESS in systemInfo.h to " +
                String(BLE.address()) + ".";

            _errorIndicatorPtr->errorOccurred(errorMessage);
        }
    }

    // Name the device
    BLE.setDeviceName(deviceName);
    BLE.setLocalName(deviceName);

    this->_mainService.addCharacteristic(this->_obstaclePositionCharacteristic);
    this->_mainService.addCharacteristic(this->_robotPoseCharacteristic);

    BLE.addService(this->_mainService);
    BLE.setAdvertisedService(this->_mainService);

    // Start advertising
    BLE.advertise();
}

void BluetoothLowEnergy::sendObstaclePosition(Position obstaclePosition,
                                              uint8_t priority) {
    CompressedObstacleStruct compressedObstaclePosition;

    compressedObstaclePosition.x = (int16_t)obstaclePosition.x;
    compressedObstaclePosition.y = (int16_t)obstaclePosition.y;
    compressedObstaclePosition.priority = (int16_t)priority;

    uint8_t* dataToSend = (uint8_t*)&compressedObstaclePosition;

    this->_obstaclePositionCharacteristic.writeValue(
        dataToSend, sizeof(CompressedObstacleStruct));
}

void BluetoothLowEnergy::sendRobotPose(Pose robotPose) {
    CompressedPoseStruct compressedRobotPose;

    compressedRobotPose.x = (int16_t)robotPose.position.x;
    compressedRobotPose.y = (int16_t)robotPose.position.y;
    compressedRobotPose.angle = (int16_t)robotPose.angle;

    uint8_t* dataToSend = (uint8_t*)&compressedRobotPose;

    this->_robotPoseCharacteristic.writeValue(dataToSend,
                                              sizeof(CompressedPoseStruct));
}

void BluetoothLowEnergy::poll() { BLE.poll(); }