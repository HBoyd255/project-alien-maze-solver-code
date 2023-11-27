

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

    // Use a lambda function to set the bumperUpdate flag when the bumper is
    // pressed or released.
    bumper.assignCallback([]() { bumperUpdate = true; });
}

void loop() {
    // Print out values over serial

    Serial.print("Left: ");
    Serial.print(leftInfrared.read());
    Serial.print("mm, Front: ");
    Serial.print(ultrasonic.read());
    Serial.print("mm, Right:");
    Serial.print(rightInfrared.read());
    Serial.println("mm");
}
