
#include "bleControl.h"

#include "bleInfo.h"



BLEService generalService(MAIN_SERVICE_UUID);
BLEByteCharacteristic bumperCharacteristic(BUMPER_CHARACTERISTIC_UUID,
                                           BLERead | BLENotify);
BLEUnsignedLongCharacteristic millisCharacteristic(MILLIS_CHARACTERISTIC_UUID,
                                                   BLERead | BLENotify);

BLEIntCharacteristic motorOverrideCharacteristic(MOTOR_OVERRIDE_CHARACTERISTIC_UUID,
                                                 BLEWrite | BLEWriteWithoutResponse);

BLEIntCharacteristic ledCharacteristic(LED_CHARACTERISTIC_UUID,
                                       BLEWrite | BLEWriteWithoutResponse);

void setupBle() {
    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1)
            ;
    }
    Serial.print("BLE started with the address: ");
    Serial.println(BLE.address());

    // Name the device
    BLE.setDeviceName(BLE_DEVICE_NAME);
    BLE.setLocalName(BLE_DEVICE_NAME);

    // Add the characteristic to the sensor service, add the sensor service to
    // the device, and advertise the service.
    generalService.addCharacteristic(bumperCharacteristic);
    generalService.addCharacteristic(millisCharacteristic);
    generalService.addCharacteristic(motorOverrideCharacteristic);
    generalService.addCharacteristic(ledCharacteristic);
    BLE.addService(generalService);
    BLE.setAdvertisedService(generalService);

    // Start advertising
    BLE.advertise();
}

// TODO expand this functionality
void bleUpdateBumper(byte value) {
    static byte lastValue = 0;

    if (value != lastValue) {
        bumperCharacteristic.writeValue(value);
        lastValue = value;
    }
}

void bleUpdateMillis(unsigned long value) {
    millisCharacteristic.writeValue(value);
}

signed char getMotorOverrideValues(Motors motor) {
    int data = motorOverrideCharacteristic.value();

    if (motor == left_motor) {
        return (data & 0xFF);

    } else {
        return ((data >> 8) & 0xFF);
    }
    return 0;
}

unsigned int getLed() {
    if (ledCharacteristic.written()) {
        return ledCharacteristic.value();
        

        // Serial.println("ledCharacteristic.written()");
        // unsigned int data = ledCharacteristic.value();

        // Serial.print("Data: ");
        // Serial.println(data);

        // uint8_t green = (data & 0xFF);
        // uint8_t blue = ((data >> 8) & 0xFF);
        // uint8_t red = ((data >> 16) & 0xFF);
        // uint8_t group = ((data >> 24) & 0xFF);

        // Serial.print("Group: ");
        // Serial.println(group);
        // Serial.print("Red: ");
        // Serial.println(red);
        // Serial.print("Green: ");
        // Serial.println(green);
        // Serial.print("Blue: ");
        // Serial.println(blue);

        // pixels.setAll(red, green, blue, true);


    }
    return 0;
}

void blePoll() {
    BLE.poll();
}