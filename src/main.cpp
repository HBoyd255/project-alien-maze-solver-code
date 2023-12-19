
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

#include "angle.h"
#include "binary.h"
#include "bluetoothLowEnergy.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "infrared.h"
#include "motionTracker.h"
#include "motor.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

// TODO pass in the on board led
ErrorIndicator errorIndicator(LED_BUILTIN);

Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_A_PIN, LEFT_MOTOR_ENCODER_B_PIN,
                LEFT_MOTOR_ROTATION_INVERTED);

Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_A_PIN, RIGHT_MOTOR_ENCODER_B_PIN,
                 RIGHT_MOTOR_ROTATION_INVERTED);

Drive drive(&leftMotor, &rightMotor);

Pixels pixels(LED_GROUPS, PIXELS_DATA_PIN);
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, ULTRASONIC_TIMEOUT);
Infrared leftInfrared(&errorIndicator, LEFT_INFRARED_INDEX);
Infrared rightInfrared(&errorIndicator, RIGHT_INFRARED_INDEX);
Infrared frontLeftInfrared(&errorIndicator, FRONT_LEFT_INFRARED_INDEX);
Infrared frontRightInfrared(&errorIndicator, FRONT_RIGHT_INFRARED_INDEX);

Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);

BluetoothLowEnergy bluetoothLowEnergy(&errorIndicator, MAIN_SERVICE_UUID,
                                      BUMPER_CHARACTERISTIC_UUID,
                                      RANGE_SENSORS_CHARACTERISTIC_UUID,
                                      POSITION_CHARACTERISTIC_UUID);

MotionTracker motionTracker(&leftMotor, &rightMotor, &frontLeftInfrared,
                            &frontRightInfrared);

volatile bool bumperUpdate = false;

#include <list>

struct {
    int16_t x;
    int16_t y;
    int16_t angle;
} typedef Pose;

using Path = std::list<Pose>;

Path pathList;

void printPose(const Pose& poseToPrintPtr) {
    Serial.print("X :");
    Serial.print(poseToPrintPtr.x);
    Serial.print(" Y :");
    Serial.print(poseToPrintPtr.y);
    Serial.print(" angle :");
    Serial.println(poseToPrintPtr.angle);
}
void printPath(const Path& listToPrint) {
    for (const Pose& elementPtr : listToPrint) {
        printPose(elementPtr);
    }
}

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
    frontLeftInfrared.setup();
    frontRightInfrared.setup();

    ultrasonic.setup([]() { ultrasonic.isr(); });

    bumper.setup();

    // Use a lambda function to set the bumperUpdate flag when the bumper is
    // pressed or released.
    bumper.assignCallback([]() { bumperUpdate = true; });

    bluetoothLowEnergy.setup(BLE_DEVICE_NAME, BLE_MAC_ADDRESS);

    pathList.push_back({1, 1, 1});
    pathList.push_back({1700, 7856, -89});

    //     leftMotor.setVelocity(-100);
    //     rightMotor.setVelocity(100);
}
// TODO change the set all to use the pixel function, not the group function

// std::vector<std::vector<uint8_t>> groups =

void loop() {
    pixels.stupidTest();

    for (uint8_t i = 0; i < pixels.getGroupCount(); i++) {
        pixels.clear();
        pixels.setGroup(i, 255, 0, 255, true);
        delay(1000);
    }
}
