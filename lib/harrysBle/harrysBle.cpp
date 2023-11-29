

#include "harrysBle.h"

struct {
    uint16_t leftInfrared;
    uint16_t ultrasonic;
    uint16_t rightInfrared;
} typedef RangeSensorData;

BLEService sensorService(SENSOR_SERVICE_UUID);

BLEByteCharacteristic bumperCharacteristic(BUMPER_CHARACTERISTIC_UUID,
                                           BLERead | BLENotify);
BLECharacteristic sensorDataCharacteristic(RANGE_SENSORS_CHARACTERISTIC_UUID,
                                           BLERead | BLENotify,
                                           sizeof(RangeSensorData));

BluetoothLowEnergy::BluetoothLowEnergy(ErrorIndicator* errorIndicatorPtr) {
    this->errorIndicator = errorIndicatorPtr;
}

void BluetoothLowEnergy::setup() {
    // Set a boolean indicating if the error indicator object is available
    bool errorIndicatorAvailable = (this->errorIndicator != NULL);

    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1)
            ;
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
    BLE.setDeviceName(BLE_DEVICE_NAME);
    BLE.setLocalName(BLE_DEVICE_NAME);

    sensorService.addCharacteristic(sensorDataCharacteristic);
    sensorService.addCharacteristic(bumperCharacteristic);
    BLE.addService(sensorService);
    BLE.setAdvertisedService(sensorService);

    // Start advertising
    BLE.advertise();
}

void BluetoothLowEnergy::updateRangeSensors(uint16_t leftSensor,
                                            uint16_t frontSensor,
                                            uint16_t rightSensor) {
    static RangeSensorData sensorData;

    sensorData.leftInfrared = leftSensor;
    sensorData.ultrasonic = frontSensor;
    sensorData.rightInfrared = rightSensor;

    // Write the sensor data to the characteristic


    //TODO fix this so it casts to three uint16_t
    sensorDataCharacteristic.writeValue((uint8_t*)&sensorData,
                                        sizeof(RangeSensorData));
}

void BluetoothLowEnergy::updateBumper(uint8_t value) {
    static uint8_t lastValue = 0;

    if (value != lastValue) {
        bumperCharacteristic.writeValue(value);
        lastValue = value;
    }
}

void BluetoothLowEnergy::poll() { BLE.poll(); }