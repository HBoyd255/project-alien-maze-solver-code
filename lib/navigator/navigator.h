#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <queue>

#include "angleAndPosition.h"

// Forward declaration of the MotionTracker and Drive class.
class MotionTracker;
class Drive;

class Navigator {
   public:
    Navigator(MotionTracker* motionTrackerPtr, Drive* drivePtr);

    bool gotOneInScope();

    void setTargetPosition(int targetXValue, int targetYValue);

    void simpleGoTo(int targetXValue, int targetYValue);

    void moveToTarget();

    void forwardsLeft();

   private:
    MotionTracker* _motionTrackerPtr;
    Drive* _drivePtr;
    Pose _targetPose;

    int _inRangeTolerance = 30;
    int _angleTolerance = 10;
    bool _hasTarget = false;

    int _getDistanceToTarget();
    Angle _getGlobalAngleToTarget();
    Angle _getLocalAngleToTarget();

    void _clearQueue();

    std::queue<Position> _pathQueue;
};

#endif  // NAVIGATOR_H