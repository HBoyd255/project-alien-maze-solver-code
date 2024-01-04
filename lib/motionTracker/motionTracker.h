#ifndef MOTION_TRACKER_H
#define MOTION_TRACKER_H

#include <Arduino.h>

#include "angleAndPosition.h"
#include "bluetoothLowEnergy.h"
#include "infrared.h"
#include "motor.h"

// Forward declaration
class BluetoothLowEnergy;
class Motor;
class Infrared;

class MotionTracker {
   public:
    MotionTracker(BluetoothLowEnergy* blePtr, Motor* leftMotorPtr,
                  Motor* rightMotorPtr, Infrared* frontLeftInfraredPtr,
                  Infrared* frontRightInfraredPtr);

    Angle angleFromOdometry();
    Angle angleFromFrontIR();

    bool updateAngle();
    bool updatePosition();

    void poll(bool sendOverBLE);

    Pose getPose();

    void setTargetPosition(int16_t xValue, int16_t yValue);

    void moveToTarget();

    Angle getGlobalAngleToPoint();
    Angle getLocalAngleToTurn();
    int16_t getDistanceFromTarget();

    Position targetPosition;

   private:
    BluetoothLowEnergy* _blePtr;
    Motor* _leftMotorPtr;
    Motor* _rightMotorPtr;
    Infrared* _frontLeftInfraredPtr;
    Infrared* _frontRightInfraredPtr;

    Position _currentPosition;
    Angle _currentAngle = 0;

    int32_t _averageTravelDistance();
};

#endif  // MOTION_TRACKER_H