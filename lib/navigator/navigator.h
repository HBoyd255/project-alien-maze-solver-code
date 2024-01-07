#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "angleAndPosition.h"

// Forward declaration of the MotionTracker and Drive class.
class MotionTracker;
class Drive;

class Navigator {
   public:
    Navigator(MotionTracker* motionTrackerPtr, Drive* drivePtr);

    bool isAtTarget();

    void setTargetPosition(int16_t targetXValue, int16_t targetYValue);

    void moveToTarget();

   private:
    MotionTracker* _motionTrackerPtr;
    Drive* _drivePtr;
    Position _targetPosition;

    int _inRangeTolerance = 30;
    int _angleTolerance = 10;

    int _getDistanceToTarget();
    Angle _getGlobalAngleToTarget();
    Angle _getLocalAngleToTarget();
};

#endif  // NAVIGATOR_H