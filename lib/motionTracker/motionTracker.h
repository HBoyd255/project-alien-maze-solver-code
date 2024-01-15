#ifndef MOTION_TRACKER_H
#define MOTION_TRACKER_H

#include <Arduino.h>

#include "angleAndPosition.h"
#include "bluetoothLowEnergy.h"
#include "infrared.h"
#include "motor.h"

// Forward declaration
class Motor;
class Infrared;

class MotionTracker {
   public:
    MotionTracker(Motor* leftMotorPtr, Motor* rightMotorPtr,
                  Infrared* frontLeftInfraredPtr,
                  Infrared* frontRightInfraredPtr);

    Angle angleFromOdometry();

    bool updateAngle();
    bool updatePosition();
    bool poll();

    void umActually();

    void setInitialX(int initialX);

    void setInitialY(int initialY);

    Angle getAngle();
    Position getPosition();
    Pose getPose();

   private:
    Motor* _leftMotorPtr;
    Motor* _rightMotorPtr;
    Infrared* _frontLeftInfraredPtr;
    Infrared* _frontRightInfraredPtr;

    Position _currentPosition;
    Angle _currentAngle = 90;
    Angle _angleCalibration = 0;

    int32_t _averageTravelDistance();
};

#endif  // MOTION_TRACKER_H