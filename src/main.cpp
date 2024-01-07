
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
#include "navigator.h"
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
                                      ROBOT_POSE_UUID, BRICK_UUID,
                                      NEEDS_DATA_UUID);

MotionTracker motionTracker(&bluetoothLowEnergy, &leftMotor, &rightMotor,
                            &frontLeftInfrared, &frontRightInfrared);

Navigator navigator(&motionTracker, &drive);

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

enum SystemState {
    DrivingForwards,
    GoingToTarget,
    Aligning,
    InFrontOfWall,  // -----------------------
    Chilling
};
const String SystemStateString[] = {"DrivingForwards", "GoingToTarget",
                                    "Aligning", "InFrontOfWall", "Chilling"};

void printState(SystemState stateToPrint) {
    Serial.print("Current State:");
    Serial.println(SystemStateString[(int)stateToPrint]);
}
SystemState systemState = GoingToTarget;

///

void DriveForwards() {
    drive.forwards();

    int distance = ultrasonic.read();

    // if a wall if his from the left bumper
    if (bumper.read() & 128) {
        drive.stop();
        systemState = GoingToTarget;
    }

    // if a wall is there
    if (distance < 100 && distance != -1) {
        drive.stop();
        systemState = Aligning;
    }
}
void goingToTarget() {
    // TODO change this to a .inRange() function

    //     int dist = motionTracker.getDistanceFromTarget();
    //
    //     Serial.println(dist);
    //
    //     if (dist < 10) {
    //         systemState = Chilling;
    //
    //     } else {
    //         motionTracker.moveToTarget();
    //     }
}

void aligning() {
    const Angle angleTolerance = 0;
    Angle angleToAlign = motionTracker.angleFromFrontIR();

    if (angleToAlign < -angleTolerance) {
        drive.turnRight();
    } else if (angleToAlign > angleTolerance) {
        drive.turnLeft();
    } else {
        drive.stop();
        systemState = InFrontOfWall;
    }
}

void measureWall() {
    enum MeasurementState { TurningLeft, TurningRight, DoneMeasuring };
    static MeasurementState interState = TurningLeft;

    switch (interState) {
        case TurningLeft:

            Serial.println("LEft");
            interState = TurningRight;
            break;
        case TurningRight:

            interState = DoneMeasuring;
            Serial.println("Right");

            break;
        case DoneMeasuring:

            Serial.println("Done");

            interState = TurningLeft;

            break;
    }
}

void chilling() {
    if (bumper.read()) {
        // navigator.setTargetPosition(100, 100);
        systemState = GoingToTarget;
    }
}

void doState(SystemState stateToExecute) {
    printState(stateToExecute);

    switch (stateToExecute) {
        case DrivingForwards:
            DriveForwards();
            break;
        case GoingToTarget:
            goingToTarget();
            break;
        case Aligning:
            aligning();

            break;
        case InFrontOfWall:
            measureWall();

            break;
        case Chilling:
            chilling();

            break;
    }
}

PassiveSchedule send(1000);

void loop() {
    polls();

    // navigator.moveToTarget();

    // doState(systemState);

    // doState(systemState);

    if (Serial.available() > 0) {
        // This part was stolend from gpt
        String inputString = "";
        // Read the incoming string until newline character
        inputString = Serial.readStringUntil('\n');

        // If you want to parse two integers separated by a comma
        int commaIndex =
            inputString.indexOf(',');  // Find the position of the coma
        if (commaIndex != -1) {        // Check if a comma was found
            int firstNumber = inputString.substring(0, commaIndex)
                                  .toInt();  // Extract first number
            int secondNumber = inputString.substring(commaIndex + 1)
                                   .toInt();  // Extract second number

            navigator.setTargetPosition(firstNumber, secondNumber);

            drive.forwards(firstNumber);
        }
    }
}