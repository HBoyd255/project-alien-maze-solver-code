#ifndef MOTION_TRACKER_H
#define MOTION_TRACKER_H

#include <Arduino.h>

#include "angleAndPosition.h"
#include "bluetoothLowEnergy.h"
#include "motor.h"
#include "schedule.h"

// Forward declaration of the Motor class.
class Motor;

class MotionTracker {
   public:
    MotionTracker(Motor* leftMotor_P, Motor* rightMotor_P, Angle statingAngle);

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
    Angle _currentAngle;
    Angle _angleCalibration;

    int _currentAverageDistance;
    int _lastAverageDistance;

    PassiveSchedule _pollSchedule;

    int _getAverageDistance();
    int _getChangeInDistance();
};

#endif  // MOTION_TRACKER_H