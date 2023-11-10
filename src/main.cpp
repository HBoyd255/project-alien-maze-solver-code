#include <Arduino.h>

// https://maker.pro/arduino/tutorial/how-to-use-the-arduino-nano-33-bles-built-in-imu

// #include <NeoPixelBus.h>

// #include "bleControl.h"
// #include "bumperControl.h"
#include "motorControl.h"
// #include "pixels.h"

#define GPIO1 A7
#define GPIO2 A6
#define GPIO3 A3
#define GPIO4 A2

#define DATA GPIO1   // Green wire
#define LOAD GPIO2   // Yellow wire
#define CLOCK GPIO3  // Orange wire

#define USONIC1 D7
#define USONIC2 D6
#define USONIC3 D5
#define USONIC4 D4

#define MOTOR_LEFT_A D2
#define MOTOR_LEFT_B USONIC3

#define MOTOR_RIGHT_A D3
#define MOTOR_RIGHT_B USONIC4

void countStepsLeft();
void countStepsRight();

volatile int steps_left = 0;
volatile int steps_right = 0;

volatile int target_left = 0;
volatile int target_right = 0;

void setup() {
    Serial.begin(115200);

    pinMode(MOTOR_LEFT_A, INPUT);
    pinMode(MOTOR_LEFT_B, INPUT);

    pinMode(MOTOR_RIGHT_A, INPUT);
    pinMode(MOTOR_RIGHT_B, INPUT);

    setupMotors();

    attachInterrupt(digitalPinToInterrupt(MOTOR_LEFT_A), countStepsLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_RIGHT_A), countStepsRight, CHANGE);
}

int target_millis = 5000;

void loop() {
    // float x, y, z;

    // IMU.readMagneticField(x, y, z);

    // Serial.print(x);
    // Serial.print(",");
    // Serial.print(y);
    // Serial.print(",");
    // Serial.println(z);

    if (target_millis < millis()) {
        target_millis = millis() + 2000;

        target_left -= 205;
        target_right += 205;
    }

    int diff_left = target_left - steps_left;

    diff_left *= 2;

    if (diff_left > 50)
        diff_left = 50;
    else if (diff_left < -50)
        diff_left = -50;

    int diff_right = target_right - steps_right;

    diff_right *= 2;

    if (diff_right > 50)
        diff_right = 50;
    else if (diff_right < -50)
        diff_right = -50;


    setMotorSpeed(left_motor, diff_left);
    setMotorSpeed(right_motor, diff_right);

    Serial.print(steps_left);
    Serial.print(",");
    Serial.print(steps_right);
    Serial.print(",");
    Serial.print(diff_left);
    Serial.print(",");
    Serial.print(diff_right);
    Serial.print(",");
    Serial.print(target_left);
    Serial.print(",");
    Serial.println(target_right);

    delay(10);
}

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
