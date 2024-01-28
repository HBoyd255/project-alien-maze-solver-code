#ifndef MOTION_TRACKER_H
#define MOTION_TRACKER_H

#include <Arduino.h>

#include "angleAndPosition.h"
#include "bluetoothLowEnergy.h"
#include "infrared.h"
#include "motor.h"

// Forward declaration of the Motor class.
class Motor;

class MotionTracker {
   public:
    MotionTracker(Motor* leftMotor_P, Motor* rightMotor_P,
                  Angle statingAngle);

    Angle angleFromOdometry();

    bool updateAngle();
    bool updatePosition();
    bool poll();

    int recalibratePosition(int frontDistance, int leftDistance);

    Angle getAngle();
    Position getPosition();
    Pose getPose();

   private:
    Motor* _leftMotor_P;
    Motor* _rightMotor_P;

    Position _currentPosition;
    const Angle _statingAngle;
    Angle _currentAngle = 90;
    Angle _angleCalibration = 0;

    int32_t _averageTravelDistance();
};

#endif  // MOTION_TRACKER_H