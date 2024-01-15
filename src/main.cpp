
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
#include "map.h"
#include "motionTracker.h"
#include "motor.h"
#include "navigator.h"
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
                      ULTRASONIC_DATA_SHELF_LIFE, 85);

Infrared leftInfrared(&errorIndicator, LEFT_INFRARED_INDEX, 85);
Infrared rightInfrared(&errorIndicator, RIGHT_INFRARED_INDEX, 85);
Infrared frontLeftInfrared(&errorIndicator, FRONT_LEFT_INFRARED_INDEX, 64);
Infrared frontRightInfrared(&errorIndicator, FRONT_RIGHT_INFRARED_INDEX, 64);

Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN,
              BUMPER_ROTATION_OFFSET);

BluetoothLowEnergy bluetoothLowEnergy(&errorIndicator, MAIN_SERVICE_UUID,
                                      ROBOT_POSE_UUID, BRICK_UUID,
                                      NEEDS_DATA_UUID);

MotionTracker motionTracker(&leftMotor, &rightMotor, &frontLeftInfrared,
                            &frontRightInfrared);

Navigator navigator(&motionTracker, &drive);

BrickList brickList;

Angle globalTargetAngle = 0;

Map gridMap;

// void updateFollowAngle(Pose currentPose) {
//     Point directions[8] = {{1, 0},  {1, 1},   {0, 1},  {-1, 1},
//                            {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
//
//     if (brickList.nearDeadzone(currentPose.position)) {
//         return;
//     }
//
//     int currentXCM = (currentPose.position.x / 10);
//     int currentYCM = (currentPose.position.y / 10);
//
//     Point innerPoint = {currentXCM, currentYCM};
//
//     int16_t innerValue = map[innerPoint.y][innerPoint.x];
//     int16_t LowestValue = INT16_MAX;
//     int lowestIndex = -1;
//     // Serial.print("Inner");
//     // Serial.print(innerValue);
//     // Serial.println(",");
//     // Serial.print("LowestValue");
//     // Serial.print(LowestValue);
//     // Serial.println(",");
//
//     for (int D = 0; D < 8; D++) {
//         Point outerPoint = innerPoint + directions[D];
//
//         int16_t outerValue = map[outerPoint.y][outerPoint.x];
//         // Serial.print(outerValue);
//         // Serial.print(",");
//
//         bool isValid = validatePoint(outerPoint);
//         bool notWall = (map[outerPoint.y][outerPoint.x] != -1);
//         bool lessThanInner = outerValue < innerValue;
//         bool lessThanLowest = outerValue < LowestValue;
//         // Serial.print(" isValid:");
//         // Serial.print(isValid);
//         // Serial.print(" notWall:");
//         // Serial.print(notWall);
//         // Serial.print(" lessThanInner:");
//         // Serial.print(lessThanInner);
//         // Serial.print(" lessThanLowest:");
//         // Serial.print(lessThanLowest);
//
//         if (isValid && notWall && lessThanInner && lessThanLowest) {
//             LowestValue = outerValue;
//             lowestIndex = D;
//             // Serial.print("New LowestValue");
//             // Serial.print(LowestValue);
//         }
//         // Serial.println(" END");
//     }
//
//     if (lowestIndex != -1) {
//         Angle newAngle = (45 * lowestIndex);
//         globalTargetAngle = newAngle;
//     }
// }

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

void polls() {
    frontLeftInfrared.poll();
    frontRightInfrared.poll();
    leftInfrared.poll();
    rightInfrared.poll();

    ultrasonic.poll();

    // TODO strip BLE from motionTracker

    motionTracker.poll();

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
    DriveTaxi,
    WallsInMyFace,
    FollowingPath,
    Chilling,
    Ready,
    Mazing
};
const String SystemStateString[] = {"DriveTaxi",     "WallsInMyFace",
                                    "FollowingPath", "Chilling",
                                    "Ready",         "Mazing"};

void printState(SystemState stateToPrint) {
    Serial.print("Current State:");
    Serial.println(SystemStateString[(int)stateToPrint]);
}
const SystemState startingSystemState = Mazing;
SystemState systemState = startingSystemState;

bool readyToGo = false;

////--------------------

#include "mazeConstants.h"

// TODO finish this code
void lineToBlock(Position startPoint, Position endPoint,
                 Position robotPosition) {
    int tolerance = 20;

    float xLength = abs(startPoint.x - endPoint.x);
    float yLength = abs(startPoint.y - endPoint.y);

    Position lineCentrePoint;
    lineCentrePoint.x = (startPoint.x + endPoint.x) / 2;
    lineCentrePoint.y = (startPoint.y + endPoint.y) / 2;

    // Serial.print(" Start:");
    // Serial.println(startPoint);
    // Serial.print(" End:");
    // Serial.println(endPoint);
    // Serial.print(" LinePont:");
    // Serial.println(lineCentrePoint);

    float drift = min(xLength, yLength);
    float length = max(xLength, yLength);

    if (drift > tolerance) {
        Serial.println("Wonky Ass line");
        return;
    }

    bool robotIsLeftOfLine = (robotPosition.x < lineCentrePoint.x);

    bool robotIsBelowLine = (robotPosition.y < lineCentrePoint.y);

    bool lineInVertical = (yLength > xLength);

    if (lineInVertical) {
        Serial.print("Vertical Line ");
        Serial.print(length);
        Serial.println(" Long.");

        if ((length > 35) && (length < 90)) {
            Serial.println("SHORT SIDE OF BRICK");

            Brick brickToAdd;

            brickToAdd.position.y = lineCentrePoint.y;
            brickToAdd.isVertical = false;

            if (robotIsLeftOfLine) {
                brickToAdd.position.x = lineCentrePoint.x + (BRICK_LENGTH / 2);
            } else {
                brickToAdd.position.x = lineCentrePoint.x - (BRICK_LENGTH / 2);
            }
            brickList.TEMP_addBrick(brickToAdd, 1);

            brickToAdd.position.x -= 250;

            brickList.TEMP_addBrick(brickToAdd, 0);
        }
    } else {
        // Serial.print("Horizontal Line ");
        // Serial.print(length);
        // Serial.println(" Long.");
    }
}

void stopAndSend() {
    // return;  // TODO remove

    drive.stop();

    delay(100);

    Pose robotPose = motionTracker.getPose();

    Serial.print("Sending :");
    Serial.println(robotPose);

    bluetoothLowEnergy.sendRobotPose(robotPose);

    bluetoothLowEnergy.sendBrickList(brickList);
}

Position realWorldCoords(Pose robotPose, Pose localSensorPose,
                         int sensorDistance) {
    Position positionToReturn;

    // Set the position of the detected obstacle relative to the
    // sensor.
    positionToReturn.y = sensorDistance;

    // Transform the position from being relative to the sensor, to
    // being relative to the robot.
    positionToReturn.transformByPose(localSensorPose);

    // Transform the position from being relative to the robot, to
    // being relative to the global coordinate system.
    positionToReturn.transformByPose(robotPose);

    return positionToReturn;
}

// TODO fix this lol
void runAwayFromBumper(byte bumperData) {
    if (bumperData) {
        navigator.hitBumper(bumperData);
        systemState = FollowingPath;
    }
}

Position leftStatingCorner;
Position leftEndingCorner;

void setStatingPosition() {
    // Serial.print(" Usonic Wall:");
    // Serial.print(ultrasonic.readFromRobotCenter());
    // Serial.print(" :");
    // Serial.println();

    // This code is about the be written really badly
    // But why bother refactoring when this code is only ran during startup.
    // Its not like were working with GTA 5 Loading times here, theres not much
    // need for optimization at this stage.

    static int innerStage = 0;
    // 1 - turn 90, to 180°
    // 2 - Alight with the left wall, this is the wall where x = 0;
    // 3 - read the distance from the wall, and set that to the robots x
    // position
    // 4 - turn 90, to -90°
    // 5 - Alight with the Back wall, where y = 0;
    // 6 - Set the robots Y position from the ultrasonic reading.
    // 7  - turn 180, to 90°
    // Then start the program

    pixels.setAll(0, 255, 255);

    int FLDist;
    int FRDist;

    switch (innerStage) {
        case 0:
            navigator.turnLeft();
            innerStage++;
            break;
        case 1:
            navigator.moveToTarget();
            if (navigator.hasNoPath()) {
                innerStage++;
            }
            break;
        case 2:
            pixels.setAll(0, 0, 255);

            // TODO add the bit to catch misalignment.
            FLDist = frontLeftInfrared.readSafe();
            FRDist = frontRightInfrared.readSafe();

            if (FLDist < FRDist) {
                drive.turnLeft();
            } else if (FLDist > FRDist) {
                drive.turnRight();
            } else {
                drive.stop();
                motionTracker.umActually();
                innerStage++;
            }

            break;

        case 3:

            motionTracker.setInitialX(ultrasonic.readFromRobotCenter());

            navigator.turnLeft();
            innerStage++;

            break;

        case 4:
            navigator.moveToTarget();
            if (navigator.hasNoPath()) {
                innerStage++;
            }
            break;

        case 5:
            pixels.setAll(0, 0, 255);

            // TODO add the bit to catch misalignment.
            FLDist = frontLeftInfrared.readSafe();
            FRDist = frontRightInfrared.readSafe();

            if (FLDist < FRDist) {
                drive.turnLeft();
            } else if (FLDist > FRDist) {
                drive.turnRight();
            } else {
                drive.stop();
                motionTracker.umActually();
                innerStage++;
            }

            break;

        case 6:
            motionTracker.setInitialY(ultrasonic.readFromRobotCenter());

            navigator.turn(180);
            innerStage++;

            break;

        case 7:
            navigator.moveToTarget();
            if (navigator.hasNoPath()) {
                readyToGo = true;
            }
            break;
    }
}

void driveTaxi() {
    Pose robotPose = motionTracker.getPose();

    Angle closest90 = robotPose.angle.toClosestRightAngle();
    int taxiOffsetAngle = closest90 - robotPose.angle;

    drive.forwards(taxiOffsetAngle);

    int frontUSDistance = ultrasonic.read();

    if (leftInfrared.brickAppeared(150, 50)) {
        float distanceToWall = leftInfrared.readFromRobotCenter(false);
        // Serial.print("Wall appeared ");
        // Serial.print(distanceToWall);
        // Serial.println(" cm away.");

        leftStatingCorner = {-(distanceToWall), 0};
        leftStatingCorner.transformByPose(robotPose);
    };

    if (leftInfrared.brickDisappeared(150, 50)) {
        float distanceToWall = leftInfrared.readFromRobotCenter(true);
        // Serial.print("Wall disappeared ");
        // Serial.print(distanceToWall);
        // Serial.println(" cm away.");

        leftEndingCorner = {-(distanceToWall), 0};
        leftEndingCorner.transformByPose(robotPose);

        lineToBlock(leftStatingCorner, leftEndingCorner, robotPose.position);
        stopAndSend();

        navigator.turnLeft(150);
        systemState = FollowingPath;
    }

    // if a wall is there
    if (frontUSDistance < 100 && frontUSDistance != -1) {
        stopAndSend();
        systemState = WallsInMyFace;
    }
}

void aligning() {
    int FLDist = frontLeftInfrared.readSafe();
    int FRDist = frontRightInfrared.readSafe();

    bool cantSeeLeft = (FLDist <= -1 || FLDist > 200);
    bool cantSeeRight = (FRDist <= -1 || FRDist > 200);

    if (cantSeeLeft || cantSeeRight) {
        Serial.println("I cant read this");

        navigator.turnRight();
        systemState = FollowingPath;
        return;
    }

    if (FLDist < FRDist) {
        drive.turnLeft();
    } else if (FLDist > FRDist) {
        drive.turnRight();
    } else {  // is aligned
        drive.stop();

        motionTracker.umActually();  // Sets the current angle to the closest 90

        navigator.turnRight();
        systemState = FollowingPath;
    }
}

void followPath() {
    if (navigator.hasNoPath()) {
        systemState = startingSystemState;
    } else {
        navigator.moveToTarget();
    }
}

void chilling() { drive.stop(); }

void ready() {
    drive.stop();

    if (ultrasonic.read() < 200) {
        systemState = DriveTaxi;
    }
}

void doMaze() {
    //     Pose currentPose = motionTracker.getPose();
    //
    //     updateFollowAngle(currentPose);
    //
    //     Serial.print(" Angle:");
    //     Serial.println(globalTargetAngle);
    //
    //     Angle angleToTurn = globalTargetAngle - currentPose.angle;
    //
    //     if (angleToTurn > 45) {
    //         drive.turnLeft();
    //     } else if (angleToTurn < -45) {
    //         drive.turnRight();
    //     } else {
    //         int angleInt = (int)angleToTurn;
    //         int offest = constrain(angleInt, -10, 10);
    //         drive.forwards(offest);
    //     }
}

void doState(SystemState stateToExecute) {
    // printState(stateToExecute);

    switch (stateToExecute) {
        case DriveTaxi:
            pixels.setAll(255, 255, 255);
            driveTaxi();
            break;

        case WallsInMyFace:
            pixels.setAll(0, 0, 255);
            aligning();
            break;

        case FollowingPath:
            pixels.setAll(0, 255, 0);
            followPath();
            break;

        case Chilling:
            pixels.setAll(255, 105, 180);
            chilling();
            break;
        case Ready:
            pixels.setAll(105, 105, 0);
            ready();

            break;
        case Mazing:
            pixels.setAll(255, 0, 255);
            doMaze();
            break;
    }
}

// void findEdges() {
//     Point directions[8] = {{1, 0},  {1, 1},   {0, 1},  {-1, 1},
//                            {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
//
//     for (int y = 0; y < MAX_Y; y++) {
//         for (int x = 0; x < MAX_X; x++) {
//             Point iterativePoint = {x, y};
//
//             bool isDeadZone = (map[iterativePoint.y][iterativePoint.x] ==
//             -1);
//
//             if (isDeadZone) {
//                 for (int D = 0; D < 8; D++) {
//                     Point newPoint = iterativePoint + directions[D];
//                     int outerValue = map[newPoint.y][newPoint.x];
//
//                     if (outerValue != -1) {
//                         // Serial.println(outerValue);
//                         Serial.print("self.coordsToPoint(");
//                         Serial.print(iterativePoint.x);
//                         Serial.print(", ");
//                         Serial.print(iterativePoint.y);
//                         Serial.println(")");
//                         break;
//                     }
//                 }
//             }
//         }
//     }
// }

PassiveSchedule five(5000);

void loop() {
    polls();

    // if (eachSecond.isReadyToRun()) {
    //     Serial.print(" Pose:");
    //     Serial.println(motionTracker.getPose());
    // }

    // Serial.println(brickList.toString());

    if (five.isReadyToRun()) {
        // findEdges();
    }

    // byte bumperValue = bumper.read();
    // if (bumperValue && readyToGo) {
    //     runAwayFromBumper(bumperValue);
    // }

    // if (second.isReadyToRun()) {
    //     Serial.println(motionTracker.getPose());
    // }

    pixels.clear();
    //
    //     if (readyToGo) {
    //         doState(systemState);
    //     } else {
    //         setStatingPosition();
    //     }

    pixels.clear();
    pixels.show();

    // if (five.isReadyToRun()) {
    //     Brick brick = brickList.getBrick(0);

    //         for (int i = 0; i < 40; i++) {
    //             for (int j = 0; j < 60; j++) {
    //                 int y = (i * 10) + 100;
    //                 int x = (j * 10) + 200;
    //
    //                 Position scanPosition = {x, y};
    //
    //                 int distance = brick.squaredDistanceTo(scanPosition);
    //
    //                 distance = sqrt(distance);
    //
    //                 distance = constrain(distance, 0, 100);
    //
    //                 gridMap.jankyPrintPosition(scanPosition, distance);
    //             }
    //         }

    //         Serial.println("Here1");
    //

    //         // gridMap.print();
    //         // gridMap.printDir();
    //
    //         // gridMap.JankyPrintPath2({5, 5});
    //         Serial.println("Here6");
    // }

    if (Serial.available() > 0) {
        String data = Serial.readString();
        const int maxArgs = 10;
        String args[maxArgs];
        int foundArgs = 0;
        int spaceIndex = 0;

        while ((data.length() > 0) && (foundArgs < maxArgs) &&
               (spaceIndex != -1)) {
            spaceIndex = data.indexOf(" ");
            args[foundArgs] = data.substring(0, spaceIndex);
            data = data.substring(spaceIndex + 1);
            foundArgs++;
        }

        if (args[0] == "get-csv") {
            brickList.TEMP_fillWithTestData();

            gridMap.fillFromBrickList(brickList);

            gridMap.solve({12, 180});

            int samplingInterval = 5;
            if (foundArgs > 1) {
                samplingInterval = args[1].toInt();
            }
            gridMap.printDataToCSV(samplingInterval);
        }
        if (args[0] == "get-bin") {
            brickList.TEMP_fillWithTestData();

            gridMap.fillFromBrickList(brickList);

            gridMap.solve({12, 180});

            int samplingInterval = 5;
            if (foundArgs > 1) {
                samplingInterval = args[1].toInt();
            }
            gridMap.printDataToBIN(samplingInterval);
        }
    }
}

// Short Sides are looking like 56,49
// Long side looks like 222,228
// Readings are seeming to be like 20-30 mm shy

// TODO swap distance comparison for squared length comparison.

//
//  if (Serial.available() > 0) {
//         // This part was stolen from gpt
//         String inputString = "";
//         // Read the incoming string until newline character
//         inputString = Serial.readStringUntil('\n');
//
//         // If you want to parse two integers separated by a comma
//         int commaIndex =
//             inputString.indexOf(',');  // Find the position of the coma
//         if (commaIndex != -1) {        // Check if a comma was found
//             float firstNumber = inputString.substring(0, commaIndex)
//                                     .toInt();  // Extract first number
//             float secondNumber = inputString.substring(commaIndex + 1)
//                                      .toInt();  // Extract second number
//
//             Serial.println("Here1");
//
//             gridMap.fillFromBrickList(brickList);
//
//             gridMap.solve({15, 15});
//             gridMap.print();
//             Serial.println();
//
//             gridMap.printDir();
//
//             gridMap.JankyPrintPath({5, 5});
//             Serial.println("Here6");
//         }
//     }