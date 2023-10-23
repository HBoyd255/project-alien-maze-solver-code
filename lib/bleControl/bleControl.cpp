
#include "bleControl.h"

#include "bleInfo.h"

BLEService sensorService(SENSOR_SERVICE_UUID);
BLEByteCharacteristic bumperCharacteristic(BUMPER_CHARACTERISTIC_UUID,
                                           BLERead | BLENotify);

void setupBle() {
    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1)
            ;
    }
    Serial.print("BLE started with the address: ");
    Serial.println(BLE.address());

    // Name the device
    BLE.setDeviceName("HBOYD");
    BLE.setLocalName("HBOYD");

    // Add the characteristic to the sensor service, add the sensor service to
    // the device, and advertise the service.
    sensorService.addCharacteristic(bumperCharacteristic);
    BLE.addService(sensorService);
    BLE.setAdvertisedService(sensorService);

    // Start advertising
    BLE.advertise();
}

// TODO expand this functionality
void bleUpdateBumper(byte value) {
    bumperCharacteristic.writeValue(value);
}

void blePoll() {
    BLE.poll();
}