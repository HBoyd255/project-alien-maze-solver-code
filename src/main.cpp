
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

// TODO decide which order the uuids go
BluetoothLowEnergy bluetoothLowEnergy(&errorIndicator, MAIN_SERVICE_UUID,
                                      ROBOT_POSE_UUID, BRICK_UUID, CORNER_UUID);

MotionTracker motionTracker(&leftMotor, &rightMotor, &frontLeftInfrared,
                            &frontRightInfrared, -90);

Navigator navigator(&motionTracker, &drive);

BrickList brickList;

Map gridMap;

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

    gridMap.primeForTracking();

    // motionTracker._currentPosition = {750, 200};

    // gridMap.TEMP_cutMagicLine();

    // gridMap.solve(brickList, {130, 180});
}

bool readyToGo = true;

void polls() {
    frontLeftInfrared.poll();
    frontRightInfrared.poll();
    leftInfrared.poll();
    rightInfrared.poll();

    ultrasonic.poll();

    // TODO strip BLE from motionTracker

    motionTracker.poll();

    bluetoothLowEnergy.poll();
}

// Forward declaration of states.
void followingLeftWall_S();
void aligningWithWall_S();
void stopped_S();
void followingMaze_S();
void celebrating_S();

void UseMazeToGoTo(Position);

const voidFuncPtr startingState_GCP = stopped_S;
voidFuncPtr nextState_GP = startingState_GCP;
// TODO add a default state basest on currentGoal

enum Goal {
    NoGoal,
    MapOuter,
    Launch1,
    DriveToEndMaybe,
    MapInner,
    DriveToStart,
    DriveToEnd,
    Won,

};

Goal currentGoal_G = MapOuter;
////--------------------

#include <queue>

#include "mazeConstants.h"

struct Corner {
    Position position;
    uint8_t index;

    Corner(Position position = {0, 0}, int index = 0)
        : position(position), index(index){};
};

std::queue<Corner> cornerQueue;

void sendData() {
    Pose robotPose = motionTracker.getPose();
    bluetoothLowEnergy.sendRobotPose(robotPose);

    bluetoothLowEnergy.sendBrickList(brickList);

    while (!cornerQueue.empty()) {
        // Corner cornerToSend = cornerQueue.front();
        // bluetoothLowEnergy.sendCorner(cornerToSend.position,
        //                               cornerToSend.index);
        cornerQueue.pop();
    }
}

void maybePlonkAgainstWall(Position startPoint) {
    bool onBottomWall = (startPoint.y < 150);
    bool onLeftWall = (startPoint.x < 150);
    bool onTopWall = (startPoint.y > 1850);
    bool onRightWall = (startPoint.x > 1350);

    if (onBottomWall) {
        Brick brickToAdd;

        brickToAdd.position.x = startPoint.x - 125;
        brickToAdd.position.y = 45;
        brickToAdd.isVertical = false;

        brickList.attemptAppendBrick(brickToAdd);
    }
    if (onLeftWall) {
        Brick brickToAdd;

        brickToAdd.position.x = 45;
        brickToAdd.position.y = startPoint.y + 125;
        brickToAdd.isVertical = true;

        brickList.attemptAppendBrick(brickToAdd);
    }
    if (onTopWall) {
        Brick brickToAdd;

        brickToAdd.position.x = startPoint.x + 125;
        brickToAdd.position.y = 1955;
        brickToAdd.isVertical = false;

        brickList.attemptAppendBrick(brickToAdd);
    }
    if (onRightWall) {
        Brick brickToAdd;

        brickToAdd.position.x = 1455;
        brickToAdd.position.y = startPoint.y - 125;
        brickToAdd.isVertical = true;

        brickList.attemptAppendBrick(brickToAdd);
    }
}

// TODO finish this code
void twoCornersToBrick(Position robotPosition, Position startPoint,
                       Position endPoint) {
    float xLength = abs(startPoint.x - endPoint.x);
    float yLength = abs(startPoint.y - endPoint.y);

    Position lineCentrePoint;
    lineCentrePoint.x = (startPoint.x + endPoint.x) / 2;
    lineCentrePoint.y = (startPoint.y + endPoint.y) / 2;

    float drift = min(xLength, yLength);
    float length = max(xLength, yLength);
    //
    //     Serial.print(" Start:");
    //     Serial.print(startPoint);
    //     Serial.print(" End:");
    //     Serial.print(endPoint);
    //     Serial.print(" LinePont:");
    //     Serial.println(lineCentrePoint);
    //     Serial.print(" length:");
    //     Serial.println(length);
    //     Serial.print(" drift:");
    //     Serial.println(drift);

    // Tolerance of about 14 degrees
    // USE trig. dont calculate on fly
    // calculator.net/triangle-calculator.html
    if ((drift * 4) > length) {
        // Serial.println("Wonky Ass line");

        return;
    }

    bool robotIsLeftOfLine = (robotPosition.x < lineCentrePoint.x);
    bool robotIsBelowLine = (robotPosition.y < lineCentrePoint.y);

    bool lineInVertical = (yLength > xLength);

    bool hasSentABrickOff = false;

    if (lineInVertical) {
        // Serial.print("Vertical Line ");
        // Serial.print(length);
        // Serial.println(" Long.");

        if ((length > 40) && (length < 120)) {
            // Serial.println("SHORT SIDE OF BRICK");

            Brick brickToAdd;

            brickToAdd.position.y = lineCentrePoint.y;
            brickToAdd.isVertical = false;

            if (robotIsLeftOfLine) {
                brickToAdd.position.x = lineCentrePoint.x + (BRICK_LENGTH / 2);
            } else {
                brickToAdd.position.x = lineCentrePoint.x - (BRICK_LENGTH / 2);
            }
            brickList.attemptAppendBrick(brickToAdd);
            hasSentABrickOff = true;
        }
        if ((length > 210) && (length < 290)) {
            // Serial.println("LONG SIDE OF BRICK");

            Brick brickToAdd;

            brickToAdd.position.y = lineCentrePoint.y;
            brickToAdd.isVertical = true;

            if (robotIsLeftOfLine) {
                brickToAdd.position.x = lineCentrePoint.x + (BRICK_WIDTH / 2);
            } else {
                brickToAdd.position.x = lineCentrePoint.x - (BRICK_WIDTH / 2);
            }
            brickList.attemptAppendBrick(brickToAdd);
            hasSentABrickOff = true;
        }

    } else {  // Horizontal Line
        // Serial.print("Horizontal Line ");
        // Serial.print(length);
        // Serial.println(" Long.");

        if ((length > 40) && (length < 120)) {
            // Serial.println("SHORT SIDE OF BRICK");

            Brick brickToAdd;

            brickToAdd.position.x = lineCentrePoint.x;
            brickToAdd.isVertical = true;

            if (robotIsBelowLine) {
                brickToAdd.position.y = lineCentrePoint.y + (BRICK_LENGTH / 2);
            } else {
                brickToAdd.position.y = lineCentrePoint.y - (BRICK_LENGTH / 2);
            }
            brickList.attemptAppendBrick(brickToAdd);
            hasSentABrickOff = true;
        }
        if ((length > 210) && (length < 290)) {
            // Serial.println("LONG SIDE OF BRICK");

            Brick brickToAdd;

            brickToAdd.position.x = lineCentrePoint.x;
            brickToAdd.isVertical = false;

            if (robotIsBelowLine) {
                brickToAdd.position.y = lineCentrePoint.y + (BRICK_WIDTH / 2);
            } else {
                brickToAdd.position.y = lineCentrePoint.y - (BRICK_WIDTH / 2);
            }
            brickList.attemptAppendBrick(brickToAdd);
            hasSentABrickOff = true;
        }
    }
    if (hasSentABrickOff) {
        sendData();
    }
}

// TODO rename
bool blockyBlockBlock(Position robotPosition, Angle angleOfSensor,
                      int measuredDistance) {
    bool needsRecalibration = false;

    if (!angleOfSensor.isOrthogonal()) {
        // This function only works with orthogonal angles
        return false;
    }

    // if the thing is octagonally upwards.

    bool pointingDown = angleOfSensor.isOrthogonallyDown();
    bool pointingLeft = angleOfSensor.isOrthogonallyLeft();
    bool pointingUp = angleOfSensor.isOrthogonallyUp();
    bool pointingRight = angleOfSensor.isOrthogonallyRight();

    Position brickEdgePosition;

    if (pointingDown) {
        brickEdgePosition = {0, -(float)measuredDistance};
    } else if (pointingLeft) {
        brickEdgePosition = {-(float)measuredDistance, 0};
    } else if (pointingUp) {
        brickEdgePosition = {0, (float)measuredDistance};
    } else if (pointingRight) {
        brickEdgePosition = {(float)measuredDistance, 0};
    }

    brickEdgePosition += robotPosition;

    int comparison =
        brickList.compare(robotPosition, angleOfSensor, measuredDistance);

    if (comparison == 1) {
        // Serial.println("Comp 1");

        Position positionA;
        Position positionB;

        Position positionC;
        Position positionD;

        if (pointingDown) {
            if (brickEdgePosition.y < 250) {
                return false;
            }

            positionA.x = brickEdgePosition.x + BRICK_WIDTH / 2;
            positionA.y = brickEdgePosition.y;
            positionB.x = brickEdgePosition.x - BRICK_WIDTH / 2;
            positionB.y = brickEdgePosition.y - BRICK_LENGTH;

            positionC.x = brickEdgePosition.x + BRICK_LENGTH / 2;
            positionC.y = brickEdgePosition.y;
            positionD.x = brickEdgePosition.x - BRICK_LENGTH / 2;
            positionD.y = brickEdgePosition.y - BRICK_WIDTH;
        }

        if (pointingLeft) {
            if (brickEdgePosition.x < 250) {
                return false;
            }
            positionA.x = brickEdgePosition.x;
            positionA.y = brickEdgePosition.y - BRICK_WIDTH / 2;
            positionB.x = brickEdgePosition.x - BRICK_LENGTH;
            positionB.y = brickEdgePosition.y + BRICK_WIDTH / 2;

            positionC.x = brickEdgePosition.x;
            positionC.y = brickEdgePosition.y - BRICK_LENGTH / 2;
            positionD.x = brickEdgePosition.x - BRICK_WIDTH;
            positionD.y = brickEdgePosition.y + BRICK_LENGTH / 2;
        }
        if (pointingUp) {
            if (brickEdgePosition.y > 1750) {
                return false;
            }
            positionA.x = brickEdgePosition.x - BRICK_WIDTH / 2;
            positionA.y = brickEdgePosition.y;
            positionB.x = brickEdgePosition.x + BRICK_WIDTH / 2;
            positionB.y = brickEdgePosition.y + BRICK_LENGTH;

            positionC.x = brickEdgePosition.x - BRICK_LENGTH / 2;
            positionC.y = brickEdgePosition.y;
            positionD.x = brickEdgePosition.x + BRICK_LENGTH / 2;
            positionD.y = brickEdgePosition.y + BRICK_WIDTH;
        }
        if (pointingRight) {
            if (brickEdgePosition.x > 1250) {
                return false;
            }
            positionA.x = brickEdgePosition.x;
            positionA.y = brickEdgePosition.y + BRICK_WIDTH / 2;
            positionB.x = brickEdgePosition.x + BRICK_LENGTH;
            positionB.y = brickEdgePosition.y - BRICK_WIDTH / 2;

            positionC.x = brickEdgePosition.x;
            positionC.y = brickEdgePosition.y + BRICK_LENGTH / 2;
            positionD.x = brickEdgePosition.x + BRICK_WIDTH;
            positionD.y = brickEdgePosition.y - BRICK_LENGTH / 2;
        }

        bool hasRanOffMaze = false;
        bool brickCouldBeVert =
            gridMap.safeForBrick(positionA, positionB, &hasRanOffMaze);

        // disregard any values that are too close to the edge.
        if (hasRanOffMaze) {
            return false;
        }
        bool brickCouldBeHor =
            gridMap.safeForBrick(positionC, positionD, &hasRanOffMaze);

        if (hasRanOffMaze) {
            return false;
        }

        if (brickCouldBeVert) {
        } else if (brickCouldBeHor) {
            brickList.setBrickFromApproximation(brickEdgePosition,
                                                angleOfSensor);
        }
    } else if (comparison == -1) {
        needsRecalibration = true;
    }
    return needsRecalibration;
}

void followingLeftWall_S() {
    Pose robotPose = motionTracker.getPose();
    Position robotPosition = motionTracker.getPosition();
    Angle robotAngle = motionTracker.getAngle();

    static Goal lastGoal = NoGoal;
    bool goalChanged = lastGoal != currentGoal_G;
    lastGoal = currentGoal_G;

    static Position otherSideOfMaze;

    static int halfLaps;

    static Position innerStartPosition;
    static bool innerBeenFarEnough;

    if (goalChanged) {
        if (currentGoal_G == MapOuter) {
            otherSideOfMaze = {1300, 1800};
            halfLaps = 0;
        }
        if (currentGoal_G == MapInner) {
            innerStartPosition = robotPosition;
            innerBeenFarEnough = false;
        }
    }

    static Position leftStatingCorner;
    static Position leftEndingCorner;
    static Position rightStatingCorner;
    static Position rightEndingCorner;

    if (currentGoal_G == MapOuter) {
        const int targetTolerance_C = 200;

        // TODO refactor
        int squaredDistTotherSide =
            robotPosition.calculateSquaredDistanceTo(otherSideOfMaze);

        if (squaredDistTotherSide < (targetTolerance_C * targetTolerance_C)) {
            halfLaps += 1;
            bool halfLapsIsOdd = halfLaps & 1;
            if (halfLapsIsOdd) {
                otherSideOfMaze = {200, 200};
            } else {
                otherSideOfMaze = {1300, 1800};
            }

            if (halfLaps == 2) {
                currentGoal_G = Launch1;
            }
        }
    }
    if (currentGoal_G == MapInner) {
        // TODO refactor

        bool doneInnerLap = false;

        const int innerOuterTolerance = 150;
        const int innerInnerTolerance = 75;

        int squaredDistFromInnerStart =
            robotPosition.calculateSquaredDistanceTo(innerStartPosition);

        if (squaredDistFromInnerStart >
            (innerOuterTolerance * innerOuterTolerance)) {
            innerBeenFarEnough = true;
        }

        if (squaredDistFromInnerStart <
            (innerInnerTolerance * innerInnerTolerance)) {
            if (innerBeenFarEnough) {
                doneInnerLap = true;
            }
        }
        if (doneInnerLap) {
            currentGoal_G = DriveToStart;
            UseMazeToGoTo({200, 200});
        }
    }

    int orthogonalOffset = robotAngle.OrthogonalOffset();
    drive.forwards(orthogonalOffset);

    int frontUSDistance = ultrasonic.readFromRobotCenter();

    if (abs(robotAngle.isOrthogonal()) <= 2) {
        Angle roundedRobotAngle = robotAngle.closestOrthogonal();

        static PassiveSchedule RENAME_ME(100);

        static int ALSO_RENAME_ME = 0;

        if (RENAME_ME.isReadyToRun()) {
            int leftIRDistance = leftInfrared.readFromRobotCenter();
            // int rightIRDistance = rightInfrared.readFromRobotCenter();

            if ((ALSO_RENAME_ME == 0) && (frontUSDistance > 120)) {
                blockyBlockBlock(robotPosition, roundedRobotAngle,
                                 frontUSDistance);
            } else if ((ALSO_RENAME_ME == 1) && (leftIRDistance > 120)) {
                bool needsCalibrating = blockyBlockBlock(
                    robotPosition, roundedRobotAngle + 90, leftIRDistance);

                if (needsCalibrating) {
                    // navigator.turnLeft();
                }
            }

            // else if ((ALSO_RENAME_ME == 2) && (rightIRDistance > 120)) {
            //     blockyBlockBlock(robotPosition, robotAngle - 90,
            //                      rightIRDistance);
            // }

            ALSO_RENAME_ME++;
            ALSO_RENAME_ME %= 2;
        }
    }

    if (leftInfrared.brickAppeared(150, 50)) {
        float distanceToWall = leftInfrared.readFromRobotCenter(false);

        leftStatingCorner = {-(distanceToWall), -40};
        leftStatingCorner.transformByPose(robotPose);

        maybePlonkAgainstWall(leftStatingCorner);
    };

    if (leftInfrared.brickDisappeared(150, 50)) {
        float distanceToWall = leftInfrared.readFromRobotCenter(true);

        leftEndingCorner = {-(distanceToWall), 0};
        leftEndingCorner.transformByPose(robotPose);

        twoCornersToBrick(robotPosition, leftStatingCorner, leftEndingCorner);

        if (currentGoal_G == Launch1) {
            currentGoal_G = DriveToEndMaybe;
            UseMazeToGoTo({1300, 1800});
        } else {
            drive.stop();
            sendData();

            navigator.turnLeft(150);
        }
    }

    if (rightInfrared.brickAppeared(150, 50)) {
        float distanceToWall = rightInfrared.readFromRobotCenter(false);
        rightStatingCorner = {(distanceToWall), 0};
        rightStatingCorner.transformByPose(robotPose);
    }

    if (rightInfrared.brickDisappeared(150, 50)) {
        float distanceToWall = rightInfrared.readFromRobotCenter(true);

        rightEndingCorner = {(distanceToWall), 0};
        rightEndingCorner.transformByPose(robotPose);

        twoCornersToBrick(robotPosition, rightStatingCorner, rightEndingCorner);
    }

    // if a wall is there
    if (frontUSDistance < 185 && frontUSDistance != -1) {
        nextState_GP = aligningWithWall_S;

        drive.stop();
        sendData();
    }
}

void aligningWithWall_S() {
    int FLDist = frontLeftInfrared.readSafe();
    int FRDist = frontRightInfrared.readSafe();

    bool cantSeeLeft = (FLDist <= -1 || FLDist > 200);
    bool cantSeeRight = (FRDist <= -1 || FRDist > 200);

    if (cantSeeLeft || cantSeeRight) {
        Serial.println("I cant read this");

        navigator.turnRight();
        nextState_GP = followingLeftWall_S;
        return;
    }

    if (FLDist < FRDist) {
        drive.turnLeft();
    } else if (FLDist > FRDist) {
        drive.turnRight();
    } else {  // is aligned
        drive.stop();

        int frontDistance = ultrasonic.readFromRobotCenter();

        int leftDistance = leftInfrared.readFromRobotCenter();

        // Sets the current angle to the closest 90
        int thingy = motionTracker.umActually(frontDistance, leftDistance);

        if (thingy == 1) {
            pixels.setAll(255, 0, 0, true);
            // TODO remove
            delay(100);
        }
        if (thingy == 2) {
            pixels.setAll(255, 87, 51, true);
            // TODO remove
            delay(100);
        }

        sendData();

        navigator.turnRight();
        nextState_GP = followingLeftWall_S;
    }
}

void stopped_S() {
    drive.stop();

    pixels.setAll(255, 255, 255);
}

void UseMazeToGoTo(Position positionToGoTo) {
    drive.stop();
    pixels.setAll(255, 87, 51, true);
    // TODO add something to show that it is loading
    gridMap.solve(brickList, positionToGoTo);

    nextState_GP = followingMaze_S;
}

void followingMaze_S() {
    static Angle angleToDrive = 90;

    const int range = 100;

    Position robotPosition = motionTracker.getPosition();
    Angle robotAngle = motionTracker.getAngle();

    gridMap.getAngle(robotPosition, &angleToDrive);

    Angle angleToTurn = angleToDrive - robotAngle;

    drive.forwards(angleToTurn);

    // TODO refactor this with Squared Method.
    int distanceToEndMM = gridMap.getEuclideanDistanceToEnd(robotPosition);

    if (distanceToEndMM < range) {
        if (currentGoal_G == DriveToEndMaybe) {
            nextState_GP = celebrating_S;
            currentGoal_G = Won;
        }
        if (currentGoal_G == DriveToStart) {
            UseMazeToGoTo({1300, 1800});
        }
        if (currentGoal_G == DriveToEnd) {
            nextState_GP = celebrating_S;
            currentGoal_G = Won;
        }
    }
}

void celebrating_S() {
    drive.stop();
    pixels.setAll(255, 255, 0);
}

void colourCodeState(voidFuncPtr currentState_P) {
    if (currentState_P == followingLeftWall_S) {
        pixels.setAll(255, 255, 255);
    } else if (currentState_P == aligningWithWall_S) {
        pixels.setAll(0, 0, 255);
    } else if (currentState_P == stopped_S) {
        pixels.setAll(255, 105, 180);
    } else if (currentState_P == followingMaze_S) {
        pixels.setAll(255, 0, 255);
    } else if (currentState_P == celebrating_S) {
        pixels.setAll(255, 255, 0);
    }
}

PassiveSchedule oneSecond(1000);
PassiveSchedule fiveSecond(5000);

void followPathInstead() {}

void loop() {
    polls();

    // if (five.isReadyToRun()) {
    //     // findEdges();
    // }

    byte bumperData = bumper.read();
    if (bumperData) {
        readyToGo = true;
        navigator.hitBumper(bumperData);

        bool isDriveToEndMaybe = (currentGoal_G == DriveToEndMaybe);

        Position robotPosition = motionTracker.getPosition();

        // Get the position of the crash point.

        bool inFromLeft = (robotPosition.x > 300);
        bool inFromRight = (robotPosition.x < 1200);
        bool inFromBottom = (robotPosition.y > 600);
        bool inFromTop = (robotPosition.y < 1400);

        bool inMiddle =
            (inFromLeft && inFromRight && inFromBottom && inFromTop);

        Serial.print(" isDriveToEndMaybe:");
        Serial.print(isDriveToEndMaybe);
        Serial.print(" inFromLeft:");
        Serial.print(inFromLeft);
        Serial.print(" inFromRight:");
        Serial.print(inFromRight);
        Serial.print(" inFromBottom:");
        Serial.print(inFromBottom);
        Serial.print(" inFromTop:");
        Serial.println(inFromTop);

        if ((isDriveToEndMaybe) && (inMiddle)) {
            Serial.println("MApping Inner");
            currentGoal_G = MapInner;
            nextState_GP = followingLeftWall_S;
        }
    }

    if (oneSecond.isReadyToRun()) {
        Position robotPosition = motionTracker.getPosition();

        gridMap.snowPlow(robotPosition);
    }

    colourCodeState(nextState_GP);

    if (readyToGo) {
        if (!navigator.hasNoPath()) {
            // pixels.setAll(0, 255, 0);
            navigator.moveToTarget();
        } else {
            nextState_GP();
        }
    }

    pixels.clear();
    pixels.show();

    if (Serial.available() > 0) {
        String data = Serial.readString();
        const int maxArgs_C = 10;
        String args[maxArgs_C];
        int foundArgs = 0;
        int spaceIndex = 0;

        while ((data.length() > 0) && (foundArgs < maxArgs_C) &&
               (spaceIndex != -1)) {
            spaceIndex = data.indexOf(" ");
            args[foundArgs] = data.substring(0, spaceIndex);
            data = data.substring(spaceIndex + 1);
            foundArgs++;
        }

        if (args[0] == "get-map") {
            gridMap.sendOverSerial();
        }
    }
}

/*

Do 2 laps of the maze, this maps out most of the blocks

Solve the maize to goto the centre of the map, if anything is hit

point towards the centre, then do a left wall follow,

once 2 laps are done,

Solve the maze to goto the Start of the map,

,solve the maze to go to the end,

set the colour to purple and traverse the maze,

//end State.




*/

// TODO list
//  standardize a new naming convention
//   _G = global
//   _L = local version of a global or pointer
//   _C = constant
//   _T = test variable
//   _P = pointer
//   _S = state
//   _I = index

// Tick off all TODOs.

// Give everting a sensible name.

// DOcument all code.
