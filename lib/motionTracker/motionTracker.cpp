#include "motionTracker.h"

#include "bluetoothLowEnergy.h"

// TODO calibrate this
// This value also changes depending on the surface for some reason.
#define STEPS_PER_ROTATION 855

// The period to wait between updating the angle and position.
#define MOTION_TRACKER_POLL_RATE 10

MotionTracker::MotionTracker(BluetoothLowEnergy* blePtr, Motor* leftMotorPtr,
                             Motor* rightMotorPtr,
                             Infrared* frontLeftInfraredPtr,
                             Infrared* frontRightInfraredPtr)
    : _blePtr(blePtr),
      _leftMotorPtr(leftMotorPtr),
      _rightMotorPtr(rightMotorPtr),
      _frontLeftInfraredPtr(frontLeftInfraredPtr),
      _frontRightInfraredPtr(frontRightInfraredPtr) {}

// TODO Check if the vales of distance traveled has changes before doing all
// that maths
Angle MotionTracker::angleFromOdometry() {
    int32_t leftTravelDistance = this->_leftMotorPtr->getDistanceTraveled();
    int32_t rightTravelDistance = this->_rightMotorPtr->getDistanceTraveled();

    int16_t motorTravelDifference = rightTravelDistance - leftTravelDistance;

    Angle angle = (int16_t)(motorTravelDifference * 360 / STEPS_PER_ROTATION);

    return angle;
}

Angle MotionTracker::angleFromFrontIR() {
    // TODO move this into somewhere different
    // The measurement was obtained from my cad file.
    const uint16_t distanceBetweenSensors = 73;

    uint16_t L = _frontLeftInfraredPtr->read();
    uint16_t R = _frontRightInfraredPtr->read();

    if (L == 639 || R == 639) {
        // 91 is just supposed to represent that the angle cannot be measured.
        // Ill fix this later.
        return 91;
    }

    int16_t height = R - L;

    Angle angle = (int16_t)degrees(atan2(height, distanceBetweenSensors));

    if (angle > 45 || angle < -45) {
        return 91;
    }

    return angle;
}

bool MotionTracker::updateAngle() {
    Angle newAngle = 90 + this->angleFromOdometry();

    bool hasMoved = false;
    if (this->_currentAngle != newAngle) {
        hasMoved = true;
        this->_currentAngle = newAngle;
    }

    return hasMoved;
}

bool MotionTracker::updatePosition() {
    static int32_t AverageDistance = 0;
    static int32_t lastAverageDistance = 0;
    static int32_t difference;

    bool hasMoved = false;

    AverageDistance = this->_averageTravelDistance();
    difference = AverageDistance - lastAverageDistance;
    lastAverageDistance = AverageDistance;

    if (difference != 0) {
        hasMoved = true;

        float sinAngle = sin(this->_currentAngle.toRadians());
        float cosAngle = cos(this->_currentAngle.toRadians());

        float xDif = difference * cosAngle;
        float yDif = difference * sinAngle;

        this->_currentPosition.x += xDif;
        this->_currentPosition.y += yDif;
    }
    return hasMoved;
}

void MotionTracker::poll(bool sendOverBLE = false) {
    static PassiveSchedule motionTrackerSchedule(MOTION_TRACKER_POLL_RATE);

    if (motionTrackerSchedule.isReadyToRun()) {
        bool angleHasChanged = this->updateAngle();

        bool positionHasChanged = this->updatePosition();

        bool poseHasChanged = angleHasChanged || positionHasChanged;

        if (poseHasChanged && sendOverBLE) {
            this->_blePtr->sendRobotPose(this->getPose());
        }
    }
}

Pose MotionTracker::getPose() {
    Pose poseToReturn;
    poseToReturn.angle = this->_currentAngle;
    poseToReturn.position = this->_currentPosition;
    return poseToReturn;
}

void MotionTracker::setTargetPosition(int16_t xValue, int16_t yValue) {
    this->targetPosition.x = xValue;
    this->targetPosition.y = yValue;
}

void MotionTracker::moveToTarget() {
    const int minSpeed = 40;
    const int angleTolerance = 20;
    const int withinRangeRadius = 50;

    int16_t distanceFromTarget = this->getDistanceFromTarget();
    Angle angleToTurn = getLocalAngleToTurn();

    int8_t targetLeftSpeed = 0;
    int8_t targetRightSpeed = 0;

    if (distanceFromTarget > withinRangeRadius) {
        if (angleToTurn < -angleTolerance) {
            targetLeftSpeed = minSpeed;
            targetRightSpeed = -minSpeed;

        } else if (angleToTurn > angleTolerance) {
            targetLeftSpeed = -minSpeed;
            targetRightSpeed = minSpeed;
        } else {  // Angle within tolerance

            uint16_t angleAdjustment = constrain(
                (int16_t)angleToTurn, -angleTolerance, angleTolerance);

            targetLeftSpeed = minSpeed - angleAdjustment;
            targetRightSpeed = minSpeed + angleAdjustment;
        }

    } else {
        targetLeftSpeed = 0;
        targetRightSpeed = 0;
    }

    this->_leftMotorPtr->setVelocity(targetLeftSpeed);
    this->_rightMotorPtr->setVelocity(targetRightSpeed);

//     Serial.print(" Rob:");
//     Serial.print(this->getPose());
// 
//     Serial.print(" target:");
//     Serial.print(this->targetPosition);
// 
//     Serial.print(" distance:");
//     Serial.print(distanceFromTarget);
// 
//     Serial.print(" LS:");
//     Serial.print(targetLeftSpeed);
//     Serial.print(" RS:");
//     Serial.print(targetRightSpeed);
// 
//     Serial.println();

}

Angle MotionTracker::getGlobalAngleToPoint() {
    int xDif = this->targetPosition.x - this->_currentPosition.x;
    int yDif = this->targetPosition.y - this->_currentPosition.y;

    return (Angle)degrees(atan2(yDif, xDif));
}

Angle MotionTracker::getLocalAngleToTurn() {
    return this->getGlobalAngleToPoint() - this->_currentAngle;
}

int16_t MotionTracker::getDistanceFromTarget() {
    float xDif = this->targetPosition.x - this->_currentPosition.x;
    float yDif = this->targetPosition.y - this->_currentPosition.y;

    return sqrt(xDif * xDif + yDif * yDif);
}

int32_t MotionTracker::_averageTravelDistance() {
    int32_t leftTravelDistance = this->_leftMotorPtr->getDistanceTraveled();
    int32_t rightTravelDistance = this->_rightMotorPtr->getDistanceTraveled();

    int32_t averageTravelDistance =
        (leftTravelDistance + rightTravelDistance) / 2;

    return averageTravelDistance;
}
