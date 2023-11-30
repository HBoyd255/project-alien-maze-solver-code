

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

void getDiff(int16_t &diff, int16_t &degree) {
    diff =
        leftMotor.getStepsInMillimeters() - rightMotor.getStepsInMillimeters();
    degree = diff / 2.16;
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

    // Serial.print("Dif ");
    // Serial.print(diff);

    // doing one rotation of the robot involves a difference in steps of 780mm
    // between the two motors.
    // so the degree of rotation is the difference between the two motors
    // divided by 2.16 (with 2.16 being approximately 780/360)

    int16_t degree = diff / 2.28;

    int32_t average_steps = (leftSteps + rightSteps) / 2;

    Serial.print(" degree: ");
    Serial.print(degree);
    Serial.print(" average steps: ");
    Serial.print(average_steps);

    update_pos(average_steps, degree);

    Serial.print(" X:");
    Serial.print(running_x_pos);
    Serial.print(" Y:");
    Serial.println(running_y_pos);

    int16_t discreet_x_pos = (int16_t)running_x_pos;
    int16_t discreet_y_pos = (int16_t)running_y_pos;

    Serial.print(" X:");
    Serial.print(running_x_pos);
    Serial.print(" Y:");
    Serial.print(running_y_pos);

    Serial.print(" XD:");
    Serial.print(discreet_x_pos);
    Serial.print(" YD:");
    Serial.println(discreet_y_pos);

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
    harrysBle.updatePosition(discreet_x_pos, discreet_y_pos, degree);
    harrysBle.poll();
}
