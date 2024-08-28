/**
 * @file systemInfo.h
 * @brief Definitions for the pin designations and other system information for
 * the Project A.L.I.E.N. robot.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-21
 * @copyright Copyright (c) 2024
 */

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

// The mac address of the BLE module.
#define BLE_MAC_ADDRESS "e3:95:8a:e9:b3:f5"

// The name of the BLE device.
#define BLE_DEVICE_NAME "A.L.I.E.N"
// A.L.I.E.N - Autonomous Labyrinth Investigator, Explorer, and Navigator

// UUIDS for the BLE service and characteristics, generated at using
// https://www.uuidgenerator.net/
// There must all be less than 80 character line length, or the python script
// breaks. //TODO Remove
#define MAIN_SERVICE_UUID "cbb12285-f4bf-44e3-95e3-f402eb8ccd49"
#define ROBOT_POSE_UUID "0d9d01ba-f4e5-4cbc-896e-e391c9b7ae74"
#define BRICK_UUID "3aa898fd-58bb-42f8-b126-d2f31bbc6c5b"

// Serial
#define SERIAL_BAUD_RATE 230400

// Motors
// Left (Motor 1)
#define LEFT_MOTOR_DIRECTION_PIN A0
#define LEFT_MOTOR_SPEED_PIN D9
#define LEFT_MOTOR_ENCODER_A_PIN D2
#define LEFT_MOTOR_ENCODER_B_PIN D4
#define LEFT_MOTOR_ROTATION_INVERTED true

// Right (Motor 2)
#define RIGHT_MOTOR_DIRECTION_PIN A1
#define RIGHT_MOTOR_SPEED_PIN D10
#define RIGHT_MOTOR_ENCODER_A_PIN D3
#define RIGHT_MOTOR_ENCODER_B_PIN A7
#define RIGHT_MOTOR_ROTATION_INVERTED false

#define DEFAULT_DRIVE_SPEED 35
#define INITIAL_ANGLE -90

// Shift registers
#define BUMPER_SHIFT_REG_DATA A6
#define NES_SHIFT_REG_DATA D8
#define COMMON_SHIFT_REG_LOAD A3
#define COMMON_SHIFT_REG_CLOCK A2
#define BUMPER_BIT_OFFSET 4

// Interrupt pin
#define BUMPER_INTERRUPT_PIN D8 //TODO Remove

// The offset in degrees that the bumper buttons have been rotated around from
// the position where the front button would be at 0 degrees.
#define BUMPER_ROTATION_OFFSET 180  //TODO Remove

// Pixels
// The pin that D1 of the LED strip is connected to.
#define PIXELS_DATA_PIN D5
// The number of Ws2812 LEDs on the robot.
#define LED_COUNT 16
// The offset in degrees that the LEDs have been rotated around from the
// position where LED 0 would be at 0 degrees.
#define LED_ROTATION_OFFSET 180  //TODO Remove

// Ultrasonic
#define ULTRASONIC_TRIGGER D7
#define ULTRASONIC_ECHO D6
#define ULTRASONIC_TIMEOUT_MICROSECONDS 20000UL
#define ULTRASONIC_MAX_DISTANCE 1000
#define ULTRASONIC_DATA_SHELF_LIFE 200
#define FRONT_ULTRASONIC_FORWARD_DISTANCE 85

// Infrared
#define LEFT_INFRARED_INDEX 0
#define LEFT_INFRARED_FORWARD_DISTANCE 85

#define FRONT_LEFT_INFRARED_INDEX 1
#define FRONT_LEFT_INFRARED_FORWARD_DISTANCE 64

#define FRONT_RIGHT_INFRARED_INDEX 2
#define FRONT_RIGHT_INFRARED_FORWARD_DISTANCE 64

#define RIGHT_INFRARED_INDEX 3
#define RIGHT_INFRARED_FORWARD_DISTANCE 85

#endif  // SYSTEM_INFO_H