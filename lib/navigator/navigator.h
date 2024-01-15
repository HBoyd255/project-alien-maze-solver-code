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

    void turn(Angle localAngleToTurn, int distanceToDriveBeforeTurning = 0);

    void turnLeft(int distanceToDriveBeforeTurning = 0);
    void turnRight(int distanceToDriveBeforeTurning = 0);

    void moveToTarget();

    String getPathAsString();

    void hitBumper(byte bumperData);

    void goDirection(Angle angleToDrive);

   private:
    MotionTracker* _motionTrackerPtr;
    Drive* _drivePtr;

    int _inRangeTolerance = 30;
    int _angleTolerance = 10;
    bool _hasTarget = false;

    bool _goToAngle(Angle angleToGoTo);
    bool _goToPosition(Position positionToGoTo);

    void _pushPosition(Position positionToPush);
    void _pushLocalPosition(float localX, float LocalY);
    void _pushOffsetPosition(float offsetX, float offsetY);
    void _pushAngle(Angle angleToPush);

    void _clearQueue();

    void _simpleGoTo(int targetXValue, int targetYValue, bool goBack = false);

    std::queue<PathPoint> _pathQueue;
};

#endif  // NAVIGATOR_H