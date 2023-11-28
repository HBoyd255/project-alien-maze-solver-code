

#include "ble.h"

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

    // Start advertising
    BLE.advertise();
}

void BluetoothLowEnergy::poll() { BLE.poll(); }