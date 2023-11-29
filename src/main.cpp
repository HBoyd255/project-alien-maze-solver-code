

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
    ultrasonic.setup([]() { ultrasonic.isr(); });

    bumper.setup();

    // Use a lambda function to set the bumperUpdate flag when the bumper is
    // pressed or released.
    bumper.assignCallback([]() { bumperUpdate = true; });

    harrysBle.setup();
}

void loop() {
    Serial.print("left motor steps: ");
    Serial.print(leftMotor.getStepsInMillimeters());
    Serial.print("right motor steps: ");
    Serial.print(rightMotor.getStepsInMillimeters());

    int16_t diff =
        leftMotor.getStepsInMillimeters() - rightMotor.getStepsInMillimeters();

    // doing one rotation of the robot involves a difference in steps of 780mm
    // between the two motors.
    // so the degree of rotation is the difference between the two motors
    // divided by 2.16 (with 2.16 being approximately 780/360)

    uint16_t degree = diff / 2.16;

    Serial.print("degree: ");
    Serial.println(degree);

    uint16_t leftSensor = leftInfrared.read();
    uint16_t frontSensor = ultrasonic.read();
    uint16_t rightSensor = rightInfrared.read();

    // Serial.print("Left:");
    // Serial.print(leftSensor);
    // Serial.print(" Front:");
    // Serial.print(frontSensor);
    // Serial.print(" Right:");
    // Serial.println(rightSensor);

    harrysBle.updateRangeSensors(leftSensor, frontSensor, rightSensor);
    harrysBle.updateBumper(bumper.read());
    harrysBle.updatePosition(100,
                             100, degree);
    harrysBle.poll();

    delay(1000);
}
