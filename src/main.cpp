
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
// also the angle should be locked between -179 and 180 degrees

#define INITIAL_ANGLE 90

#include <Arduino.h>

#include "binary.h"
#include "bluetoothLowEnergy.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "infrared.h"
#include "motor.h"
#include "pedometer.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

// TODO pass in the on board led
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

BluetoothLowEnergy bluetoothLowEnergy(&errorIndicator);

Pedometer pedometer(&leftMotor, &rightMotor);

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

    bluetoothLowEnergy.setup(BLE_DEVICE_NAME);

    pixels.setPixel(0,255,100,0,true);

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

void spinTest() {
    int16_t angle = pedometer.calculateAngle();

    angle += 179;

    uint8_t led = angle / 22.5;

    pixels.clear();
    pixels.setPixel(led, 255, 0, 0, true);
    // Serial.println(led);
}

void loop() {





    // for (uint16_t x = 0; x < 256; x++) {
    //     Serial.println();
    //     Serial.println(x);
    //     printByte(x);
    // }
    // Serial.println("==================================");

    //     spinTest();
    // 
    //     uint32_t bit = millis() / 3000;
    // 
    //     bit = bit % 5;
    // 
    //     switch (bit) {
    //         case 0:
    //             leftMotor.setVelocity(-100);
    //             rightMotor.setVelocity(100);
    //             break;
    //         case 1:
    //             leftMotor.setVelocity(100);
    //             rightMotor.setVelocity(-100);
    //             break;
    //         case 2:
    //             leftMotor.setVelocity(-50);
    //             rightMotor.setVelocity(100);
    //             break;
    //         case 3:
    //             leftMotor.setVelocity(-50);
    //             rightMotor.setVelocity(50);
    //             break;
    //         case 4:
    //             leftMotor.setVelocity(0);
    //             rightMotor.setVelocity(-0);
    //             break;
    //         default:
    //             break;
    //     }

//     Serial.print(" Diff:");
//     Serial.print(pedometer.getDifference());
//     Serial.print(" Angle:");
//     Serial.print(pedometer.calculateAngle());
// 
//     Serial.print(" US:");
//     Serial.print(ultrasonic.read());
//     Serial.print(" L:");
//     Serial.print(leftInfrared.read());
//     Serial.print(" R:");
//     Serial.print(rightInfrared.read());
//     Serial.print(" FL:");
//     Serial.print(alignmentLeftInfrared.read());
//     Serial.print(" FR:");
//     Serial.print(alignmentRightInfrared.read());

    bluetoothLowEnergy.updateBumper(bumper.read());
}
