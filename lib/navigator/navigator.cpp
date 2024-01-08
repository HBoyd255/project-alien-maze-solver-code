

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

void Navigator::setTargetPosition(int targetXValue, int targetYValue) {
    this->_hasTarget = true;
    this->_targetPose.position.x = targetXValue;
    this->_targetPose.position.y = targetYValue;
    this->_targetPose.angle = this->_motionTrackerPtr->getAngle();
}
void Navigator::simpleGoTo(int localX, int localY) {
    Angle currentAngle = this->_motionTrackerPtr->getAngle();

    Angle rotationAngle = currentAngle - 90;

    float sinAngle = sin(rotationAngle.toRadians());
    float cosAngle = cos(rotationAngle.toRadians());

    float rotatedLocalX = localX * cosAngle - localY * sinAngle;
    float rotatedLocalY = localY * cosAngle + localX * sinAngle;

    Position currentPosition = this->_motionTrackerPtr->getPosition();

    float newTargetX = currentPosition.x + rotatedLocalX;
    float newTargetY = currentPosition.y + rotatedLocalY;

    Position positionToGo;
    positionToGo.x = newTargetX;
    positionToGo.y = newTargetY;

    Angle angleToPoint = currentAngle;

    // Add the bit to find out if an angle exists;

    this->_clearQueue();
    this->_pushPosition(positionToGo);
    this->_pushAngle(angleToPoint);
}

// void Navigator::moveToTarget() {
//     if (this->_hasTarget == false) {
//         return;
//     }
// 
//     const int upperAngle = this->_angleTolerance;
//     const int lowerAngle = -(this->_angleTolerance);
// 
//     // If in range
//     if (this->_getDistanceToTarget() <= this->_inRangeTolerance) {
//         Angle angleToTurnToPoint =
//             _targetPose.angle - this->_motionTrackerPtr->getAngle();
// 
//         Serial.println(angleToTurnToPoint);
// 
//         if (angleToTurnToPoint <= lowerAngle) {
//             this->_drivePtr->turnRight();
// 
//         } else if (angleToTurnToPoint >= upperAngle) {
//             this->_drivePtr->turnLeft();
// 
//         } else {  // If pointing towards target.
//             this->_drivePtr->stop();
//             _hasTarget = false;
//         }
//         return;
//     }
// 
//     Angle angleToTurn = this->_getLocalAngleToTarget();
//     bool drivingForwards = true;
// 
//     // if target is in behind the robot, drive backwards to the target.
//     if ((angleToTurn < -90) || (angleToTurn > 90)) {
//         angleToTurn += 180;
//         drivingForwards = false;
//     }
// 
//     if (angleToTurn <= lowerAngle) {
//         this->_drivePtr->turnRight();
// 
//     } else if (angleToTurn >= upperAngle) {
//         this->_drivePtr->turnLeft();
// 
//     } else {  // If pointing towards target.
// 
//         int angleAdjustment =
//             constrain((int)angleToTurn, lowerAngle, upperAngle);
// 
//         if (drivingForwards) {
//             this->_drivePtr->forwards(angleAdjustment);
//         } else {
//             this->_drivePtr->backwards(angleAdjustment);
//         }
//     }
// }

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

    int distanceToTarget = currentPosition.distanceToPosition(positionToGoTo);
    if (distanceToTarget < this->_inRangeTolerance) {
        this->_drivePtr->stop();
        return true;
    }

    Angle currentAngle = this->_motionTrackerPtr->getAngle();
    Angle globalAngleToTarget = currentPosition.angleToPosition(positionToGoTo);
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

    // Serial.print(" Dis:");
    // Serial.print(distanceToTarget);
    // Serial.print(" Ang:");
    // Serial.print(globalAngleToTarget);
    // Serial.print(" Tun:");
    // Serial.print(angleToTurn);
    // Serial.print(" drivingForwards:");
    // Serial.print(drivingForwards);

    //     if (angleToTurn <= lowerAngle) {
    //         this->_drivePtr->turnRight();
    //
    //     } else if (angleToTurn >= upperAngle) {
    //         this->_drivePtr->turnLeft();
    //
    //     } else {  // If pointing towards target.
    //
    //         int angleAdjustment =
    //             constrain((int)angleToTurn, lowerAngle, upperAngle);
    //
    //         if (drivingForwards) {
    //             this->_drivePtr->forwards(angleAdjustment);
    //         } else {
    //             this->_drivePtr->backwards(angleAdjustment);
    //         }
    //     }

    return false;  // SIgnifies destination has not been reached
}

void Navigator::newMoveToTarget() {
    Serial.println();

    if (this->hasNoPath()) {
        return;
    }
    Serial.print("following Path:");
    Serial.print(this->getPathAsString());
    Serial.print(" ");

    PathPoint currentAction = this->_pathQueue.front();

    bool reachedDestination = false;

    if (currentAction.usingPosition) {
        reachedDestination = this->_goToPosition(currentAction.position);
    } else {
        reachedDestination = this->_goToAngle(currentAction.angle);
    }

    if (reachedDestination) {
        this->_pathQueue.pop();
    }
}

void hitBumper(byte bumperData) {}

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

void Navigator::_pushPosition(Position positionToPush) {
    PathPoint pointToPush;

    pointToPush.position = positionToPush;
    pointToPush.usingPosition = true;

    this->_pathQueue.push(pointToPush);
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

//
// this->_clearQueue();
//
// PathPoint pointA;
//
// pointA.position = {23, 45};
// pointA.angle = 69;
//
// this->_pathQueue.push(pointA);
//
// PathPoint pointB;
//
// pointB.position = {23, 45};
// pointB.angle = 69;
// pointB.usingPosition = false;
//
// this->_pathQueue.push(pointB);
//
// Serial.print(this->getPathAsString());
// Serial.print(" -- ");
//
// this->_pushAngle(190);
// this->_pushPosition({1, 2});
//
// Serial.print(this->getPathAsString());
// Serial.println();