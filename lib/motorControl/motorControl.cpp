
#include "motorControl.h"

#include <Arduino.h>

#define LEFT_MOTOR_DIRECTION_PIN A0
#define RIGHT_MOTOR_DIRECTION_PIN A1

#define LEFT_MOTOR_SPEED_PIN D9
#define RIGHT_MOTOR_SPEED_PIN D10

void setMotorDirection(Motors motor, MotorDirection dir) {
    if (motor == left_motor) {
        digitalWrite(LEFT_MOTOR_DIRECTION_PIN, dir);
    } else {
        digitalWrite(RIGHT_MOTOR_DIRECTION_PIN, dir);
    }
}

void setMotorSpeed(Motors motor, uint8_t speed) {
    if (motor == left_motor) {
        analogWrite(LEFT_MOTOR_SPEED_PIN, speed);
    } else {
        analogWrite(RIGHT_MOTOR_SPEED_PIN, speed);
    }
}

void setupMotorControl() {
    pinMode(LEFT_MOTOR_DIRECTION_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_DIRECTION_PIN, OUTPUT);

    pinMode(LEFT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_SPEED_PIN, OUTPUT);
}

void setDriveDirection(DriveDirection dir) {
    switch (dir) {
        case forward:
            setMotorDirection(left_motor, antiClockwise);
            setMotorDirection(right_motor, clockwise);
            break;
        case right:
            setMotorDirection(left_motor, antiClockwise);
            setMotorDirection(right_motor, antiClockwise);
            break;
        case backwards:
            setMotorDirection(left_motor, clockwise);
            setMotorDirection(right_motor, antiClockwise);
            break;
        case left:
            setMotorDirection(left_motor, clockwise);
            setMotorDirection(right_motor, clockwise);
            break;
        default:
            break;
    }
}
void setDriveSpeed(uint8_t speed) {
    setMotorSpeed(left_motor, speed);
    setMotorSpeed(right_motor, speed);
}
