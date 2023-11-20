#include <Arduino.h>

#include "motors.h"



volatile bool left_motor_needs_updating = false;
void updateLeftMotor() { left_motor_needs_updating = true; }

volatile bool right_motor_needs_updating = false;
void updateRightMotor() { right_motor_needs_updating = true; }

void updateMotors() {
    left_motor_needs_updating = true;
    right_motor_needs_updating = true;
}

motor left_motor = motor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                         LEFT_ENCODER_PIN, true);

motor right_motor = motor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                          RIGHT_ENCODER_PIN, false);

void setup() {
    Serial.begin(115200);

    left_motor.setup();
    right_motor.setup();

    attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_PIN), updateLeftMotor,
                    CHANGE);
    attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_PIN), updateRightMotor,
                    CHANGE);
}

void loop() {
    
    if (left_motor_needs_updating) {
        left_motor.takeStep();
        left_motor_needs_updating = false;
    }
    if (right_motor_needs_updating) {
        right_motor.takeStep();
        right_motor_needs_updating = false;
    }

    if (Serial.available()) {
        char input = Serial.read();
        switch (input) {
            case 'w':
                left_motor.steps_remaining = 100;
                right_motor.steps_remaining = 100;
                updateMotors();
                break;
            case 'a':
                left_motor.steps_remaining = -200;
                right_motor.steps_remaining = 200;
                updateMotors();
                break;
            case 's':
                left_motor.steps_remaining = -100;
                right_motor.steps_remaining = -100;
                updateMotors();
                break;
            case 'd':
                left_motor.steps_remaining = 200;
                right_motor.steps_remaining = -200;
                updateMotors();
                break;
            default:
                break;
        }
    }
}
