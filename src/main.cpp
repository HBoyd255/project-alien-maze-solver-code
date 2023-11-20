#include <Arduino.h>

#include "motors.h"

Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_ENCODER_PIN, true);
// TODO look into deconstructor and if they are needed here.
Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_ENCODER_PIN, false);

void setup() {
    Serial.begin(115200);

    leftMotor.setup();
    rightMotor.setup();

    leftMotor.connectISR([]() { leftMotor.ISR(); });
    rightMotor.connectISR([]() { rightMotor.ISR(); });
}

void loop() {
    leftMotor.checkISR();
    rightMotor.checkISR();

    if (Serial.available()) {
        char input = Serial.read();
        switch (input) {
            case 'w':
                leftMotor.setSteps(100);
                rightMotor.setSteps(100);
                break;
            case 'a':
                leftMotor.setSteps(-200);
                rightMotor.setSteps(200);
                break;
            case 's':
                leftMotor.setSteps(-100);
                rightMotor.setSteps(-100);
                break;
            case 'd':
                leftMotor.setSteps(200);
                rightMotor.setSteps(-200);
                break;
            default:
                break;
        }
    }
}
