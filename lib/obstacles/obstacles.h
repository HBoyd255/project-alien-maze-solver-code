#ifndef OBSTACLES_H
#define OBSTACLES_H

// TODO fix this mess

#include <Arduino.h>

#include "angleAndPosition.h"
#include "vectorUpgrades.h"

// Forward declaration
class MotionTracker;
class Infrared;
class Ultrasonic;
class Bumper;
class BluetoothLowEnergy;

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

class ObstacleDetector {
   public:
    ObstacleDetector(MotionTracker* motionTrackerPtr,
                     Infrared* infraredSensorPtr, Pose sensorPose);
    ObstacleDetector(MotionTracker* motionTrackerPtr,
                     Ultrasonic* ultrasonicSensorPtr, Pose sensorPose);
    ObstacleDetector(MotionTracker* motionTrackerPtr, Bumper* bumperSensorPtr,
                     Pose sensorPose);

    void addObstaclesToVector(ObstacleVector* vectorPtr);

    void sendOverBLE(BluetoothLowEnergy* blePtr);

   private:
    MotionTracker* _motionTrackerPtr;

    Infrared* _infraredSensorPtr = nullptr;
    Ultrasonic* _ultrasonicSensorPtr = nullptr;
    Bumper* _bumperSensorPtr = nullptr;

    SensorType _sensorType;
    Pose _sensorPose;

    int16_t _readRange();
    void _addObstaclesFromRangeSensor(ObstacleVector* vectorPtr);
    void _addObstaclesFromBumper(ObstacleVector* vectorPtr);

    Pose _getGlobalPoseOfSensor();

    };

#endif  // OBSTACLES_H