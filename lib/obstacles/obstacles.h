#ifndef OBSTACLES_H
#define OBSTACLES_H

#include <Arduino.h>

#include "angleAndPosition.h"
#include "bumper.h"
#include "infrared.h"
#include "motionTracker.h"
#include "ultrasonic.h"
#include "vectorUpgrades.h"

enum SensorType {
    UnknownSensor,
    InfraredSensor,
    UltrasonicSensor,
    BumperSensor
};

struct Obstacle {
    Position position;
    uint8_t sensorType = UnknownSensor;

    operator String() const;
};

using ObstacleVector = std::vector<Obstacle>;

void printObstacleVector(ObstacleVector& vectorToPrint);

// Forward declaration
class MotionTracker;
class Infrared;
class Ultrasonic;
class Bumper;

class ObstacleDetector {
   public:
    ObstacleDetector(MotionTracker* motionTrackerPtr,
                     Infrared* infraredSensorPtr, Pose sensorPose);
    ObstacleDetector(MotionTracker* motionTrackerPtr,
                     Ultrasonic* ultrasonicSensorPtr, Pose sensorPose);
    ObstacleDetector(MotionTracker* motionTrackerPtr, Bumper* bumperSensorPtr,
                     Pose sensorPose);

    void addObstaclesToVector(ObstacleVector* vectorPtr);

    int16_t _readRange();

   private:
    MotionTracker* _motionTrackerPtr;

    Infrared* _infraredSensorPtr = nullptr;
    Ultrasonic* _ultrasonicSensorPtr = nullptr;
    Bumper* _bumperSensorPtr = nullptr;

    SensorType _sensorUsed;
    Pose _sensorPose;

    void _addObstaclesFromRangeSensor(ObstacleVector* vectorPtr);
    void _addObstaclesFromBumper(ObstacleVector* vectorPtr);
};

#endif  // OBSTACLES_H