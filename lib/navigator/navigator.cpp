

#include "navigator.h"

#include "drive.h"
#include "motionTracker.h"

PathPoint::operator String() const {
    String stringToReturn = "[";

    if (this->usingPosition) {
        stringToReturn += "Pos:";
        stringToReturn += this->position;
    } else {
        stringToReturn += "Ang:";
        stringToReturn += this->angle;
    }
    stringToReturn += "]";
    return stringToReturn;
}

Navigator::Navigator(MotionTracker* motionTrackerPtr, Drive* drivePtr)
    : _motionTrackerPtr(motionTrackerPtr), _drivePtr(drivePtr) {}

bool Navigator::hasNoPath() { return this->_pathQueue.empty(); }

void Navigator::_simpleGoTo(int localX, int localY, bool goBack) {
    Angle angleToPoint =
        this->_motionTrackerPtr->getAngle().toClosestRightAngle();

    // Destructively search the que fro an angle
    while (!this->_pathQueue.empty()) {
        PathPoint item = this->_pathQueue.front();

        if (item.usingPosition == false) {
            angleToPoint = item.angle;
            break;
        }
        this->_pathQueue.pop();
    }

    this->_clearQueue();

    if (goBack) {
        if (localY <= 0) {
            this->_pushLocalPosition(0, -50);
        } else {
            this->_pushLocalPosition(0, 50);
        }
    }

    this->_pushLocalPosition(localX, localY);
    this->_pushAngle(angleToPoint);
}

void Navigator::turn(Angle localAngleToTurn, int distanceToDriveBeforeTurning) {
    this->_clearQueue();

    if (distanceToDriveBeforeTurning != 0) {
        this->_pushLocalPosition(0, distanceToDriveBeforeTurning);
    }

    Angle currentAngle = this->_motionTrackerPtr->getAngle();
    Angle angleToPointTo =
        currentAngle.toClosestRightAngle() + localAngleToTurn;

    this->_pushAngle(angleToPointTo);
}

void Navigator::turnLeft(int distanceToDriveBeforeTurning) {
    this->turn(90, distanceToDriveBeforeTurning);
}
void Navigator::turnRight(int distanceToDriveBeforeTurning) {
    this->turn(-90, distanceToDriveBeforeTurning);
}

void Navigator::moveToTarget() {
    if (this->hasNoPath()) {
        return;
    }

    PathPoint currentTarget = this->_pathQueue.front();

    bool reachedDestination = false;

    if (currentTarget.usingPosition) {
        reachedDestination = this->_goToPosition(currentTarget.position);
    } else {
        reachedDestination = this->_goToAngle(currentTarget.angle);
    }

    if (reachedDestination) {
        this->_drivePtr->stop();
        this->_pathQueue.pop();
    }
}

void Navigator::hitBumper(byte bumperData) {
    bool frontPressed = (bumperData & 1);
    bool frontRightPressed = (bumperData & 2);
    bool rightPressed = (bumperData & 4);
    bool backRightPressed = (bumperData & 8);
    bool backPressed = (bumperData & 16);
    bool backLeftPressed = (bumperData & 32);
    bool leftPressed = (bumperData & 64);
    bool frontLeftPressed = (bumperData & 128);

    // This is by far the worst code I have written in my life.

    if (frontPressed) {
        if (frontLeftPressed) {
            this->_simpleGoTo(25, -50, true);
        } else if (frontRightPressed) {
            this->_simpleGoTo(-25, -50, true);
        } else {
            this->_simpleGoTo(-25, -50, false);
        }

    } else if (backPressed) {
        if (backLeftPressed) {
            this->_simpleGoTo(-25, 50, true);
        } else if (backRightPressed) {
            this->_simpleGoTo(25, 50, true);
        } else {
            this->_simpleGoTo(25, 50, false);
        }

    } else if (leftPressed) {
        if (frontLeftPressed) {
            this->_simpleGoTo(50, -1, true);
        } else if (backLeftPressed) {
            this->_simpleGoTo(50, 1, true);
        } else {
            this->_simpleGoTo(50, 0, false);
        }

    } else if (rightPressed) {
        if (frontRightPressed) {
            this->_simpleGoTo(-50, -1, true);
        } else if (backRightPressed) {
            this->_simpleGoTo(-50, 1, true);
        } else {
            this->_simpleGoTo(-50, 0, false);
        }
    } else if (frontLeftPressed) {
        this->_simpleGoTo(100, -50, true);
    } else if (frontRightPressed) {
        this->_simpleGoTo(-100, -50, true);
    } else if (backLeftPressed) {
        this->_simpleGoTo(100, 50, true);
    } else if (backRightPressed) {
        this->_simpleGoTo(-100, 50, true);
    }
}

void Navigator::goDirection(Angle angleToDrive) {
    const int distanceToDrive = 100;
    int localX = distanceToDrive * sin(angleToDrive.toRadians());
    int localY = distanceToDrive * cos(angleToDrive.toRadians());
    this->_pushOffsetPosition(localX, localY);
}

String Navigator::getPathAsString() {
    std::queue<PathPoint> tempQueue = this->_pathQueue;

    String stringToReturn = "(";

    while (!tempQueue.empty()) {
        PathPoint value = tempQueue.front();
        stringToReturn += value;
        tempQueue.pop();
        if (!tempQueue.empty()) {
            stringToReturn += ",";
        }
    }
    stringToReturn += ")";

    return stringToReturn;
}

bool Navigator::_goToAngle(Angle angleToGoTo) {
    Angle currentAngle = this->_motionTrackerPtr->getAngle();
    Angle angleToTurn = angleToGoTo - currentAngle;

    const int upperAngle = this->_angleTolerance;
    const int lowerAngle = -(this->_angleTolerance);

    if (angleToTurn <= lowerAngle) {
        this->_drivePtr->turnRight();
    } else if (angleToTurn >= upperAngle) {
        this->_drivePtr->turnLeft();
    } else {
        return true;
    }
    return false;
}

bool Navigator::_goToPosition(Position positionToGoTo) {
    Position currentPosition = this->_motionTrackerPtr->getPosition();

    int distanceToTarget = currentPosition.calculateDistanceTo(positionToGoTo);
    if (distanceToTarget < this->_inRangeTolerance) {
        this->_drivePtr->stop();
        return true;
    }

    Angle currentAngle = this->_motionTrackerPtr->getAngle();
    Angle globalAngleToTarget = currentPosition.calculateAngleTo(positionToGoTo);
    Angle angleToTurn = globalAngleToTarget - currentAngle;
    bool drivingForwards = true;

    const int upperAngle = this->_angleTolerance;
    const int lowerAngle = -(this->_angleTolerance);

    // if target is in behind the robot, drive backwards to the target.
    if ((angleToTurn < -90) || (angleToTurn > 90)) {
        globalAngleToTarget += 180;
        angleToTurn += 180;
        drivingForwards = false;
    }

    bool pointingToTarget = this->_goToAngle(globalAngleToTarget);

    if (pointingToTarget) {
        // If pointing towards target.

        int angleAdjustment =
            constrain((int)angleToTurn, lowerAngle, upperAngle);

        if (drivingForwards) {
            this->_drivePtr->forwards(angleAdjustment);
        } else {
            this->_drivePtr->backwards(angleAdjustment);
        }
    }

    return false;  // signifies destination has not been reached
}

void Navigator::_pushPosition(Position positionToPush) {
    PathPoint pointToPush;

    pointToPush.position = positionToPush;
    pointToPush.usingPosition = true;

    this->_pathQueue.push(pointToPush);
}
void Navigator::_pushLocalPosition(float localX, float localY) {
    Angle currentAngle = this->_motionTrackerPtr->getAngle();

    Angle rotationAngle = currentAngle - 90;

    float sinAngle = sin(rotationAngle.toRadians());
    float cosAngle = cos(rotationAngle.toRadians());

    float rotatedLocalX = localX * cosAngle - localY * sinAngle;
    float rotatedLocalY = localY * cosAngle + localX * sinAngle;

    Position currentPosition = this->_motionTrackerPtr->getPosition();

    Position newTargetPosition;
    newTargetPosition.x = currentPosition.x + rotatedLocalX;
    newTargetPosition.y = currentPosition.y + rotatedLocalY;

    this->_pushPosition(newTargetPosition);
}

void Navigator::_pushOffsetPosition(float offsetX, float offsetY) {
    Position currentPosition = this->_motionTrackerPtr->getPosition();

    Position newTargetPosition;
    newTargetPosition.x = currentPosition.x + offsetX;
    newTargetPosition.y = currentPosition.y + offsetY;

    this->_pushPosition(newTargetPosition);
}

void Navigator::_pushAngle(Angle angleToPush) {
    PathPoint pointToPush;

    pointToPush.angle = angleToPush;
    pointToPush.usingPosition = false;

    this->_pathQueue.push(pointToPush);
}

void Navigator::_clearQueue() {
    while (!this->_pathQueue.empty()) {
        this->_pathQueue.pop();
    }
}
