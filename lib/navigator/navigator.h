#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <queue>

#include "angleAndPosition.h"

// Forward declaration of the MotionTracker and Drive class.
class MotionTracker;
class Drive;

struct PathPoint {
    Position position = {0, 0};
    Angle angle = 0;
    bool usingPosition = true;

    operator String() const;
};

class Navigator {
   public:
    Navigator(MotionTracker* motionTrackerPtr, Drive* drivePtr);

    bool hasNoPath();

    void setTargetPosition(int targetXValue, int targetYValue);

    void simpleGoTo(int targetXValue, int targetYValue);

    void moveToTarget();
    void newMoveToTarget();

    String getPathAsString();

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

    bool _goToAngle(Angle angleToGoTo);
    bool _goToPosition(Position positionToGoTo);

    void _pushPosition(Position positionToPush);
    void _pushAngle(Angle angleToPush);

    void _clearQueue();

    std::queue<PathPoint> _pathQueue;
};

#endif  // NAVIGATOR_H