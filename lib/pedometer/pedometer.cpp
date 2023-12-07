#include "pedometer.h"

// TODO calibrate this
// This value also changes depending on the surface for some reason.
#define STEPS_PER_ROTATION 855

Pedometer::Pedometer(Motor* leftMotorPtr, Motor* rightMotorPtr) {
    this->leftMotorPtr = leftMotorPtr;
    this->rightMotorPtr = rightMotorPtr;
}

int32_t Pedometer::getDifference() {
    uint32_t leftDistance = this->leftMotorPtr->getDistanceTraveled();
    uint32_t rightDistance = this->rightMotorPtr->getDistanceTraveled();

    return rightDistance - leftDistance;
}

void Pedometer::normalizeAngle(int16_t* anglePrt) {
    // normalize the angle to be between 0 and 360

    // the angle offset signifies the angle lower bound.
    // for example, to normalize an angle between -179 and 180, the angle offset
    // would be 179.
    // as i want the angle to be between 0 and 360, the angle offset is 0.

    // TODO Decide if i want the angle to be -179 - 180 or 0 - 369

    const uint8_t angleOffset = 179;

    int16_t angle = *anglePrt;

    angle += angleOffset;

    angle %= 360;

    if (angle < 0) {
        angle += 360;
    }

    angle -= angleOffset;

    *anglePrt = angle;
}

int16_t Pedometer::calculateAngle() {
    int32_t wheelDifference = this->getDifference();

    // doing one rotation of the robot involves a difference in travel
    // distance of 780-820mm between the two motors. So the degree of
    // rotation is the difference between the two motor * 360 / the steps
    // difference per rotation.
    int16_t angle = wheelDifference * 360 / STEPS_PER_ROTATION;

    this->normalizeAngle(&angle);

    return angle;
}