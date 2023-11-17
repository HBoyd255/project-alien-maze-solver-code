#include <Arduino.h>

#include "binary.h"
#include "bumpers.h"
#include "motors.h"
#include "pixels.h"

#define MOTOR_LEFT_A D2
#define MOTOR_LEFT_B D4  // USONIC4

#define MOTOR_RIGHT_A D3
#define MOTOR_RIGHT_B D5  // USONIC3

Pixels pixels = Pixels();
Bumper bumper = Bumper();

volatile bool bumperCalled = false;
long unsigned int timetToCallback = 0;

volatile int target_steps_left = 0;
volatile int target_steps_right = 0;
volatile int steps_left = 0;
volatile int steps_right = 0;

void countStepsLeft() {
    if (digitalRead(MOTOR_LEFT_A) == digitalRead(MOTOR_LEFT_B)) {
        steps_left++;
    } else {
        steps_left--;
    }
}

void countStepsRight() {
    if (digitalRead(MOTOR_RIGHT_A) == digitalRead(MOTOR_RIGHT_B)) {
        steps_right--;
        // Inverted, because we are tracking the steps taken forwards

    } else {
        steps_right++;
    }
}

void bumperCallback() {
    bumperCalled = true;
    setMotorSpeed(left_motor, 0);
    setMotorSpeed(right_motor, 0);

    target_steps_right = steps_right + 100;
    target_steps_left = steps_left + 100;
}

void setup() {
    Serial.begin(115200);

    pixels.setup();

    bumper.setup();
    bumper.assignCallback(bumperCallback);

    attachInterrupt(digitalPinToInterrupt(MOTOR_LEFT_A), countStepsLeft,
                    CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_RIGHT_A), countStepsRight,
                    CHANGE);

    setupMotors();
}

void loop() {
    if (bumperCalled) {
        bumperCalled = false;

        //         setMotorSpeed(left_motor, -100);
        //         setMotorSpeed(right_motor, -100);
        //
        //         timetToCallback = millis() + 1000;
        //
        //         delay(10);
        //
        //         Serial.print("Whacko!");
        //         printByte(bumper.read());
    }

    if (timetToCallback < millis()) {
        setMotorSpeed(left_motor, 100);
        setMotorSpeed(right_motor, 100);

        timetToCallback = 0;
    }

    Serial.print(steps_left);
    Serial.print(",");
    Serial.println(steps_right);

    int left_diff = constrain(target_steps_left - steps_left, -100, 100);
    int right_diff = constrain(target_steps_right - steps_right, -100, 100);

    // setMotorSpeed(left_motor, left_diff);
    // setMotorSpeed(right_motor, right_diff);

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
