
// Im just going to put this here for now to keep track
//  Im basing the positioning of the traditional polar coordinate system
//  Not the flipped y system that graphical programs use,
//  Like in this image
//  https://en.wikipedia.org/wiki/Body_relative_direction#/media/File:XYZ_model.jpg
//  On this wikipedia page
//  https://en.wikipedia.org/wiki/Body_relative_direction
// X and Y are measured in millimeters, and the angle is measured in degrees
// 0 degrees is the positive x axis, and the angle increases in the clockwise
// direction

// For now im just going to add a Screenshot in this directory to show the
// coordinate system.

// This does mean that at the start of the program, the robot facing 90 degrees.
// This is because the robot is facing the positive x axis, and the angle
// increases in the clockwise direction.

// angle can be calculated using the formula degrees(atan2(y,x))

#define INITIAL_ANGLE 90


#include <Arduino.h>

#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "harrysBle.h"
#include "infrared.h"
#include "motor.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

ErrorIndicator errorIndicator;

Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_A_PIN, LEFT_MOTOR_ENCODER_B_PIN,
                LEFT_MOTOR_ROTATION_INVERTED);

Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_A_PIN, RIGHT_MOTOR_ENCODER_B_PIN,
                 RIGHT_MOTOR_ROTATION_INVERTED);

Drive drive(&leftMotor, &rightMotor);
Pixels pixels(PIXELS_DATA_PIN);
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, ULTRASONIC_TIMEOUT);
Infrared leftInfrared(LEFT_INFRARED_INDEX);
Infrared rightInfrared(RIGHT_INFRARED_INDEX);
Infrared alignmentLeftInfrared(ALIGNMENT_LEFT_INFRARED_INDEX);
Infrared alignmentRightInfrared(ALIGNMENT_RIGHT_INFRARED_INDEX);

Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);

BluetoothLowEnergy harrysBle(&errorIndicator);

volatile bool bumperUpdate = 0;

// void ISR() { ultrasonic.isr(); }

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    errorIndicator.assignPixels(&pixels);
    errorIndicator.assignDrive(&drive);
    errorIndicator.setup();

    leftMotor.setup([]() { leftMotor.isr(); });
    rightMotor.setup([]() { rightMotor.isr(); });

    pixels.setup();

    leftInfrared.setup();
    rightInfrared.setup();
    alignmentLeftInfrared.setup();
    alignmentRightInfrared.setup();

    ultrasonic.setup([]() { ultrasonic.isr(); });

    bumper.setup();

    // Use a lambda function to set the bumperUpdate flag when the bumper is
    // pressed or released.
    bumper.assignCallback([]() { bumperUpdate = true; });

    harrysBle.setup();
}

int16_t last_av_steps = 0;

float running_x_pos = 0;
float running_y_pos = 0;

void update_pos(int32_t av, uint16_t degree) {
    int16_t diff = av - last_av_steps;

    running_x_pos += diff * sin(radians(degree));
    running_y_pos += diff * cos(radians(degree));

    last_av_steps = av;
}

void loop() {
    int32_t leftSteps = leftMotor.getStepsInMillimeters();
    int32_t rightSteps = rightMotor.getStepsInMillimeters();

    int32_t diff = leftSteps - rightSteps;

    // doing one rotation of the robot involves a difference in steps of 780mm
    // between the two motors.
    // so the degree of rotation is the difference between the two motors
    // divided by 2.16 (with 2.16 being approximately 780/360)

    int16_t degree = diff / 2.28;

    int32_t average_steps = (leftSteps + rightSteps) / 2;

    update_pos(average_steps, degree);

    int16_t discreet_x_pos = (int16_t)running_x_pos;
    int16_t discreet_y_pos = (int16_t)running_y_pos;

    uint16_t leftSensor = leftInfrared.read();
    uint16_t frontSensor = ultrasonic.read();
    uint16_t rightSensor = rightInfrared.read();
    uint16_t alignmentLeftSensor = alignmentLeftInfrared.read();
    uint16_t alignmentRightSensor = alignmentRightInfrared.read();

    harrysBle.updateRangeSensors(leftSensor, frontSensor, rightSensor);
    harrysBle.updateBumper(bumper.read());
    harrysBle.updatePosition(discreet_x_pos, discreet_y_pos, degree);
    harrysBle.poll();

    // Position Printing
    // Serial.print(" leftSteps:");
    // Serial.print(leftSteps);
    // Serial.print(" rightSteps:");
    // Serial.print(rightSteps);
    // Serial.print("Dif ");
    // Serial.print(diff);
    // Serial.print(" degree: ");
    // Serial.print(degree);
    // Serial.print(" average steps: ");
    // Serial.print(average_steps);
    // Serial.print(" XD:");
    // Serial.print(discreet_x_pos);
    // Serial.print(" YD:");
    // Serial.println(discreet_y_pos);

    // Sensor Printing
    // Serial.print("Left:");
    // Serial.print(leftSensor);
    // Serial.print(" Front:");
    // Serial.print(frontSensor);
    // Serial.print(" Right:");
    // Serial.print(rightSensor);
    //     Serial.print(" AlignLeft: ");
    //     Serial.print(alignmentLeftSensor);
    //     Serial.print(" AlignRight: ");
    //     Serial.print(alignmentRightSensor);
    //
    //     int16_t alignmentDifference = alignmentLeftSensor -
    //     alignmentRightSensor;
    //
    //     Serial.print(" AlignDiff: ");
    //     Serial.print(alignmentDifference);
    //
    //     const uint16_t alignmentOffset = 60;
    //
    //     int16_t angleToWall = degrees(atan2(alignmentDifference,
    //     alignmentOffset));
    //
    //     Serial.print(" AngleToWall: ");
    //     Serial.println(angleToWall);

    int x = 0;

    int y = 1;

    delay(10);
}
