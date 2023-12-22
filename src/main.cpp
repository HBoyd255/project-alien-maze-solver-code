
// https://www.arduino.cc/reference/en/
#include <Arduino.h>

#include "angleAndPosition.h"
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
#include "vectorUpgrades.h"

ErrorIndicator errorIndicator(LED_BUILTIN, SERIAL_BAUD_RATE);

Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_A_PIN, LEFT_MOTOR_ENCODER_B_PIN,
                LEFT_MOTOR_ROTATION_INVERTED);

Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_A_PIN, RIGHT_MOTOR_ENCODER_B_PIN,
                 RIGHT_MOTOR_ROTATION_INVERTED);

Drive drive(&leftMotor, &rightMotor);

Pixels pixels(PIXELS_DATA_PIN, LED_COUNT, LED_ROTATION_OFFSET);
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO,
                      ULTRASONIC_TIMEOUT_MICROSECONDS, ULTRASONIC_MAX_DISTANCE,
                      ULTRASONIC_DATA_SHELF_LIFE);

// TODO Change this class so that 639 is not passed in, as 639mm is not a user
// preference, it is a hardware limitation.
Infrared leftInfrared(&errorIndicator, LEFT_INFRARED_INDEX, 639);
Infrared rightInfrared(&errorIndicator, RIGHT_INFRARED_INDEX, 639);
Infrared frontLeftInfrared(&errorIndicator, FRONT_LEFT_INFRARED_INDEX, 639);
Infrared frontRightInfrared(&errorIndicator, FRONT_RIGHT_INFRARED_INDEX, 639);

Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN,
              BUMPER_ROTATION_OFFSET);

BluetoothLowEnergy bluetoothLowEnergy(&errorIndicator, MAIN_SERVICE_UUID,
                                      BUMPER_CHARACTERISTIC_UUID,
                                      RANGE_SENSORS_CHARACTERISTIC_UUID,
                                      POSITION_CHARACTERISTIC_UUID);

MotionTracker motionTracker(&leftMotor, &rightMotor, &frontLeftInfrared,
                            &frontRightInfrared);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    errorIndicator.assignPixels(&pixels);
    errorIndicator.assignDrive(&drive);
    errorIndicator.setup();

    leftMotor.setup([]() { leftMotor.isr(); });
    rightMotor.setup([]() { rightMotor.isr(); });

    pixels.setup();

    leftInfrared.setup([]() { leftInfrared.routineFunction(); });
    rightInfrared.setup([]() { rightInfrared.routineFunction(); });
    frontLeftInfrared.setup([]() { frontLeftInfrared.routineFunction(); });
    frontRightInfrared.setup([]() { frontRightInfrared.routineFunction(); });

    ultrasonic.setup([]() { ultrasonic.isr(); });

    bumper.setup([]() { bumper.isr(); });

    bluetoothLowEnergy.setup(BLE_DEVICE_NAME, BLE_MAC_ADDRESS);
}

PassiveSchedule scheduler1(100);

uint32_t startTime;
uint32_t endTime;
uint32_t duration;
int16_t val;

void polls() {
    ultrasonic.poll();
    motionTracker.poll();
}

Position overlayPositionOntoPose(Position position, Pose pose) {
    float sinAngle = sin(pose.angle.toRadians());
    float cosAngle = cos(pose.angle.toRadians());

    Position positionToReturn;

    positionToReturn.x = position.x * cosAngle + position.y * sinAngle;
    positionToReturn.y = position.y * cosAngle + position.x * sinAngle;

    positionToReturn += pose.position;

    return positionToReturn;
}

void loop() {
    polls();

    static Pose UsonicPose = {{0, 85}, 0};
    Pose robotPose = motionTracker.getPose();

    int16_t distance = ultrasonic.read();

    if (distance > 0) {
        Position posRelSensor;
        posRelSensor.y = distance;

        Position posRelRobot =
            overlayPositionOntoPose(posRelSensor, UsonicPose);

        Position posGlob = overlayPositionOntoPose(posRelRobot, robotPose);

        Serial.println(posGlob);
    }

    delay(10);
}
