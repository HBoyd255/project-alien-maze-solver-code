#include "motionTracker.h"

// TODO calibrate this
// This value also changes depending on the surface for some reason.
#define STEPS_PER_ROTATION 855

// The period to wait between updating the angle and position.
#define MOTION_TRACKER_POLL_RATE 10

MotionTracker::MotionTracker(Motor* leftMotorPtr, Motor* rightMotorPtr,
                             Infrared* frontLeftInfraredPtr,
                             Infrared* frontRightInfraredPtr) {
    _leftMotorPtr = leftMotorPtr;
    _rightMotorPtr = rightMotorPtr;
    _frontLeftInfraredPtr = frontLeftInfraredPtr;
    _frontRightInfraredPtr = frontRightInfraredPtr;
}

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

void MotionTracker::updateAngle() {
    // TODO update 90 to a constant
    this->_currentAngle = this->angleFromOdometry() + 90;
}

void MotionTracker::updatePosition() {
    static int32_t AverageDistance = 0;
    static int32_t lastAverageDistance = 0;
    static int32_t difference;

    AverageDistance = this->_averageTravelDistance();
    difference = AverageDistance - lastAverageDistance;
    lastAverageDistance = AverageDistance;

    if (difference != 0) {
        float xDif = difference * cos(this->_currentAngle.toRadians());
        float yDif = difference * sin(this->_currentAngle.toRadians());

        this->_currentPosition.x += xDif;
        this->_currentPosition.y += yDif;
    }
}

void MotionTracker::poll() {
    static PassiveSchedule motionTrackerSchedule(MOTION_TRACKER_POLL_RATE);

    if (motionTrackerSchedule.isReadyToRun()) {
        this->updateAngle();
        this->updatePosition();
    }
}

Pose MotionTracker::getPose() {
    Pose poseToReturn;
    poseToReturn.angle = this->_currentAngle;
    poseToReturn.position = this->_currentPosition;
    return poseToReturn;
}

int32_t MotionTracker::_averageTravelDistance() {
    int32_t leftTravelDistance = this->_leftMotorPtr->getDistanceTraveled();
    int32_t rightTravelDistance = this->_rightMotorPtr->getDistanceTraveled();

    int32_t averageTravelDistance =
        (leftTravelDistance + rightTravelDistance) / 2;

    return averageTravelDistance;
}