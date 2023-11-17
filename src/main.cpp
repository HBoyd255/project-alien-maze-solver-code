#include <Arduino.h>

#include "bumpers.h"
#include "motors.h"
#include "pixels.h"

Pixels pixels = Pixels();
Bumper bumper = Bumper();

volatile bool bumperCalled = false;

void printByte(u_int8_t byte) {
    for (int b = 7; b; b--) {
        Serial.print((byte >> b) & 1);
        Serial.print(",");
    }
    Serial.println(byte & 1);
}

void bumperCallback() {
    bumperCalled = true;
    setMotorSpeed(left_motor, 0);
    setMotorSpeed(right_motor, 0);
}

void setup() {
    Serial.begin(115200);

    pixels.setup();

    bumper.setup();
    bumper.assignCallback(bumperCallback);

    setupMotors();
}

long unsigned int timetToCallback = 0;

void loop() {
    if (bumperCalled) {
        bumperCalled = false;

        setMotorSpeed(left_motor, -100);
        setMotorSpeed(right_motor, -100);

        timetToCallback = millis() + 1000;

        // delay(10);

        // Serial.print("Whacko!");
        // printByte(bumper.read());
    }

    if (timetToCallback < millis()) {
        setMotorSpeed(left_motor, 100);
        setMotorSpeed(right_motor, 100);

        timetToCallback = 0;
    }

    /*
        int red = millis() >> 4 & 255;

        int green = 100;

        int blue = 0;

        Serial.print(red);
        Serial.print(",");
        Serial.print(green);
        Serial.print(",");
        Serial.println(blue);

        pixels.setAll(red, green, blue, false);
        pixels.setGroup(LEFT_LED_GROUP, 255, 0, 0);*/

    // pixels.show();
}
