

#include "bluetoothLowEnergy.h"

// This is the only module to directly import systemInfo.h. All the other
// modules have their constant values passed by reference upon construction.
// This module, however, imports the entire contents of systemInfo.h, due to
// quantity of variables that are needed. This decision was also inspired by the
// fact that the first and most important function of the systemInfo.h was just
// to enforce consistency between the UUIDs uploaded to the robot and the ones
// accessed by the central program.
#include "systemInfo.h"  // TODO actually

// Two structs created for the purpose of bundling integers together so
// that the can be sent as one object over bluetooth.
struct {
    uint16_t leftInfrared;
    uint16_t ultrasonic;
    uint16_t rightInfrared;
} typedef RangeSensorData;

struct {
    int16_t x;
    int16_t y;
    int16_t angle;
} typedef PositionStruct;

BLEService mainService(MAIN_SERVICE_UUID);
BLEByteCharacteristic bumperCharacteristic(BUMPER_CHARACTERISTIC_UUID,
                                           BLERead | BLENotify);
BLECharacteristic rangeSensorsCharacteristic(RANGE_SENSORS_CHARACTERISTIC_UUID,
                                             BLERead | BLENotify,
                                             sizeof(RangeSensorData));
BLECharacteristic positionCharacteristic(POSITION_CHARACTERISTIC_UUID,
                                         BLERead | BLENotify,
                                         sizeof(PositionStruct));

BluetoothLowEnergy::BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr) {
    this->errorIndicator = errorIndicatorPtr;
}

void BluetoothLowEnergy::setup(const char* deviceName) {
    // Set a boolean indicating if the error indicator object is available
    bool errorIndicatorAvailable = (this->errorIndicator != NULL);

    if (!BLE.begin()) {
        if (errorIndicatorAvailable) {
            String errorMessage = "BLE initialisation has failed.";
            errorIndicator->errorOccurred(errorMessage);
        }
    }

    if (BLE.address() != BLE_MAC_ADDRESS) {
        if (errorIndicatorAvailable) {
            String errorMessage =
                "The BLE MAC address read from the device is not the same as "
                "the one defined in systemInfo.h.\nPlease update the "
                "BLE_MAC_ADDRESS in systemInfo.h to " +
                String(BLE.address()) + ".";

            errorIndicator->errorOccurred(errorMessage);
        }
    }

    // Name the device
    BLE.setDeviceName(deviceName);
    BLE.setLocalName(deviceName);

    

    mainService.addCharacteristic(rangeSensorsCharacteristic);
    mainService.addCharacteristic(bumperCharacteristic);
    mainService.addCharacteristic(positionCharacteristic);
    BLE.addService(mainService);
    BLE.setAdvertisedService(mainService);

    // Start advertising
    BLE.advertise();
}

void BluetoothLowEnergy::updateRangeSensors(uint16_t leftSensor,
                                            uint16_t frontSensor,
                                            uint16_t rightSensor) {
    RangeSensorData sensorData;

    sensorData.leftInfrared = leftSensor;
    sensorData.ultrasonic = frontSensor;
    sensorData.rightInfrared = rightSensor;

    // Write the sensor data to the characteristic

    // TODO fix this so it casts to three uint16_t
    rangeSensorsCharacteristic.writeValue((uint8_t*)&sensorData,
                                          sizeof(RangeSensorData));
}

void BluetoothLowEnergy::updateBumper(uint8_t value) {
    static uint8_t lastValue = 0;

    if (value != lastValue) {
        bumperCharacteristic.writeValue(value);
        lastValue = value;
    }
}

void BluetoothLowEnergy::updatePosition(uint16_t x, uint16_t y,
                                        uint16_t angle) {
    static PositionStruct PositionData;

    PositionData.x = x;
    PositionData.y = y;
    PositionData.angle = angle;

    positionCharacteristic.writeValue((uint8_t*)&PositionData,
                                      sizeof(RangeSensorData));
}

void BluetoothLowEnergy::poll() { BLE.poll(); }