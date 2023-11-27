

#include <Arduino.h>

// REMOVE
#include "Motor.h"
#include "bumper.h"
#include "drive.h"
#include "infrared.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

// Drive drive;
Pixels pixels;

Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, 20000UL);
Infrared leftInfrared(LEFT_INFRARED_INDEX);
Infrared rightInfrared(RIGHT_INFRARED_INDEX);
Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);

Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_PIN, true);
Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_PIN, false);

bool bumperUpdate = false;

// TODO do some pid stuff

void setup() {
    Serial.begin(115200);
    // drive.setup();
    pixels.setup();

    leftInfrared.setup();
    rightInfrared.setup();

    ultrasonic.setup();

    bumper.setup();
    bumper.assignCallback([]() { bumperUpdate = true; });

    leftMotor.setup();
    rightMotor.setup();
}

void loop() {
    // Print out values over serial
    // Serial.print("Left: ");
    // Serial.print(leftInfrared.read());
    // Serial.print("mm, Front: ");
    // Serial.print(ultrasonic.read());
    // Serial.print("mm, Right:");
    // Serial.print(rightInfrared.read());
    // Serial.println("mm");

    //     int leftDistance = leftInfrared.read();
    //
    //     int intensity = constrain(300 - leftDistance, 0, 255);
    //
    //     pixels.setGroup(LEFT_LED_GROUP, intensity, 0, 0, true);
    //
    //     unsigned int distance = ultrasonic.read();
    //
    //     if (distance < 600) {
    //         int diff = distance - 200;
    //
    //         Serial.println(diff);
    //
    //         drive.setVelocity(constrain(diff, -100, 100));
    //
    //     } else {
    //         drive.stop();
    //     }
    //
    //     delay(10);
    //

    if (bumperUpdate) {
        bumperUpdate = false;

        u_int8_t bumperData = bumper.read();
        pixels.displayBumperHard(bumperData);

        if (bumperData) {
            leftMotor.setSteps(-12);
            rightMotor.setSteps(-10);
        }
    }

    int leftDistance = leftInfrared.read();
    int rightDistance = rightInfrared.read();

    int width = leftDistance + rightDistance;

    int distanceToMoveLeft = leftDistance - width / 2;
    int distanceToMoveRight = rightDistance - width / 2;

    // Serial.print("L: ");
    // Serial.print(distanceToMoveLeft);
    // Serial.print(" R: ");
    // Serial.println(distanceToMoveRight);

    int frontDistance = ultrasonic.read();

    distanceToMoveLeft /= 10;
    distanceToMoveRight /= 10;

    int pidL = constrain(75 - distanceToMoveLeft, -100, 100);
    int pidR = constrain(75 - distanceToMoveRight, -100, 100);


    if (leftMotor.hasStepsRemaining() || rightMotor.hasStepsRemaining()) {
        leftMotor.checkEncoder();
        rightMotor.checkEncoder();
    } else {



        leftMotor.setVelocity(pidL);
        rightMotor.setVelocity(pidR);
    }
}
