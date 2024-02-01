
/**
 * @file bluetoothLowEnergy.cpp
 * @brief A wrapper for the ArduinoBLE.h module used to simplify the process of
 * transmitting data using BLE.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2024-01-24
 * @copyright Copyright (c) 2024
 */
#include "bluetoothLowEnergy.h"

#include "brick.h"
#include "errorIndicator.h"

/**
 * @brief CompressedPose struct, a slimmed down version of the Pose comprised
 * of 3 16 bit integers.
 *
 * This struct is designed to simplify sending data over bluetooth, as it is
 * just 6 bytes that can be interpreted easily on the other end.
 */
struct __attribute__((packed)) CompressedPoseStruct {
    int16_t x;      // X position in millimeters.
    int16_t y;      // Y position in millimeters.
    int16_t angle;  // The angle in degrees.
};

struct __attribute__((packed)) CompressedBrickStruct {
    int16_t x;           // X position in millimeters.
    int16_t y;           // Y position in millimeters.
    uint8_t isVertical;  // The angle in degrees.
    uint8_t brickNumber;
};

// BLERead and BLENotify are bitmask flags, the logical or combines them into
// one flag, for conciseness.
#define BLE_READ_NOTIFY BLERead | BLENotify

/**
 * @brief Construct a new BluetoothLowEnergy object
 *
 * @param mainServiceUUID The UUID to attach the main service to.
 * @param robotPoseUUID  The UUID to attach the robotPose characteristic to.
 * @param brickUUID The UUID to attach the brick characteristic to.
 */
BluetoothLowEnergy::BluetoothLowEnergy(const char* mainServiceUUID,
                                       const char* robotPoseUUID,
                                       const char* brickUUID)
    : _mainService(mainServiceUUID),
      _robotPoseCharacteristic(robotPoseUUID, BLE_READ_NOTIFY,
                               sizeof(CompressedPoseStruct)),
      _brickCharacteristic(brickUUID, BLE_READ_NOTIFY,
                           sizeof(CompressedBrickStruct)) {}

/**
 * @brief Sets up the BLE events.
 *
 * @param deviceName The name of the robot to broadcast.
 * @param macAddress The mac address stored in the source code, to compare
 * to the one read from the device.
 */
void BluetoothLowEnergy::setup(const char* deviceName, const char* macAddress) {
    // Raise a critical error if BLE can't start.
    if (!BLE.begin()) {
        String errorMessage = "BLE initialisation has failed.";
        ErrorIndicator_G.errorOccurred(errorMessage);
    }

    // Raise a critical error the mac address read from the board differs from
    // the one saved in the source code. The purpose of this is to ensure
    // compatibility with the central program, as both programs read the mac
    // address from the same file.
    if (BLE.address() != macAddress) {
        String errorMessage =
            "The BLE MAC address read from the device is not the same as "
            "the one defined in systemInfo.h.\nPlease update the "
            "BLE_MAC_ADDRESS in systemInfo.h to " +
            String(BLE.address()) + ".";

        ErrorIndicator_G.errorOccurred(errorMessage);
    }

    // Name the device.
    BLE.setDeviceName(deviceName);
    BLE.setLocalName(deviceName);

    // Add the characteristics to the main service.
    this->_mainService.addCharacteristic(this->_robotPoseCharacteristic);
    this->_mainService.addCharacteristic(this->_brickCharacteristic);

    // Add the main service to the BLE.
    BLE.addService(this->_mainService);

    // Advertise the main service.
    BLE.setAdvertisedService(this->_mainService);

    // Start advertising.
    BLE.advertise();
}

/**
 * @brief Transmits the current pose of the robot over BLE.
 *
 * @param robotPose The current pose of the robot.
 */
void BluetoothLowEnergy::sendRobotPose(Pose robotPose) {
    CompressedPoseStruct compressedRobotPose;

    compressedRobotPose.x = (int16_t)robotPose.position.x;
    compressedRobotPose.y = (int16_t)robotPose.position.y;
    compressedRobotPose.angle = (int16_t)robotPose.angle;

    uint8_t* dataToSend = (uint8_t*)&compressedRobotPose;

    this->_robotPoseCharacteristic.writeValue(dataToSend,
                                              sizeof(CompressedPoseStruct));
}

/**
 * @brief Transmits a list of bricks over BLE, brick by brick.
 *
 * @param brickListToSend The list of bricks to send.
 */
void BluetoothLowEnergy::sendBrickList(BrickList brickListToSend) {
    int brickCount = brickListToSend.getBrickCount();
    for (int i = 0; i < brickCount; i++) {
        this->_sendBrick(brickListToSend.getBrick(i), i);
    }
}

/**
 * @brief Polls the ble connection.
 */
void BluetoothLowEnergy::poll() { BLE.poll(); }

/**
 * @brief Checks if a central device is currently connected.
 *
 * @return (true) If a central device is currently connected.
 * @return (false) If a central device is not currently connected.
 */
bool BluetoothLowEnergy::isConnected() { return BLE.connected(); }

/**
 * @brief Transmits data about a given brick over BLE.
 *
 * @param brickToSend The brick to send over BLE.
 * @param brickNumber The index of the current brick in the blacklist.
 */
void BluetoothLowEnergy::_sendBrick(Brick brickToSend, int brickNumber) {
    CompressedBrickStruct compressedBrick;

    compressedBrick.x = (int16_t)brickToSend.position.x;
    compressedBrick.y = (int16_t)brickToSend.position.y;
    compressedBrick.isVertical = (uint8_t)brickToSend.isVertical;
    compressedBrick.brickNumber = (uint8_t)brickNumber;

    uint8_t* dataToSend = (uint8_t*)&compressedBrick;

    this->_brickCharacteristic.writeValue(dataToSend,
                                          sizeof(CompressedBrickStruct));
}