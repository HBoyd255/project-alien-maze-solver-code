

#include "navigator.h"

#include "drive.h"
#include "motionTracker.h"

Navigator::Navigator(MotionTracker* motionTrackerPtr, Drive* drivePtr)
    : _motionTrackerPtr(motionTrackerPtr), _drivePtr(drivePtr) {}

bool Navigator::isAtTarget() {
    return this->_getDistanceToTarget() <= this->_inRangeTolerance;
}

void Navigator::setTargetPosition(int16_t targetXValue, int16_t targetYValue) {
    this->_targetPosition.x = targetXValue;
    this->_targetPosition.y = targetYValue;
}

void Navigator::moveToTarget() {
    Serial.print(" Moving To:");
    Serial.print(this->_targetPosition);
    Serial.print(" Turning:");
    Serial.print(this->_getLocalAngleToTarget());
    Serial.println();

    if (this->isAtTarget()) {
        Serial.println("Made it");
        this->_drivePtr->stop();
    } else {
        Angle angleToTurn = this->_getLocalAngleToTarget();

        // if target is in front of angle
        if ((angleToTurn > -90) && (angleToTurn < 90)) {
            if (angleToTurn <= -(this->_angleTolerance)) {
                this->_drivePtr->turnRight();
            } else if (angleToTurn >= (this->_angleTolerance)) {
                this->_drivePtr->turnLeft();

            } else {  // Point towards target

                int angleAdjustment =
                    constrain((int16_t)angleToTurn, -(this->_angleTolerance),
                              this->_angleTolerance);

                this->_drivePtr->forwards(angleAdjustment);
            }
        } else {  // If targer is begind robot
            Angle oppositeAngleToTurn = angleToTurn + 180;

            if (oppositeAngleToTurn <= -(this->_angleTolerance)) {
                this->_drivePtr->turnRight();
            } else if (oppositeAngleToTurn >= (this->_angleTolerance)) {
                this->_drivePtr->turnLeft();

            } else {  // pointing away from target

                this->_drivePtr->stop();
            }
        }
    }
}

int Navigator::_getDistanceToTarget() {
    Position robotPosition = this->_motionTrackerPtr->getPosition();

    int xDifference = this->_targetPosition.x - robotPosition.x;
    int yDifference = this->_targetPosition.y - robotPosition.y;

    return sqrt(xDifference * xDifference + yDifference * yDifference);
}

Angle Navigator::_getGlobalAngleToTarget() {
    Position robotPosition = this->_motionTrackerPtr->getPosition();

    int xDifference = this->_targetPosition.x - robotPosition.x;
    int yDifference = this->_targetPosition.y - robotPosition.y;

    return (Angle)degrees(atan2(yDifference, xDifference));
}

Angle Navigator::_getLocalAngleToTarget() {
    Angle currentRobotAngle = this->_motionTrackerPtr->getAngle();
    return this->_getGlobalAngleToTarget() - currentRobotAngle;
}

// void MotionTracker::moveToTarget() {
//     const int minSpeed = 40;
//     const int angleTolerance = 20;
//     const int withinRangeRadius = 50;
//
//     int16_t distanceFromTarget = this->getDistanceFromTarget();
//     Angle angleToTurn = getLocalAngleToTurn();
//
//     int8_t targetLeftSpeed = 0;
//     int8_t targetRightSpeed = 0;
//
//     if (distanceFromTarget > withinRangeRadius) {
//         if (angleToTurn < -angleTolerance) {
//             targetLeftSpeed = minSpeed;
//             targetRightSpeed = -minSpeed;
//
//         } else if (angleToTurn > angleTolerance) {
//             targetLeftSpeed = -minSpeed;
//             targetRightSpeed = minSpeed;
//         } else {  // Angle within tolerance
//
//             uint16_t angleAdjustment = constrain(
//                 (int16_t)angleToTurn, -angleTolerance, angleTolerance);
//
//             targetLeftSpeed = minSpeed - angleAdjustment;
//             targetRightSpeed = minSpeed + angleAdjustment;
//         }
//
//     } else {
//         targetLeftSpeed = 0;
//         targetRightSpeed = 0;
//     }
//
//     this->_leftMotorPtr->setVelocity(targetLeftSpeed);
//     this->_rightMotorPtr->setVelocity(targetRightSpeed);
//
//     //     Serial.print(" Rob:");
//     //     Serial.print(this->getPose());
//     //
//     //     Serial.print(" target:");
//     //     Serial.print(this->targetPosition);
//     //
//     //     Serial.print(" distance:");
//     //     Serial.print(distanceFromTarget);
//     //
//     //     Serial.print(" LS:");
//     //     Serial.print(targetLeftSpeed);
//     //     Serial.print(" RS:");
//     //     Serial.print(targetRightSpeed);
//     //
//     //     Serial.println();
// }