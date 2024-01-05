
// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// TODO remove circles by using forward declaration
#include "angleAndPosition.h"
#include "binary.h"
#include "bluetoothLowEnergy.h"
#include "brick.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "history.h"
#include "infrared.h"
#include "motionTracker.h"
#include "motor.h"
#include "obstacles.h"
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

Drive drive(&leftMotor, &rightMotor, DEFAULT_DRIVE_SPEED);

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
                                      ROBOT_POSE_UUID, SENSOR_UUID, NEEDY_UUID,
                                      GO_TO_UUID);

MotionTracker motionTracker(&bluetoothLowEnergy, &leftMotor, &rightMotor,
                            &frontLeftInfrared, &frontRightInfrared);

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

ObstacleDetector usonicOD(&motionTracker, &ultrasonic, {{0, 85}, 90});

ObstacleDetector FLIROD(&motionTracker, &frontLeftInfrared, {{-36, 64}, 90});
ObstacleDetector FRIROD(&motionTracker, &frontRightInfrared, {{36, 64}, 90});
ObstacleDetector LIROD(&motionTracker, &leftInfrared, {{-85, 0}, 180});
ObstacleDetector RIROD(&motionTracker, &rightInfrared, {{85, 0}, 0});

ObstacleDetector bumperOD(&motionTracker, &bumper, {{0, 125}, 90});

void polls() {
    frontLeftInfrared.poll();
    frontRightInfrared.poll();
    leftInfrared.poll();
    rightInfrared.poll();

    ultrasonic.poll();

    motionTracker.poll(true);

    bluetoothLowEnergy.poll();

    if (bluetoothLowEnergy.newlyConnected()) {
        Serial.println("Connected");
    }

    if (bluetoothLowEnergy.needsData()) {
        Serial.println("Sending all Data");
    }

    if (bluetoothLowEnergy.newlyDisconnected()) {
        Serial.println("Disconnected");
    }
}

PassiveSchedule eachSecond(1000);

enum State {
    DrivingForwards,
    Aligning,
    InFrontOfWall  // -----------------------
};

State systemState = DrivingForwards;

void DriveForwards() {
    drive.forwards();

    int distance = ultrasonic.read();

    if (distance < 100 && distance != -1) {
        drive.stop();
        systemState = Aligning;
    }
}

void aligning() {
    const Angle angleTolerance = 0;
    Angle angleToAlign = motionTracker.angleFromFrontIR();

    if (angleToAlign < -angleTolerance) {
        drive.right();
    } else if (angleToAlign > angleTolerance) {
        drive.left();
    } else {
        drive.stop();
        systemState = InFrontOfWall;
    }
}

void inFrontOfWall() {
    int distance = ultrasonic.read();
    Serial.println(distance);

    static int innerMode = 0;

    if (innerMode == 0) {
        if (distance < 200 && distance != -1) {
            drive.left();
        } else {
            drive.stop();
            innerMode = 1;
        }
    }
    if (innerMode == 0) {
    }
}

void doState(State stateToExecute) {
    switch (stateToExecute) {
        case DrivingForwards:
            DriveForwards();
            break;
        case Aligning:
            aligning();

            break;
        case InFrontOfWall:
            inFrontOfWall();

            break;

        default:
            break;
    }
}

void test() {
    Brick testBrick;

    Serial.println(testBrick);
}

void loop() {
    test();

    //     polls();
    //
    //     // doState(systemState);
    //
    //     Serial.println(frontLeftInfrared.readSafe());

    // if (eachSecond.isReadyToRun()) {
    //     Serial.print(motionTracker.getPose());
    //     Serial.print(" L:");
    //     Serial.print(leftMotor.getDistanceTraveled());
    //     Serial.print(" R:");
    //     Serial.print(rightMotor.getDistanceTraveled());
    //     Serial.println();
    // }

    //     usonicOD.sendOverBLE(&bluetoothLowEnergy);
    //
    //     FLIROD.sendOverBLE(&bluetoothLowEnergy);
    //     FRIROD.sendOverBLE(&bluetoothLowEnergy);
    //     LIROD.sendOverBLE(&bluetoothLowEnergy);
    //     RIROD.sendOverBLE(&bluetoothLowEnergy);

    //     motionTracker.moveToTarget();
    //
    //     pixels.point(-motionTracker.getLocalAngleToTurn());
    // //
    //     if (eachSecond.isReadyToRun()) {
    //         static int count = 0;
    //
    //         if (count == 0) {
    //             motionTracker.setTargetPosition(0, 300);
    //         }
    //         if (count == 1) {
    //             motionTracker.setTargetPosition(300, 300);
    //         }
    //         if (count == 2) {
    //             motionTracker.setTargetPosition(300, 0);
    //         }
    //         if (count == 3) {
    //             motionTracker.setTargetPosition(0, 0);
    //         }
    //         count++;
    //         count %= 4;
    //     }

    //     if (Serial.available() > 0) {
    //         // This part was stolend from gpt
    //         String inputString = "";
    //         // Read the incoming string until newline character
    //         inputString = Serial.readStringUntil('\n');
    //
    //         // If you want to parse two integers separated by a comma
    //         int commaIndex =
    //             inputString.indexOf(',');  // Find the position of the
    //             comma
    //         if (commaIndex != -1) {        // Check if a comma was found
    //             int firstNumber = inputString.substring(0, commaIndex)
    //                                   .toInt();  // Extract first number
    //             int secondNumber = inputString.substring(commaIndex + 1)
    //                                    .toInt();  // Extract second
    //                                    number
    //
    //             motionTracker.setTargetPosition(firstNumber,
    //             secondNumber);
    //         }
    //     }

    //     static PassiveSchedule printer(1000);
    //     static uint32_t lastTime;
    //     static uint32_t thisTime;
    //     static uint32_t duration;
    //     lastTime = thisTime;
    //     thisTime = micros();
    //
    //     duration = thisTime - lastTime;
    //
    //     if (printer.isReadyToRun()) {
    //         Serial.print(" Duration:");
    //         Serial.println(duration);
    //     }
}