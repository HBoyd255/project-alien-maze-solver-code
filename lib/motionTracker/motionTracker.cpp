#include "motionTracker.h"

#include "mazeConstants.h"

// Calibrated value
#define STEPS_PER_ROTATION 855

// The period to wait between updating the angle and position.
#define MOTION_TRACKER_POLL_RATE 10

MotionTracker::MotionTracker(Motor* leftMotor_P, Motor* rightMotor_P,
                             Angle statingAngle)
    : _leftMotor_P(leftMotor_P),
      _rightMotor_P(rightMotor_P),
      _statingAngle(statingAngle),
      _currentAngle(statingAngle),
      _angleCalibration(0),
      _currentAverageDistance(0),
      _lastAverageDistance(0),

      _pollSchedule(MOTION_TRACKER_POLL_RATE) {}

// TODO Check if the values of distance traveled has changes before doing all
// that maths
Angle MotionTracker::angleFromOdometry() {
    int leftTravelDistance = this->_leftMotor_P->getDistanceTraveled();
    int rightTravelDistance = this->_rightMotor_P->getDistanceTraveled();

    int16_t motorTravelDifference = rightTravelDistance - leftTravelDistance;

    Angle angle = (int16_t)(motorTravelDifference * 360 / STEPS_PER_ROTATION);

    return angle;
}

bool MotionTracker::updateAngle() {
    Angle newAngle = this->_statingAngle + this->angleFromOdometry() +
                     this->_angleCalibration;

    bool hasMoved = false;
    if (this->_currentAngle != newAngle) {
        hasMoved = true;
        this->_currentAngle = newAngle;
    }

    return hasMoved;
}

bool MotionTracker::updatePosition() {
    bool hasMoved = false;

    int changeInDistance = this->_getChangeInDistance();

    if (changeInDistance != 0) {
        hasMoved = true;

        float sinAngle = sin(this->_currentAngle.getRadians());
        float cosAngle = cos(this->_currentAngle.getRadians());

        float xDif = changeInDistance * cosAngle;
        float yDif = changeInDistance * sinAngle;

        this->_currentPosition.x += xDif;
        this->_currentPosition.y += yDif;
    }
    return hasMoved;
}

bool MotionTracker::poll() {
    if (_pollSchedule.isReadyToRun()) {
        bool angleHasChanged = this->updateAngle();

        bool positionHasChanged = this->updatePosition();

        bool poseHasChanged = angleHasChanged || positionHasChanged;

        return poseHasChanged;
    }
    // False indicating that nothing has changed since the last poll;
    return false;
}

int MotionTracker::recalibratePosition(int frontDistance, int leftDistance) {
    int closeThreshold = 200;

    Angle angleDrift = this->_currentAngle.OrthogonalOffset();

    this->_angleCalibration += angleDrift;
    this->updateAngle();

    bool facingBottom = this->_currentAngle.isPointingDown();
    bool facingLeft = this->_currentAngle.isPointingLeft();
    bool facingTop = this->_currentAngle.isPointingUp();
    bool facingRight = this->_currentAngle.isPointingRight();

    Position currentPosition = this->getPosition();

    bool closeToBottomWall = currentPosition.y < closeThreshold;
    bool closeToLeftWall = currentPosition.x < closeThreshold;
    bool closeToTopWall = currentPosition.y > (MAZE_LENGTH - closeThreshold);
    bool closeToRightWall = currentPosition.x > (MAZE_WIDTH - closeThreshold);

    bool leftDistanceValid = (leftDistance != -1);

    // This bit looks messy but all it is doing is returning 1 if it is at a
    // wall, and returning 2 if it is in a corner.

    if ((facingBottom) && (closeToBottomWall)) {
        this->_currentPosition.y = frontDistance;

        if (closeToRightWall && leftDistanceValid) {
            this->_currentPosition.x = MAZE_WIDTH - leftDistance;
            return 2;
        }
        return 1;
    } else if ((facingLeft) && (closeToLeftWall)) {
        this->_currentPosition.x = frontDistance;

        if (closeToBottomWall && leftDistanceValid) {
            this->_currentPosition.y = leftDistance;
            return 2;
        }
        return 1;
    } else if ((facingTop) && (closeToTopWall)) {
        this->_currentPosition.y = MAZE_LENGTH - frontDistance;

        if (closeToLeftWall && leftDistanceValid) {
            this->_currentPosition.x = leftDistance;
            return 2;
        }
        return 1;
    } else if ((facingRight) && (closeToRightWall)) {
        this->_currentPosition.x = MAZE_WIDTH - frontDistance;

        if (closeToTopWall && leftDistanceValid) {
            this->_currentPosition.y = MAZE_LENGTH - leftDistance;
            return 2;
        }
        return 1;
    }

    return 0;
}

Angle MotionTracker::getAngle() { return this->_currentAngle; }

Position MotionTracker::getPosition() { return this->_currentPosition; }

Pose MotionTracker::getPose() {
    Pose poseToReturn;
    poseToReturn.angle = this->getAngle();
    poseToReturn.position = this->getPosition();
    return poseToReturn;
}

int MotionTracker::_getAverageDistance() {
    int leftTravelDistance = this->_leftMotor_P->getDistanceTraveled();
    int rightTravelDistance = this->_rightMotor_P->getDistanceTraveled();

    int averageTravelDistance = (leftTravelDistance + rightTravelDistance) / 2;

    return averageTravelDistance;
}

int MotionTracker::_getChangeInDistance() {
    this->_currentAverageDistance = this->_getAverageDistance();
    int changeInDistance = _currentAverageDistance - _lastAverageDistance;
    this->_lastAverageDistance = this->_currentAverageDistance;

    return changeInDistance;
}
