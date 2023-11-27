

#include <Arduino.h>

#include "bumper.h"
#include "drive.h"
#include "infrared.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

Drive drive;
Pixels pixels;

Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, 20000UL);
Infrared leftInfrared(LEFT_INFRARED_INDEX);
Infrared rightInfrared(RIGHT_INFRARED_INDEX);
Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);
bool bumperUpdate = false;

// TODO do some pid stuff

void setup() {
    Serial.begin(115200);
    drive.setup();
    pixels.setup();

    leftInfrared.setup();
    rightInfrared.setup();

    ultrasonic.setup();

    bumper.setup();
    bumper.assignCallback([]() { bumperUpdate = true; });
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
            drive.setSteps(-10, -15);
        }
    }

    int frontDistance = ultrasonic.read();
    int leftDistance = leftInfrared.read();
    int rightDistance = rightInfrared.read();

    if (frontDistance > leftDistance && frontDistance > rightDistance) {
        drive.setSteps(10, 10);
    } else {
        if (leftDistance > rightDistance) {
            drive.setSteps(0, 10);
        } else {
            drive.setSteps(10, 0);
        }
    }

    // Serial.print("L: ");
    // Serial.print(distanceToMoveLeft);
    // Serial.print(" R: ");
    // Serial.println(distanceToMoveRight);
}
