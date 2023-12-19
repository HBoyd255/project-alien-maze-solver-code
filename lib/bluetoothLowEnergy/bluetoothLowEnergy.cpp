

#include "bluetoothLowEnergy.h"

// Two structs created for the purpose of bundling integers together so
// that the can be sent as one object over bluetooth.

/**
 * @brief Struct for bundling the distances obtained from the various range
 * sensors, all measured in millimetres.
 *
 */
struct {
    uint16_t leftInfrared;
    uint16_t frontUltrasonic;
    uint16_t rightInfrared;
} typedef RangeSensorData;

// The size of the RangeSensorData struct. Three uint16_ts should be 6 bytes.
#define RANGE_SIZE sizeof(RangeSensorData)

/**
 * @brief Struct for bundling the position and
 *
 */
struct {            // TODO turn this into the Pose.
    int16_t x;      // X position in millimeters.
    int16_t y;      // Y position in millimeters.
    int16_t angle;  // The angle in degrees()
} typedef PositionStruct;

// The size of the PositionStruct struct. Three int16_ts should be 6 bytes.
#define POSITION_SIZE sizeof(PositionStruct)

// BLERead and BLENotify are bitmask flags, the logical or combines them into
// one flag, for conciseness.
#define BLE_READ_NOTIFY BLERead | BLENotify


BluetoothLowEnergy::BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr,
                                       const char* mainServiceUUID,
                                       const char* bumperUUID,
                                       const char* rangeUUID,
                                       const char* positionUUID)
    : _mainService(mainServiceUUID),
      _bumperCharacteristic(bumperUUID, BLE_READ_NOTIFY),
      _rangeSensorsCharacteristic(rangeUUID, BLE_READ_NOTIFY, RANGE_SIZE),
      _positionCharacteristic(positionUUID, BLE_READ_NOTIFY, POSITION_SIZE)

{
    this->_errorIndicatorPtr = errorIndicatorPtr;
}

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

    this->_mainService.addCharacteristic(this->_bumperCharacteristic);
    this->_mainService.addCharacteristic(this->_rangeSensorsCharacteristic);
    this->_mainService.addCharacteristic(this->_positionCharacteristic);

    BLE.addService(this->_mainService);
    BLE.setAdvertisedService(this->_mainService);

    // Start advertising
    BLE.advertise();
}

void BluetoothLowEnergy::updateBumper(uint8_t value) {
    this->_bumperCharacteristic.writeValue(value);
}

void BluetoothLowEnergy::updateRangeSensors(uint16_t leftSensor,
                                            uint16_t frontSensor,
                                            uint16_t rightSensor) {
    RangeSensorData sensorData;

    sensorData.leftInfrared = leftSensor;
    sensorData.frontUltrasonic = frontSensor;
    sensorData.rightInfrared = rightSensor;

    RangeSensorData* ptr = &sensorData;

    uint8_t* dataToSend = (uint8_t*)&sensorData;

    this->_rangeSensorsCharacteristic.writeValue(dataToSend, RANGE_SIZE);
}

void BluetoothLowEnergy::updatePosition(int16_t x, int16_t y, int16_t angle) {
    PositionStruct PositionData;

    PositionData.x = x;
    PositionData.y = y;
    PositionData.angle = angle;

    uint8_t* dataToSend = (uint8_t*)&PositionData;

    this->_positionCharacteristic.writeValue(dataToSend, RANGE_SIZE);
}

void BluetoothLowEnergy::poll() { BLE.poll(); }