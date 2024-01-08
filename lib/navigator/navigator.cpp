

#include "navigator.h"

#include "drive.h"
#include "motionTracker.h"

Navigator::Navigator(MotionTracker* motionTrackerPtr, Drive* drivePtr)
    : _motionTrackerPtr(motionTrackerPtr), _drivePtr(drivePtr) {}

bool Navigator::gotOneInScope() { return this->_hasTarget; }

void Navigator::setTargetPosition(int targetXValue, int targetYValue) {
    this->_hasTarget = true;
    this->_targetPose.position.x = targetXValue;
    this->_targetPose.position.y = targetYValue;
    this->_targetPose.angle = this->_motionTrackerPtr->getAngle();
}
void Navigator::simpleGoTo(int localX, int localY) {
    Angle robotAngle = this->_motionTrackerPtr->getAngle();

    Angle rotationAngle = robotAngle - 90;

    float sinAngle = sin(rotationAngle.toRadians());
    float cosAngle = cos(rotationAngle.toRadians());

    float rotatedLocalX = localX * cosAngle - localY * sinAngle;
    float rotatedLocalY = localY * cosAngle + localX * sinAngle;

    Position currentPosition = this->_motionTrackerPtr->getPosition();

    int newTargetX = currentPosition.x + rotatedLocalX;
    int newTargetY = currentPosition.y + rotatedLocalY;

    this->setTargetPosition(newTargetX, newTargetY);
}

void Navigator::moveToTarget() {
    if (this->_hasTarget == false) {
        return;
    }

    const int upperAngle = this->_angleTolerance;
    const int lowerAngle = -(this->_angleTolerance);

    // If in range
    if (this->_getDistanceToTarget() <= this->_inRangeTolerance) {
        Angle angleToTurnToPoint =
            _targetPose.angle - this->_motionTrackerPtr->getAngle();

        Serial.println(angleToTurnToPoint);

        if (angleToTurnToPoint <= lowerAngle) {
            this->_drivePtr->turnRight();

        } else if (angleToTurnToPoint >= upperAngle) {
            this->_drivePtr->turnLeft();

        } else {  // If pointing towards target.
            this->_drivePtr->stop();
            _hasTarget = false;
        }
        return;
    }

    Angle angleToTurn = this->_getLocalAngleToTarget();
    bool drivingForwards = true;

    // if target is in behind the robot, drive backwards to the target.
    if ((angleToTurn < -90) || (angleToTurn > 90)) {
        angleToTurn += 180;
        drivingForwards = false;
    }

    if (angleToTurn <= lowerAngle) {
        this->_drivePtr->turnRight();

    } else if (angleToTurn >= upperAngle) {
        this->_drivePtr->turnLeft();

    } else {  // If pointing towards target.

        int angleAdjustment =
            constrain((int)angleToTurn, lowerAngle, upperAngle);

        if (drivingForwards) {
            this->_drivePtr->forwards(angleAdjustment);
        } else {
            this->_drivePtr->backwards(angleAdjustment);
        }
    }
}

void Navigator::forwardsLeft() {}

int Navigator::_getDistanceToTarget() {
    Position robotPosition = this->_motionTrackerPtr->getPosition();

    int xDifference = this->_targetPose.position.x - robotPosition.x;
    int yDifference = this->_targetPose.position.y - robotPosition.y;

    return sqrt(xDifference * xDifference + yDifference * yDifference);
}

Angle Navigator::_getGlobalAngleToTarget() {
    Position robotPosition = this->_motionTrackerPtr->getPosition();

    int xDifference = this->_targetPose.position.x - robotPosition.x;
    int yDifference = this->_targetPose.position.y - robotPosition.y;

    return (Angle)degrees(atan2(yDifference, xDifference));
}

Angle Navigator::_getLocalAngleToTarget() {
    Angle currentRobotAngle = this->_motionTrackerPtr->getAngle();
    return this->_getGlobalAngleToTarget() - currentRobotAngle;
}

void Navigator::_clearQueue() {
    while () {
        this->_pathQueue()
    }
}