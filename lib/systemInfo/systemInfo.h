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
#define BLE_MAC_ADDRESS "8d:b6:2e:ef:3b:6b"

// The name of the BLE device.
#define BLE_DEVICE_NAME "A.L.I.E.N"
// A.L.I.E.N - Autonomous Labyrinth Investigator, Explorer, and Navigator

// UUIDS for the BLE service and characteristics, generated at using
// https://www.uuidgenerator.net/
// There must all be less than 80 character line length, or the python script
// breaks.
// TODO Move these to a json or something, so that my CPP and Python can both
// properly read from the same file without having to parse a .h file in python.
#define MAIN_SERVICE_UUID "cbb12285-f4bf-44e3-95e3-f402eb8ccd49"
#define ROBOT_POSE_UUID "0d9d01ba-f4e5-4cbc-896e-e391c9b7ae74"
#define SENSOR_UUID "d14e8cdf-93f8-4e25-8393-f23c8c2c0684"
#define NEEDY_UUID "e233e08c-ab7b-4e73-95b3-e77236dde767"
#define GO_TO_UUID "a9e3419b-5aa1-414e-915d-f746404694ae"

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