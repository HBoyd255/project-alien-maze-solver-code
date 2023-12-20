
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

#include <Arduino.h>

#include "angle.h"
#include "binary.h"
#include "bluetoothLowEnergy.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "history.h"
#include "infrared.h"
#include "motionTracker.h"
#include "motor.h"
#include "pixels.h"
#include "schedule.h"
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

Pixels pixels(PIXELS_DATA_PIN, LED_COUNT, LED_ROTATION_OFFSET);
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, ULTRASONIC_TIMEOUT,
                      1000);
Infrared leftInfrared(&errorIndicator, LEFT_INFRARED_INDEX, 639);
Infrared rightInfrared(&errorIndicator, RIGHT_INFRARED_INDEX, 639);
Infrared frontLeftInfrared(&errorIndicator, FRONT_LEFT_INFRARED_INDEX, 639);
Infrared frontRightInfrared(&errorIndicator, FRONT_RIGHT_INFRARED_INDEX, 639);

Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);

BluetoothLowEnergy bluetoothLowEnergy(&errorIndicator, MAIN_SERVICE_UUID,
                                      BUMPER_CHARACTERISTIC_UUID,
                                      RANGE_SENSORS_CHARACTERISTIC_UUID,
                                      POSITION_CHARACTERISTIC_UUID);

MotionTracker motionTracker(&leftMotor, &rightMotor, &frontLeftInfrared,
                            &frontRightInfrared);

volatile bool bumperUpdate = false;

bool val = false;

void toggleLED() {
    val = !val;

    digitalWrite(LED_BUILTIN, val);
}

History testHist(5);

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
}

Schedule lighty(toggleLED, 500);

History flir(5);
Schedule frontLeftInfraredSchedule([]() { flir.add(frontLeftInfrared.read()); },
                                   20);

void printFunc() {
    Serial.print(" Left:");
    Serial.print(flir.getMedian());
    Serial.print(" Right:");
    Serial.println(frontRightInfrared.read());
}
Schedule printSchedule(printFunc, 10);

void loop() {
    lighty.poll();
    frontLeftInfraredSchedule.poll();
    printSchedule.poll();
}
