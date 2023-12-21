/**
 * @file systemInfo.h
 * @brief Definition for constant values related to the hardware setup of my
 * project.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */

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
#define MAIN_SERVICE_UUID "cbb12285-f4bf-44e3-95e3-f402eb8ccd49"
#define RANGE_SENSORS_CHARACTERISTIC_UUID "1995fd92-e400-40a0-8001-e82d89ea5aaf"
#define BUMPER_CHARACTERISTIC_UUID "a2425f99-39c2-4770-9098-b1a7ea9b2e35"
#define POSITION_CHARACTERISTIC_UUID "911926f2-a3b4-442d-86eb-e90d2f90f59a"

// Serial
#define SERIAL_BAUD_RATE 115200

// Hardware definitions

// LEDs
// The pin that D1 of the LED strip is connected to.
#define PIXELS_DATA_PIN D5  // USONIC 3
// The number of Ws2812 LEDs on the robot.
#define LED_COUNT 16
// The offset in degrees that the LEDs have been rotated around from the
// position where LED 0 would be at 0 degrees.
#define LED_ROTATION_OFFSET 180

// Motors
// Left
#define LEFT_MOTOR_DIRECTION_PIN A0
#define LEFT_MOTOR_SPEED_PIN D9
#define LEFT_MOTOR_ENCODER_A_PIN D2
#define LEFT_MOTOR_ENCODER_B_PIN D4  // USONIC 4
#define LEFT_MOTOR_ROTATION_INVERTED true

// Right
#define RIGHT_MOTOR_DIRECTION_PIN A1
#define RIGHT_MOTOR_SPEED_PIN D10
#define RIGHT_MOTOR_ENCODER_A_PIN D3
#define RIGHT_MOTOR_ENCODER_B_PIN A7  // GPIO 1
#define RIGHT_MOTOR_ROTATION_INVERTED false

// Bumper
// Shift register
#define BUMPER_SHIFT_REG_DATA A6   // GPIO 2
#define BUMPER_SHIFT_REG_LOAD A3   // GPIO 3
#define BUMPER_SHIFT_REG_CLOCK A2  // GPIO 4
// Interrupt pin
#define BUMPER_INTERRUPT_PIN D8

// The offset in degrees that the bumper buttons have been rotated around from
// the position where the front button would be at 0 degrees.
#define BUMPER_ROTATION_OFFSET 180

// Ultrasonic
#define ULTRASONIC_TRIGGER D7  // USONIC 1
#define ULTRASONIC_ECHO D6     // USONIC 2
#define ULTRASONIC_TIMEOUT_MICROSECONDS 20000UL
#define ULTRASONIC_MAX_DISTANCE 1000
#define ULTRASONIC_DATA_SHELF_LIFE 200

// Infrared
#define LEFT_INFRARED_INDEX 0
#define RIGHT_INFRARED_INDEX 1
#define FRONT_LEFT_INFRARED_INDEX 2
#define FRONT_RIGHT_INFRARED_INDEX 3

#endif  // SYSTEM_INFO_H