#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

// The mac address of the BLE module.
#define BLE_MAC_ADDRESS "2c:bc:f3:cb:1f:f9"

// The name of the BLE module.
#define BLE_DEVICE_NAME "A.L.I.E.N"
// A.L.I.E.N stands for Autonomous Labyrinth Investigator, Explorer, and
// Navigator

// UUIDS for the BLE service and characteristics, generated at using
// https://www.uuidgenerator.net/
#define SENSOR_SERVICE_UUID "cbb12285-f4bf-44e3-95e3-f402eb8ccd49"
#define RANGE_SENSORS_CHARACTERISTIC_UUID "1995fd92-e400-40a0-8001-e82d89ea5aaf"
#define BUMPER_CHARACTERISTIC_UUID "a2425f99-39c2-4770-9098-b1a7ea9b2e35"

// Serial
#define SERIAL_BAUD_RATE 115200

// Hardware definitions

// LEDs
// The pin that D1 of the LED strip is connected to.
#define PIXELS_DATA_PIN A2  // GPIO 4

// Motors
// Left
#define LEFT_MOTOR_DIRECTION_PIN A0
#define LEFT_MOTOR_SPEED_PIN D9
#define LEFT_MOTOR_ENCODER_PIN D2

// Right
#define RIGHT_MOTOR_DIRECTION_PIN A1
#define RIGHT_MOTOR_SPEED_PIN D10
#define RIGHT_MOTOR_ENCODER_PIN D3

// Bumper
// Shift register
#define BUMPER_SHIFT_REG_DATA A7   // GPIO 1
#define BUMPER_SHIFT_REG_LOAD A6   // GPIO 2
#define BUMPER_SHIFT_REG_CLOCK A3  // GPIO 3
// Interrupt pin
#define BUMPER_INTERRUPT_PIN D8

// Ultrasonic
#define ULTRASONIC_TRIGGER D7  // USONIC1
#define ULTRASONIC_ECHO D6     // USONIC2
#define ULTRASONIC_TIMEOUT 20000UL

// Infrared
#define LEFT_INFRARED_INDEX 0
#define RIGHT_INFRARED_INDEX 1

#endif  // SYSTEM_INFO_H