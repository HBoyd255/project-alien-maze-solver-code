

#include "bluetoothLowEnergy.h"

#include "brick.h"
#include "errorIndicator.h"

void blePeripheralConnectHandler(BLEDevice central) {
    // BLE device connected
    Serial.print("Connected event, central: ");
    Serial.println(central.address());
    // You can add your custom code here to handle a connection event
}

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

struct CompressedBrickStruct {
    int16_t x;           // X position in millimeters.
    int16_t y;           // Y position in millimeters.
    uint8_t isVertical;  // The angle in degrees.
    uint8_t brickNumber;
};

struct __attribute__((packed)) CompressedCornerStruct {
    int16_t x;      // X position in millimeters.
    int16_t y;      // Y position in millimeters.
    uint8_t index;  // The angle in degrees.
};

// BLERead and BLENotify are bitmask flags, the logical or combines them into
// one flag, for conciseness.
#define BLE_READ_NOTIFY BLERead | BLENotify

BluetoothLowEnergy::BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                                       const char* mainServiceUUID,
                                       const char* robotPoseUUID,
                                       const char* brickUUID,
                                       const char* cornerUUID)
    : _errorIndicator_P(errorIndicatorPtr),
      _mainService(mainServiceUUID),
      _robotPoseCharacteristic(robotPoseUUID, BLE_READ_NOTIFY,
                               sizeof(CompressedPoseStruct)),
      _brickCharacteristic(brickUUID, BLE_READ_NOTIFY,
                           sizeof(CompressedBrickStruct)),
      _cornerCharacteristic(cornerUUID, BLE_READ_NOTIFY,
                            sizeof(CompressedCornerStruct)) {}

void BluetoothLowEnergy::setup(const char* deviceName, const char* macAddress) {
    // _connectFunction_P = connectFunction_P;
    // _disconnectFunction_P = disconnectFunction_P;

    // Set a boolean indicating if the error indicator object is available
    bool errorIndicatorAvailable = (this->_errorIndicator_P != NULL);

    if (!BLE.begin()) {
        if (errorIndicatorAvailable) {
            String errorMessage = "BLE initialisation has failed.";
            _errorIndicator_P->errorOccurred(errorMessage);
        }
    }

    if (BLE.address() != macAddress) {
        if (errorIndicatorAvailable) {
            String errorMessage =
                "The BLE MAC address read from the device is not the same as "
                "the one defined in systemInfo.h.\nPlease update the "
                "BLE_MAC_ADDRESS in systemInfo.h to " +
                String(BLE.address()) + ".";

            _errorIndicator_P->errorOccurred(errorMessage);
        }
    }

    // Name the device
    BLE.setDeviceName(deviceName);
    BLE.setLocalName(deviceName);

    this->_mainService.addCharacteristic(this->_robotPoseCharacteristic);
    this->_mainService.addCharacteristic(this->_cornerCharacteristic);
    this->_mainService.addCharacteristic(this->_brickCharacteristic);

    BLE.addService(this->_mainService);
    BLE.setAdvertisedService(this->_mainService);

    // Start advertising
    BLE.advertise();
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

void BluetoothLowEnergy::sendBrick(Brick brickToSend, int brickNumber) {
    CompressedBrickStruct compressedBrick;

    compressedBrick.x = (int16_t)brickToSend.position.x;
    compressedBrick.y = (int16_t)brickToSend.position.y;
    compressedBrick.isVertical = (uint8_t)brickToSend.isVertical;
    compressedBrick.brickNumber = (uint8_t)brickNumber;

    uint8_t* dataToSend = (uint8_t*)&compressedBrick;

    this->_brickCharacteristic.writeValue(dataToSend,
                                          sizeof(CompressedBrickStruct));
}

void BluetoothLowEnergy::sendBrickList(BrickList brickListToSend) {
    int brickCount = brickListToSend.getBrickCount();

    for (int i = 0; i < brickCount; i++) {
        this->sendBrick(brickListToSend.getBrick(i), i);
    }
}

void BluetoothLowEnergy::sendCorner(Position cornerPosition, uint8_t index) {
    CompressedCornerStruct compressedCorner;

    compressedCorner.x = (int16_t)cornerPosition.x;
    compressedCorner.y = (int16_t)cornerPosition.y;
    compressedCorner.index = index;

    uint8_t* dataToSend = (uint8_t*)&compressedCorner;

    this->_cornerCharacteristic.writeValue(dataToSend,
                                           sizeof(CompressedCornerStruct));
}

void BluetoothLowEnergy::poll() { BLE.poll(); }
