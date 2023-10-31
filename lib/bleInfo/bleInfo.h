#ifndef UUIDS_H
#define UUIDS_H




// The mac address of the BLE module.
#define BLE_MAC_ADDRESS "2c:bc:f3:cb:1f:f9"

// The name of the BLE module.
#define BLE_DEVICE_NAME "A.L.I.E.N"
// A.L.I.E.N stands for Autonomous Labyrinth Investigator, Explorer, and Navigator

// UUIDS for the BLE service and characteristics, generated at using
// https://www.uuidgenerator.net/
#define SENSOR_SERVICE_UUID "cbb12285-f4bf-44e3-95e3-f402eb8ccd49"
#define BUMPER_CHARACTERISTIC_UUID "a2425f99-39c2-4770-9098-b1a7ea9b2e35"
#define MILLIS_CHARACTERISTIC_UUID "cf72bce9-7e65-425e-a702-83733fd0a4e0"
#define MOTOR_OVERRIDE_CHARACTERISTIC_UUID "206740f3-4a1b-43f3-ac27-504f771b825b"
#define LED_CHARACTERISTIC_UUID "c0b778f3-9921-475c-8992-4450dfa7f5ca"


#endif  // UUIDS_H