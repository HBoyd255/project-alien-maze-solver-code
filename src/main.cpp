#include <Arduino.h>

#include "bumpers.h"
#include "drive.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

Drive drive;
Pixels pixels;
Bumper bumper;
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, 20000UL);

bool bumperUpdated = false;

void bumperCallback() { bumperUpdated = true; }

void setup() {
    Serial.begin(115200);
    drive.setup();
    pixels.setup();

    ultrasonic.setup();
}

void loop() {

    unsigned int distance = ultrasonic.read();

    if (distance < 600) {
        int diff = distance - 200;

        Serial.println(diff);

        drive.setVelocity(constrain(diff, -100, 100));

    } else {
        drive.setVelocity(0);
    }

    delay(10);
}
