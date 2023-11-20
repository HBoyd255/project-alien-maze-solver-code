#include <Arduino.h>

#include "motors.h"

#define LEFT_ENCODER_PIN D2
#define RIGHT_ENCODER_PIN D3

volatile bool left_motor_updated = false;
void updateLeftMotor() { left_motor_updated = true; }

volatile bool right_motor_updated = false;
void updateRightMotor() { right_motor_updated = true; }

void updateMotors() {
    left_motor_updated = true;
    right_motor_updated = true;
}

void setup() {
    Serial.begin(115200);

    pinMode(LEFT_ENCODER_PIN, INPUT);
    pinMode(RIGHT_ENCODER_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_PIN), updateLeftMotor,
                    CHANGE);
    attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_PIN), updateRightMotor,
                    CHANGE);

    setupMotors();
}

void motorTakeSteps(Motors motor, volatile bool *motors_enabled,
                    volatile int *steps_remaining) {
    // If the motor is enabled and there are still steps remaining, keep
    // stepping.
    if (*motors_enabled && *steps_remaining) {
        // reduce left_steps_remaining by 1, or increase it by 1 if its
        // negative, either way, take one step closer to 0.
        *steps_remaining += (*steps_remaining > 0) ? -1 : 1;

        // If the steps remaining is less than 5, set it to 0, this is to
        // prevent the motor from stuttering when it gets close to the target
        // position.
        if (abs(*steps_remaining) < 5) {
            *steps_remaining = 0;
        }

        // Calculate how fast to set the motor, the speed is proportional to
        // the number of steps remaining, and is capped at 100.
        int velocity = constrain(*steps_remaining, -100, 100);

        // Set the motor to the calculated velocity.
        motorSetVelocity(motor, velocity);
    }
    // if the motor is disabled, or there are no steps remaining, stop the
    // motor.
    else {
        motorStop(motor);
    }
}

void loop() {
    static volatile bool motors_enabled = true;

    static volatile int left_steps_remaining = 0;
    static volatile int right_steps_remaining = 0;

    if (left_motor_updated) {
        motorTakeSteps(left_motor, &motors_enabled, &left_steps_remaining);
        left_motor_updated = false;
    }
    if (right_motor_updated) {
        motorTakeSteps(right_motor, &motors_enabled, &right_steps_remaining);
        right_motor_updated = false;
    }

    if (Serial.available()) {
        char input = Serial.read();
        switch (input) {
            case 'w':
                left_steps_remaining = 100;
                right_steps_remaining = 100;
                updateMotors();

                break;
            case 'a':
                left_steps_remaining = -200;
                right_steps_remaining = 200;
                updateMotors();
                break;
            case 's':
                left_steps_remaining = -100;
                right_steps_remaining = -100;
                updateMotors();
                break;
            case 'd':
                left_steps_remaining = 200;
                right_steps_remaining = -200;
                updateMotors();
                break;
            default:
                break;
        }
    }
}
