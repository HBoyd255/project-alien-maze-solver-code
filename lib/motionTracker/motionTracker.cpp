#include "motionTracker.h"

// TODO calibrate this
// This value also changes depending on the surface for some reason.
#define STEPS_PER_ROTATION 855

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