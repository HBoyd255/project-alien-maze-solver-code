

#include "obstacles.h"

#include "binary.h"
#include "bumper.h"
#include "grid.h"
#include "infrared.h"
#include "motionTracker.h"
#include "ultrasonic.h"

const String SensorNames[4] = {"Unknown", "Infrared", "Ultrasonic", "Bumper"};

Obstacle::operator String() const {
    String stringToReturn = "[";
    stringToReturn += (String)this->position;
    stringToReturn += ",";
    stringToReturn += SensorNames[this->sensorType];
    stringToReturn += "]";

    return stringToReturn;
}

void printObstacleVector(ObstacleVector& vectorToPrint) {
    uint16_t itemCount = vectorToPrint.size();

    Serial.print("Obstacle Vector Containing ");
    Serial.print(itemCount);
    Serial.print(" Items - ");

    for (const auto& item : vectorToPrint) {
        Serial.print(item);
        Serial.print(",");
    }
    Serial.println();
}

ObstacleDetector::ObstacleDetector(MotionTracker* motionTrackerPtr,
                                   Infrared* infraredSensorPtr, Pose sensorPose)
    : _motionTrackerPtr(motionTrackerPtr),
      _infraredSensorPtr(infraredSensorPtr),
      _sensorUsed(InfraredSensor),
      _sensorPose(sensorPose)

{}

ObstacleDetector::ObstacleDetector(MotionTracker* motionTrackerPtr,
                                   Ultrasonic* ultrasonicSensorPtr,
                                   Pose sensorPose)
    : _motionTrackerPtr(motionTrackerPtr),
      _ultrasonicSensorPtr(ultrasonicSensorPtr),
      _sensorUsed(UltrasonicSensor),
      _sensorPose(sensorPose)

{}
ObstacleDetector::ObstacleDetector(MotionTracker* motionTrackerPtr,
                                   Bumper* bumperSensorPtr, Pose sensorPose)
    : _motionTrackerPtr(motionTrackerPtr),
      _bumperSensorPtr(bumperSensorPtr),
      _sensorUsed(BumperSensor),
      _sensorPose(sensorPose)

{}

void ObstacleDetector::addObstaclesToVector(ObstacleVector* vectorPtr) {
    switch (this->_sensorUsed) {
        case InfraredSensor:

            this->_addObstaclesFromRangeSensor(vectorPtr);

            break;
        case UltrasonicSensor:

            this->_addObstaclesFromRangeSensor(vectorPtr);

            break;
        case BumperSensor:

            this->_addObstaclesFromBumper(vectorPtr);

            break;

        default:

            Serial.println("Unknown Sensor");
            Serial.println(this->_sensorUsed);
            Serial.println("Please Fix");

            break;
    }
}

void ObstacleDetector::updateGrid(Grid* gridToUpdatePtr) {
    // get the global pose of the sensor

    Pose robotPose = this->_motionTrackerPtr->getPose();

    int16_t distance = this->_readRange();

    if (distance > 0) {
        Position positionFromObstacle;
        positionFromObstacle.y = distance;
        positionFromObstacle.transformByPose(this->_sensorPose);
        positionFromObstacle.transformByPose(robotPose);

        gridToUpdatePtr->setFromPosition(positionFromObstacle, 100);

        distance -= 10;
        while (distance > 0) {
            Position positionGap;
            positionGap.y = distance;
            positionGap.transformByPose(this->_sensorPose);
            positionGap.transformByPose(robotPose);

            gridToUpdatePtr->setFromPosition(positionGap, 0);
            distance -= 10;
        }
    }

    // get the distance of the sensor

    // set everyrhing from the sensor to the obstacle to 0.

    // gridToUpdatePtr->_setAndSend();
}

int16_t ObstacleDetector::_readRange() {
    switch (this->_sensorUsed) {
        case InfraredSensor:

            return this->_infraredSensorPtr->readSafe();

            break;
        case UltrasonicSensor:

            return this->_ultrasonicSensorPtr->read();

            break;

        default:

            Serial.println("Unknown Sensor");
            Serial.println(this->_sensorUsed);
            Serial.println("Please Fix");

            return -1;

            break;
    }
}

void ObstacleDetector::_addObstaclesFromRangeSensor(ObstacleVector* vectorPtr) {
    int16_t distance = this->_readRange();

    if (distance > 1) {
        Position positionToReturn;

        // Set the position of the detected obstacle relative to the
        // sensor.
        positionToReturn.y = distance;

        // Transform the position from being relative to the sensor, to
        // being relative to the robot.
        positionToReturn.transformByPose(this->_sensorPose);

        Pose robotPose = this->_motionTrackerPtr->getPose();

        // Transform the position from being relative to the robot, to
        // being relative to the global coordinate system.
        positionToReturn.transformByPose(robotPose);

        Obstacle obstacleToAdd = {positionToReturn, this->_sensorUsed};

        vectorPtr->push_back(obstacleToAdd);
    }
}

void ObstacleDetector::_addObstaclesFromBumper(ObstacleVector* vectorPtr) {
    // TODO make this more robust.

    uint8_t bumperData = this->_bumperSensorPtr->read();

    const uint8_t buttonCount = 8;
    const uint8_t degreesPerButton = 45;

    for (uint8_t buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++) {
        bool buttonValue = (bumperData >> buttonIndex) & 1;

        if (buttonValue) {
            Angle iterativeAngle =
                this->_sensorPose.angle - (buttonIndex * degreesPerButton);

            // The position of the pressed button, before being rotated.
            Position positionToReturn = this->_sensorPose.position;

            Pose poseToRotateAround;
            poseToRotateAround.position.x = 0;
            poseToRotateAround.position.y = 0;
            poseToRotateAround.angle = iterativeAngle;

            positionToReturn.transformByPose(poseToRotateAround);

            Pose robotPose = this->_motionTrackerPtr->getPose();

            // Transform the position from being relative to the robot, to
            // being relative to the global coordinate system.
            positionToReturn.transformByPose(robotPose);

            Obstacle obstacleToAdd = {positionToReturn, this->_sensorUsed};

            vectorPtr->push_back(obstacleToAdd);
        }
    }
}