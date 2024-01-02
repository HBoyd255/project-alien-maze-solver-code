

#include "bluetoothLowEnergy.h"

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
struct CompressedPoseStruct {
    int16_t x;      // X position in millimeters.
    int16_t y;      // Y position in millimeters.
    int16_t angle;  // The angle in degrees.
};

struct CompressedGridChunkStruct {
    uint32_t value;
    uint32_t startIndex;
    uint32_t count;
};

// BLERead and BLENotify are bitmask flags, the logical or combines them into
// one flag, for conciseness.
#define BLE_READ_NOTIFY BLERead | BLENotify

BluetoothLowEnergy::BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                                       const char* mainServiceUUID,
                                       const char* robotPoseUUID,
                                       const char* gridChunkUUID,
                                       const char* needyUUID)
    : _errorIndicatorPtr(errorIndicatorPtr),
      _mainService(mainServiceUUID),
      _robotPoseCharacteristic(robotPoseUUID, BLE_READ_NOTIFY,
                               sizeof(CompressedPoseStruct)),
      _gridChunkCharacteristic(gridChunkUUID, BLE_READ_NOTIFY,
                               sizeof(CompressedGridChunkStruct)),
      _needyCharacteristic(needyUUID, BLEWrite, sizeof(byte)) {}

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

    this->_needyCharacteristic.setEventHandler(
        BLEWritten, BluetoothLowEnergy::_onNeedsData);

    this->_mainService.addCharacteristic(this->_robotPoseCharacteristic);
    this->_mainService.addCharacteristic(this->_gridChunkCharacteristic);
    this->_mainService.addCharacteristic(_needyCharacteristic);

    BLE.addService(this->_mainService);
    BLE.setAdvertisedService(this->_mainService);

    // Start advertising
    BLE.advertise();

    BLE.setEventHandler(BLEConnected, BluetoothLowEnergy::_onConnect);

    BLE.setEventHandler(BLEDisconnected, BluetoothLowEnergy::_onDisconnect);
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

void BluetoothLowEnergy::sendGridChunk(uint8_t value, uint32_t startIndex,
                                       uint32_t count) {
    CompressedGridChunkStruct compressedGridChunk;

    compressedGridChunk.value = value;
    compressedGridChunk.startIndex = startIndex;
    compressedGridChunk.count = count;

    uint8_t* dataToSend = (uint8_t*)&compressedGridChunk;

    this->_gridChunkCharacteristic.writeValue(
        dataToSend, sizeof(CompressedGridChunkStruct));
}

void BluetoothLowEnergy::poll() { BLE.poll(); }

bool BluetoothLowEnergy::newlyConnected() {
    if (newlyConnectedFlag) {
        newlyConnectedFlag = false;
        return true;
    }
    return false;
}
bool BluetoothLowEnergy::newlyDisconnected() {
    if (newlyDisconnectedFlag) {
        newlyDisconnectedFlag = false;
        return true;
    }
    return false;
}
bool BluetoothLowEnergy::needsData() {
    if (needsDataFlag) {
        needsDataFlag = false;
        return true;
    }
    return false;
}

void BluetoothLowEnergy::_onConnect(BLEDevice central) {
    newlyConnectedFlag = true;
}
void BluetoothLowEnergy::_onDisconnect(BLEDevice central) {
    newlyDisconnectedFlag = true;
}
void BluetoothLowEnergy::_onNeedsData(BLEDevice central,
                                      BLECharacteristic characteristic) {
    needsDataFlag = true;
}