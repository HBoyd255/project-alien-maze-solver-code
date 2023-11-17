
#include "motors.h"

#include <Arduino.h>

const int DIRECTION_PINS[2] = {LEFT_MOTOR_DIRECTION_PIN,
                               RIGHT_MOTOR_DIRECTION_PIN};
const int SPEED_PINS[2] = {LEFT_MOTOR_SPEED_PIN, RIGHT_MOTOR_SPEED_PIN};

void setupMotors() {
    pinMode(LEFT_MOTOR_DIRECTION_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_DIRECTION_PIN, OUTPUT);

    pinMode(LEFT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_SPEED_PIN, OUTPUT);
}

int setMotorSpeed(Motors motor, int formatted_speed) {
    // Make sure that the provided value is withing the range of -100 to 100,
    // return 1 if not to signify an error.
    if ((formatted_speed > 100) || (formatted_speed < -100)) {
        Serial.println("Speed must be between -100 and 100");
        return 1;
    }

    // Set the direction to forwards("1") if the speed is greater than 0,
    // Direction represents the drive direction, not the rotation direction
    bool direction = formatted_speed > 0;

    // If the target motor is the left motor, reverse the direction, because to
    // Drive forwards, both motors need to rotate in the opposite direction.
    if (motor == left_motor) {
        direction ^= 1;
    }

    // scale the speed from a directional value from -100 to 100, to a
    // directionless value from 0 to 255, ready to be fed into the pwm signal.
    int scaled_speed = int(abs(formatted_speed) * 2.55);

    // fix that 100 gets rounded to 254, and set it to the desired 255.
    if (scaled_speed == 254) {
        scaled_speed = 255;
    }

    // Set the direction and speed of the motor.
    digitalWrite(DIRECTION_PINS[motor], direction);
    analogWrite(SPEED_PINS[motor], (int)scaled_speed);
    return 0;
}
