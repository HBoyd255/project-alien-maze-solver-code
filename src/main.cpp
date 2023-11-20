#include <Arduino.h>

#include "motor.h"
#include "systemInfo.h"

Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_PIN, true);
// TODO look into deconstructor and if they are needed here.
Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_PIN, false);

void setup() {
    Serial.begin(115200);

    leftMotor.setup();
    rightMotor.setup();
}

void loop() {
    leftMotor.checkEncoder();
    rightMotor.checkEncoder();

    


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
