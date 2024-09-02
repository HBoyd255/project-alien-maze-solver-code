/**
 * @file main.cpp
 * @brief The main file and entry point of the project.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-02-01
 * @copyright Copyright (c) 2024
 */

//  ██████╗ ██████╗  ██████╗      ██╗███████╗ ██████╗████████╗
//  ██╔══██╗██╔══██╗██╔═══██╗     ██║██╔════╝██╔════╝╚══██╔══╝
//  ██████╔╝██████╔╝██║   ██║     ██║█████╗  ██║        ██║
//  ██╔═══╝ ██╔══██╗██║   ██║██   ██║██╔══╝  ██║        ██║
//  ██║     ██║  ██║╚██████╔╝╚█████╔╝███████╗╚██████╗   ██║
//  ╚═╝     ╚═╝  ╚═╝ ╚═════╝  ╚════╝ ╚══════╝ ╚═════╝   ╚═╝
//
//   █████╗    ██╗        ██╗   ███████╗   ███╗   ██╗
//  ██╔══██╗   ██║        ██║   ██╔════╝   ████╗  ██║
//  ███████║   ██║        ██║   █████╗     ██╔██╗ ██║
//  ██╔══██║   ██║        ██║   ██╔══╝     ██║╚██╗██║
//  ██║  ██║██╗███████╗██╗██║██╗███████╗██╗██║ ╚████║██╗
//  ╚═╝  ╚═╝╚═╝╚══════╝╚═╝╚═╝╚═╝╚══════╝╚═╝╚═╝  ╚═══╝╚═╝

// Ascii text generated at https://patorjk.com/software/taag/
// Font used - ANSI Shadow

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

#include "angleAndPosition.h"
#include "binary.h"
#include "bluetoothLowEnergy.h"
#include "brick.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "history.h"
#include "infrared.h"
#include "infrared_Test.h"
#include "map.h"
#include "mazeConstants.h"
#include "motionTracker.h"
#include "motor.h"
#include "navigator.h"
#include "pixels.h"
#include "schedule.h"
#include "systemInfo.h"
#include "ultrasonic.h"

// ███████╗██╗      █████╗  ██████╗ ███████╗
// ██╔════╝██║     ██╔══██╗██╔════╝ ██╔════╝
// █████╗  ██║     ███████║██║  ███╗███████╗
// ██╔══╝  ██║     ██╔══██║██║   ██║╚════██║
// ██║     ███████╗██║  ██║╚██████╔╝███████║
// ╚═╝     ╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝
//

// If true, the robot will run a test loop instead of the main code.
#define RUN_TEST_LOOP false

// If true, the robot will prefill the brick list with the maze data.
#define PREFILL_BRICK_LIST false

// If true, after setup, the robot with count down for 2 seconds before
// starting to give the user time to open the serial monitor.
#define SERIAL_DELAY false

// If true, the robot will wait for the user to press the bumper or for a BLE
// connection before starting.
#define WAIT_UPON_START false

//   ██████╗ ██████╗      ██╗███████╗ ██████╗████████╗███████╗
//  ██╔═══██╗██╔══██╗     ██║██╔════╝██╔════╝╚══██╔══╝██╔════╝
//  ██║   ██║██████╔╝     ██║█████╗  ██║        ██║   ███████╗
//  ██║   ██║██╔══██╗██   ██║██╔══╝  ██║        ██║   ╚════██║
//  ╚██████╔╝██████╔╝╚█████╔╝███████╗╚██████╗   ██║   ███████║
//   ╚═════╝ ╚═════╝  ╚════╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝

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
                      ULTRASONIC_DATA_SHELF_LIFE,
                      FRONT_ULTRASONIC_FORWARD_DISTANCE);

Infrared leftInfrared(LEFT_INFRARED_INDEX, LEFT_INFRARED_FORWARD_DISTANCE);
Infrared frontLeftInfrared(FRONT_LEFT_INFRARED_INDEX,
                           FRONT_LEFT_INFRARED_FORWARD_DISTANCE);
Infrared frontRightInfrared(FRONT_RIGHT_INFRARED_INDEX,
                            FRONT_RIGHT_INFRARED_FORWARD_DISTANCE);
Infrared rightInfrared(RIGHT_INFRARED_INDEX, RIGHT_INFRARED_FORWARD_DISTANCE);

Bumper bumper(BUMPER_SHIFT_REG_DATA, COMMON_SHIFT_REG_LOAD,
              COMMON_SHIFT_REG_CLOCK, BUMPER_BIT_OFFSET);

BluetoothLowEnergy bluetoothLowEnergy(MAIN_SERVICE_UUID, ROBOT_POSE_UUID,
                                      BRICK_UUID);

MotionTracker motionTracker(&leftMotor, &rightMotor, INITIAL_ANGLE);

Navigator navigator(&motionTracker, &drive);

BrickList brickList;

Map gridMap;

/**
 * @brief A test loop to run instead of loop() if the RUN_TEST_LOOP define is
 * set to true.
 */
void testLoop();

/**
 * @brief Am instance of the PassiveSchedule class, that can trigger a function
 * once per second.
 *
 */
PassiveSchedule oneSecondSchedule(1000);

//  ██████╗ ███████╗ █████╗ ██████╗     ███╗   ███╗███████╗
//  ██╔══██╗██╔════╝██╔══██╗██╔══██╗    ████╗ ████║██╔════╝
//  ██████╔╝█████╗  ███████║██║  ██║    ██╔████╔██║█████╗
//  ██╔══██╗██╔══╝  ██╔══██║██║  ██║    ██║╚██╔╝██║██╔══╝
//  ██║  ██║███████╗██║  ██║██████╔╝    ██║ ╚═╝ ██║███████╗
//  ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝     ╚═╝     ╚═╝╚══════╝

/*
Im going to preface all of this by saying that I ran out of time,
There is still a lot of documenting and refactoring that I would like to do, but
in its current state the logic is sound and everything works.

Logic Flow

  Stage 1 - Follow the left wall, plot all the bricks encountered on this lap.
  Stage 2 - Use the flood fill to make a first attempt at plotting the maze.
  Stage 3 - Use this map to drive through the centre of the maze.
  Stage 4 - Hit an island in the middle of the maze, then left wall follow all
            the way around this island.
  Stage 5 - Drive back to the start of the maze.
  Stage 6 - Use the flood fill Solve and drive the maze.
  Stage 7 - Celebrate.


The robot starts by facing away from the maze, with its coordinates off the
maze, It then drives until it finds a wall It uses that wall to calibrate its
 y position to place itself on the map. It then turns right and does the same
again to calibrate its x Position.

The way that the robot preforms left wall following is by driving until it
measures a drop in distance from the infrared sensor on the left.
It then drives a further 15cm and preforms a 90° left turn, and keeps driving.
If while driving forwards it detects a obstacle less that



Ive implemented a naming convention for tagging variables
//   _G = global
//   _L = local version of a global or pointer
//   _C = constant
//   _P = pointer
//   _S = state
//   _I = index



There are two important structs that are similar in this code,
Position and MapPoint, both store x and y, but Position is measured in
millimeters and MapPoint is in centimeter, MapPoint is also stores its values as
ints, because its primary use is referencing items in the map via its index.

Coordinate System

The robot uses a coordinate system where the start is in the bottom left corner
of the maze. The x axis is the width of the maze, and the y axis is the length
of the maze. The angles are measured in degrees, with 0° being the positive x
axis. The angle increases in the counter-clockwise direction. This means that
while the robot is going "forwards" it is driving in the 90° direction, which is
a little unintuitive, but makes the calculations easier.




Default units;

time - miliseconds
distance - millimeter
angle - degrees


It is true that not everything is fully documented, but everything is
modularized and made robust.

Thought my robustness I've been able to;
. Pretty much remove all false positives from the IR reading.
. Detaching the ultrasonic from the timer and get US reading and BLE working
  simultaneously.
. Put everything on a single thread.
. Achieve mm precise Odometry, this robot could lap the maze 10 times and still
  know exactly where it is.

None of the code submitted was generated by AI.
*/

//  ███████╗███████╗████████╗██╗   ██╗██████╗
//  ██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
//  ███████╗█████╗     ██║   ██║   ██║██████╔╝
//  ╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝
//  ███████║███████╗   ██║   ╚██████╔╝██║
//  ╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝

/**
 * @brief The setup function, responsible for initialising the objects that
 * require initialization.
 */
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    // ErrorIndicator should be initialised first
    ErrorIndicator_G.begin(SERIAL_BAUD_RATE, LED_BUILTIN);

    // Add a callback to the ErrorIndicator to stop the motors if an error
    // occurs.
    ErrorIndicator_G.addHaltCallback([]() { drive.stop(); });

    // Add a callback to the ErrorIndicator to flash the pixels to draw the
    // users attention to the serial monitor.
    ErrorIndicator_G.addDrawAttentionCallback([]() { pixels.flash(); });

    // Add a callback to the ErrorIndicator to clear the pixels once the user
    // has opened the serial monitor.
    ErrorIndicator_G.addAttentionDrawnCallback_P([]() { pixels.clear(true); });

    // Interrupts must be connected to a void function pointer, not class
    // methods, so to attach a class method to an interrupt pin it must be
    // converted to a void function pointer using a global instance and a
    // lamda function.
    leftMotor.setup([]() { leftMotor.isr(); });
    rightMotor.setup([]() { rightMotor.isr(); });

    pixels.setup();

    leftInfrared.setup();
    rightInfrared.setup();
    frontLeftInfrared.setup();
    frontRightInfrared.setup();

    ultrasonic.setup([]() { ultrasonic.isr(); });

    // Initialise the bluetooth connection.
    bluetoothLowEnergy.setup(BLE_DEVICE_NAME, BLE_MAC_ADDRESS);

#if RUN_TEST_LOOP
    while (true) {
        testLoop();
    }
#endif  // RUN_TEST_LOOP

#if PREFILL_BRICK_LIST
    brickList.setPreprogrammedMazeData();
#endif  // PREFILL_BRICK_LIST

#if SERIAL_DELAY
    // Count down for 2 seconds.
    for (int i = 2000; i > 0; i--) {
        Serial.println(i);
        delay(1);
    }
#endif  // SERIAL_DELAY

#if WAIT_UPON_START
    while (!(bluetoothLowEnergy.isConnected() || (bool)bumper.read())) {
        bluetoothLowEnergy.poll();
    }
#endif  // WAIT_UPON_START
}

/**
 * @brief Polls the various classes that need to be polled.
 */
void polls() {
    frontLeftInfrared.poll();
    frontRightInfrared.poll();
    leftInfrared.poll();
    rightInfrared.poll();

    ultrasonic.poll();

    motionTracker.poll();

    bluetoothLowEnergy.poll();
}

/**
 * @brief Check if there is an incoming serial message, and if so break it down
 * into its arguments by splitting the sting up at the spaces
 *
 * This function it designed to be expandable, currently is just has one
 * function.
 *
 * Available commands:
 *  get-map - Sends the map over serial.
 *
 */
void checkIncomingSerialCommands() {
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
            drive.stop();

            gridMap.sendOverSerial();
        }
    }
}

/**
 * @brief Send the BrickList and the current pose of the robot over bluetooth.
 */
void sendDataOverBLE() {
    bluetoothLowEnergy.sendRobotPose(motionTracker.getPose());
    bluetoothLowEnergy.sendBrickList(brickList);
}

// Forward declaration of states.
void followingLeftWall_S();
void aligningWithWall_S();
void UseMazeToGoTo(Position);
void followingMaze_S();
void celebrating_S();

voidFuncPtr nextState_GP = followingLeftWall_S;

enum Objective {
    NoObjective,
    MapOuterWall,
    DriveToEndViaCentre,
    MapIsland,
    DriveToStart,
    DriveToEnd,
    CompletedMaze,

};

Objective currentObjective_G = MapOuterWall;

void colourCodeState(voidFuncPtr currentState_P) {
    if (currentState_P == followingLeftWall_S) {
        pixels.setAll(Colour("White"));
    } else if (currentState_P == aligningWithWall_S) {
        pixels.setAll(Colour("Blue"));
    } else if (currentState_P == followingMaze_S) {
        pixels.setAll(Colour("Magenta"));
    }
}

void followingLeftWall_S() {
    Pose robotPose = motionTracker.getPose();
    Position robotPosition = motionTracker.getPosition();
    Angle robotAngle = motionTracker.getAngle();

    static Objective lastObjective = NoObjective;

    // Boolean listing whether the objective has changed since the last call to
    // this function.
    bool objectiveIsNew = lastObjective != currentObjective_G;
    lastObjective = currentObjective_G;

    // static position for the stats and end position of lines seen by the left
    // and right sensors.
    static Position leftStatingCorner;
    static Position leftEndingCorner;
    static Position rightStatingCorner;
    static Position rightEndingCorner;

    // The typical distance the robot has moved before it noticed a corner has
    // appeared.
    const int startingCornerDropOff = 40;

    // If the current objective it to map the outer wall, check if enough laps
    // of the map have been done, and if so move onto the next objective
    if (currentObjective_G == MapOuterWall) {
        static Position otherSideOfMaze;
        static int halfLapCounter;

        const int otherSideTolerance = 200;

        // If this is the first call to this function in which the goal is to
        // map the outer wall.
        if (objectiveIsNew) {
            // the set the coordinate of the other side of the maze,
            otherSideOfMaze = {MAZE_WIDTH - otherSideTolerance,
                               MAZE_LENGTH - otherSideTolerance};
            // and sent the half lap counter to zero.
            halfLapCounter = 0;
        }

        // Measure the distance to the furthest corner of the maze.
        int distanceToOtherSide = robotPosition.distanceTo(otherSideOfMaze);

        // If the robot has reached the other side of the maze, then increment
        // the half lap counter.
        if (distanceToOtherSide < otherSideTolerance) {
            halfLapCounter += 1;

            // If the robot has completed an o
            bool halfLapsIsOdd = halfLapCounter & 1;

            if (halfLapsIsOdd) {
                otherSideOfMaze = {otherSideTolerance, otherSideTolerance};
            } else {
                otherSideOfMaze = {MAZE_WIDTH - otherSideTolerance,
                                   MAZE_LENGTH - otherSideTolerance};
            }

            // The number of laps that must be ran before the robot moves onto
            // the next stage, increasing the laps can lead to a more accurately
            // plotted brickiest, but ideally only one lap should be necessary.
            const int requiredLaps = 1;

            // If the robot has completed enough laps,
            if ((halfLapCounter >> 1) == requiredLaps) {
                // update the current objective,
                currentObjective_G = DriveToEndViaCentre;
                // and take a first attempt at solving the maze.
                UseMazeToGoTo(Position(MAZE_WIDTH - 200, MAZE_LENGTH - 200));
            }
        }
    }

    // If the current objective it to map the inner island, check if the robot
    // have reached its starting point, and if so move onto the next objective.
    if (currentObjective_G == MapIsland) {
        // A flag that for checking if the robot has driven far enough from its
        // starting position.
        static bool innerTraveledFarEnough;
        static Position islandStartPosition;

        // If this is the first call to this function in which the goal is to
        // map the inner island,
        if (objectiveIsNew) {
            // set the start position,
            islandStartPosition = robotPosition;
            // and lower the TraveledFarEnough flag.
            innerTraveledFarEnough = false;
        }

        const int islandOuterTolerance = 150;
        const int islandInnerTolerance = 75;

        int distanceFromStart = robotPosition.distanceTo(islandStartPosition);

        // If driven far enough from the start position, raise a flag.
        if (distanceFromStart > islandOuterTolerance) {
            innerTraveledFarEnough = true;
        }

        // If close enough to the start, and the outer flag has been raised,

        if ((distanceFromStart < islandInnerTolerance) &&
            innerTraveledFarEnough) {
            // move onto the next state,
            currentObjective_G = DriveToStart;
            // and drive to the start.
            UseMazeToGoTo({200, 200});
            return;
        }
    }

    // Get the difference between the angle the robot is driving, and the
    // closest right angle.
    int orthogonalOffset = robotAngle.OrthogonalOffset();
    // Use this angle difference to calibrate the forwards function.
    drive.forwards(orthogonalOffset);

    // Read in the front distance from the Ultrasonic sensor.
    int frontUSDistance = ultrasonic.readFromRobotCenter();

    const int orthogonalTolerance = 5;

    if (robotAngle.isOrthogonal(orthogonalTolerance)) {
        Angle roundedRobotAngle = robotAngle.closestRightAngle();

        static PassiveSchedule compareBrickScheduler(10);

        if (compareBrickScheduler.isReadyToRun()) {
            int leftIRDistance = leftInfrared.readFromRobotCenter();

            // If not using front sensor, it will be using the left sensor.
            static bool usingFrontSensor = false;

            // Alternate between the front and the left sensor.
            usingFrontSensor = !usingFrontSensor;

            if ((usingFrontSensor) && (frontUSDistance > 120)) {
                Angle frontSensorAngle = roundedRobotAngle;
                brickList.handleBrickFromSensorAndMap(
                    robotPosition, frontSensorAngle, frontUSDistance,
                    orthogonalTolerance, &gridMap);
            }

            else if ((!usingFrontSensor) && (leftIRDistance > 120)) {
                Angle leftSensorAngle = roundedRobotAngle + 90;
                brickList.handleBrickFromSensorAndMap(
                    robotPosition, leftSensorAngle, leftIRDistance,
                    orthogonalTolerance, &gridMap);
            }
        }
    }

    if (leftInfrared.seenStartingCorner(150, 50)) {
        float distanceToWall = leftInfrared.readFromRobotCenter(false);

        leftStatingCorner = Position(-distanceToWall, -startingCornerDropOff);
        leftStatingCorner.transformByPose(robotPose);

        brickList.handleBrickFromWallPosition(leftStatingCorner);
    };

    if (leftInfrared.seenEndingCorner(150, 50)) {
        float distanceToWall = leftInfrared.readFromRobotCenter(true);

        leftEndingCorner = Position(-distanceToWall, 0);
        leftEndingCorner.transformByPose(robotPose);

        brickList.handleBrickFromLine(robotPosition, leftStatingCorner,
                                      leftEndingCorner);

        drive.stop();
        sendDataOverBLE();

        // Drive 150 mm then turn left
        navigator.turnLeft(150);
    }

    if (rightInfrared.seenStartingCorner(150, 50)) {
        float distanceToWall = rightInfrared.readFromRobotCenter(false);
        rightStatingCorner = Position(distanceToWall, 0);
        rightStatingCorner.transformByPose(robotPose);
    }

    if (rightInfrared.seenEndingCorner(150, 50)) {
        float distanceToWall = rightInfrared.readFromRobotCenter(true);

        rightEndingCorner = Position(distanceToWall, -startingCornerDropOff);
        rightEndingCorner.transformByPose(robotPose);

        brickList.handleBrickFromLine(robotPosition, rightStatingCorner,
                                      rightEndingCorner);
    }

    // If a wall is there.
    if (frontUSDistance < 185 && frontUSDistance != -1) {
        nextState_GP = aligningWithWall_S;

        drive.stop();
        sendDataOverBLE();
    }
}

void aligningWithWall_S() {
    int FLDist = frontLeftInfrared.readSafe();
    int FRDist = frontRightInfrared.readSafe();

    bool cantSeeLeft = (FLDist <= -1 || FLDist > 200);
    bool cantSeeRight = (FRDist <= -1 || FRDist > 200);

    if (cantSeeLeft || cantSeeRight) {
        Serial.println("Cant read front sensor while aligning");

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
        int wallsRecelebratedAgainst =
            motionTracker.recalibratePosition(frontDistance, leftDistance);

        if (wallsRecelebratedAgainst == 1) {
            // If against a wall, flash the Pixels red to indicate.
            pixels.setAll(Colour("Red"), true);
            delay(100);
        }
        if (wallsRecelebratedAgainst == 2) {
            pixels.setAll(Colour("Pink"), true);
            // If in a corner, flash the Pixels pink to indicate.
            delay(100);
        }

        sendDataOverBLE();

        navigator.turnRight();
        nextState_GP = followingLeftWall_S;
        return;
    }
}

void UseMazeToGoTo(Position positionToGoTo) {
    drive.stop();
    pixels.setAll(Colour("Pink"), true);

    gridMap.solve(brickList, positionToGoTo);

    nextState_GP = followingMaze_S;
}

void followingMaze_S() {
    static Angle angleToDrive = 90;

    Position robotPosition = motionTracker.getPosition();
    Angle robotAngle = motionTracker.getAngle();

    gridMap.updateAngleToDrive(robotPosition, &angleToDrive);

    Angle angleToTurn = angleToDrive - robotAngle;

    drive.forwards(angleToTurn);

    int distanceToEndMM = gridMap.getCrowDistanceToEnd(robotPosition);

    const int range = 100;
    // if the robot is less than 100 mm from the goal, move onto the next
    // state.
    if (distanceToEndMM < range) {
        if (currentObjective_G == DriveToEndViaCentre) {
            nextState_GP = celebrating_S;
            currentObjective_G = CompletedMaze;
        } else if (currentObjective_G == DriveToStart) {
            currentObjective_G = DriveToEnd;
            UseMazeToGoTo({1300, 1800});
        } else if (currentObjective_G == DriveToEnd) {
            nextState_GP = celebrating_S;
            currentObjective_G = CompletedMaze;
        }
    }
}

void celebrating_S() {
    static int startedCelTime = millis();

    int celebratingDuration = millis() - startedCelTime;

    // First stop,
    if (celebratingDuration < 50) {
        drive.stop();

    }
    // Then rapidly spin for 2 seconds,
    else if (celebratingDuration < 2000) {
        drive.fullSpeedSpinLeft();
    }
    // Then stop again.
    else {
        drive.stop();
    }

    // Flash the LEDS
    int ledCount = pixels.getLedCount();
    pixels.clear();

    static bool ledToggle;
    PassiveSchedule ledToggleToggler(1000);

    // each second toggle which leds are on.
    if (ledToggleToggler.isReadyToRun()) {
        ledToggle = !ledToggle;
    }

    // Each second toggle alternating leds yellow
    for (int i = 0; i < (ledCount / 2); i++) {
        pixels.setPixel((i * 2) + ledToggle, Colour("Yellow"));
    }
    pixels.show();
}

/**
 * @brief Update the objective of the robot, depending on a combination of
 * bumper data, the current objective and the current position.
 *
 * @param bumperData The byte read in from the bumper values.
 * @param currentObjective The current objecting of the robot.
 * @param robotPosition The current position of the robot.
 */
void updateObjective(byte bumperData, Objective currentObjective,
                     Position robotPosition) {
    bool bumperIsPressed = (bool)bumperData;

    bool inFromLeft = (robotPosition.x > 300);
    bool inFromRight = (robotPosition.x < 1200);
    bool inFromBottom = (robotPosition.y > 600);
    bool inFromTop = (robotPosition.y < 1400);

    bool inMiddle = (inFromLeft && inFromRight && inFromBottom && inFromTop);

    // If one of the bumpers is pressed, the robot is in the middle of the maze
    // and the current objective is to drive to the end through the centre,
    if (bumperIsPressed && inMiddle &&
        (currentObjective == DriveToEndViaCentre)) {
        // then update the current objective to  drive to the centre, and
        currentObjective_G = MapIsland;
        nextState_GP = followingLeftWall_S;
    }
}

void loop() {
    polls();
    checkIncomingSerialCommands();

    byte bumperData = bumper.read();
    if (bumperData) {
        navigator.hitBumper(bumperData);
    }

    updateObjective(bumperData, currentObjective_G,
                    motionTracker.getPosition());

    if (oneSecondSchedule.isReadyToRun()) {
        Position robotPosition = motionTracker.getPosition();
        gridMap.plotVisitedPointsOnMap(robotPosition);
    }

    colourCodeState(nextState_GP);

    if (!navigator.hasNoPath()) {
        pixels.setAll(Colour("Green"));

        navigator.moveToTarget();

    } else {
        nextState_GP();
    }

    pixels.show();
}

/**
 * @brief A test loop to run instead of loop() if the RUN_TEST_LOOP define is
 * set to true.
 */
void testLoop() {
    // Just stay stationary until a bumper is pressed, then drive away from it.
    polls();

    byte bumperData = bumper.read();
    if (bumperData) {
        navigator.hitBumper(bumperData);
    }

    if (!navigator.hasNoPath()) {
        pixels.setAll(Colour("Green"));

        Serial.print(" Robot pose:");
        Serial.print(motionTracker.getPose());

        Serial.print(" Path: ");
        Serial.println(navigator.getPathAsString());

        navigator.moveToTarget();

    } else {
        pixels.setAll(Colour("Black"));

        Serial.print(" Robot pose:");
        Serial.print(motionTracker.getPose());

        Serial.print(" Path: ");
        Serial.println("Stopped");

        drive.stop();
    }

    pixels.show();
}
