#include "motionTracker.h"

#define STEPS_PER_ROTATION 855

// The period to wait between updating the angle and position.
#define MOTION_TRACKER_POLL_RATE 10

MotionTracker::MotionTracker(Motor* leftMotorPtr, Motor* rightMotorPtr,
                             Infrared* frontLeftInfraredPtr,
                             Infrared* frontRightInfraredPtr,
                             Angle statingAngle)
    : _leftMotorPtr(leftMotorPtr),
      _rightMotorPtr(rightMotorPtr),
      _frontLeftInfraredPtr(frontLeftInfraredPtr),
      _frontRightInfraredPtr(frontRightInfraredPtr),
      _statingAngle(statingAngle) {}

// TODO Check if the vales of distance traveled has changes before doing all
// that maths
Angle MotionTracker::angleFromOdometry() {
    int32_t leftTravelDistance = this->_leftMotorPtr->getDistanceTraveled();
    int32_t rightTravelDistance = this->_rightMotorPtr->getDistanceTraveled();

    int16_t motorTravelDifference = rightTravelDistance - leftTravelDistance;

    Angle angle = (int16_t)(motorTravelDifference * 360 / STEPS_PER_ROTATION);

    return angle;
}

// TODO remove this
//  Angle MotionTracker::angleFromFrontIR() {
//      // TODO move this into somewhere different
//      // The measurement was obtained from my cad file.
//      const uint16_t distanceBetweenSensors = 73;
//
//      uint16_t L = _frontLeftInfraredPtr->read();
//      uint16_t R = _frontRightInfraredPtr->read();
//
//      if (L == 639 || R == 639) {
//          // 91 is just supposed to represent that the angle cannot be
//          measured.
//          // Ill fix this later.
//          return 91;
//      }
//
//      int16_t height = R - L;
//
//      Angle angle = (int16_t)degrees(atan2(height, distanceBetweenSensors));
//
//      if (angle > 45 || angle < -45) {
//          return 91;
//      }
//
//      return angle;
//  }

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

bool MotionTracker::poll() {
    static PassiveSchedule motionTrackerSchedule(MOTION_TRACKER_POLL_RATE);

    if (motionTrackerSchedule.isReadyToRun()) {
        bool angleHasChanged = this->updateAngle();

        bool positionHasChanged = this->updatePosition();

        bool poseHasChanged = angleHasChanged || positionHasChanged;

        return poseHasChanged;
    }
    // False indicating that nothing has changed since the last poll;
    return false;
}

// ☝️🤓
int MotionTracker::umActually(int frontDistance, int leftDistance) {
    // TODO fix this
    int closeThreshold = 200;

    const int mazeWidth = 1500;
    const int mazeHeight = 2000;

    Angle angleDrift = this->_currentAngle.OrthogonalOffset();

    this->_angleCalibration += angleDrift;
    this->updateAngle();

    // TODO refactor this.
    bool facingBottom = (this->_currentAngle == -90);
    bool facingLeft = (this->_currentAngle == 180);
    bool facingTop = (this->_currentAngle == 90);
    bool facingRight = (this->_currentAngle == 0);

    Position currentPosition = this->getPosition();

    bool closeToBottomWall = currentPosition.y < closeThreshold;
    bool closeToLeftWall = currentPosition.x < closeThreshold;
    bool closeToTopWall = currentPosition.y > (mazeHeight - closeThreshold);
    bool closeToRightWall = currentPosition.x > (mazeWidth - closeThreshold);

    //     Serial.print(" facingRight:");
    //     Serial.print(facingRight);
    //     Serial.print(" facingTop:");
    //     Serial.print(facingTop);
    //     Serial.print(" facingLeft:");
    //     Serial.print(facingLeft);
    //     Serial.print(" facingBottom:");
    //     Serial.print(facingBottom);
    //
    //     Serial.print(" closeToRightWall:");
    //     Serial.print(closeToRightWall);
    //     Serial.print(" closeToTopWall:");
    //     Serial.print(closeToTopWall);
    //     Serial.print(" closeToLeftWall:");
    //     Serial.print(closeToLeftWall);
    //     Serial.print(" closeToBottomWall:");
    //     Serial.println(closeToBottomWall);

    bool leftDistanceValid = (leftDistance != -1);

    if ((facingBottom) && (closeToBottomWall)) {
        this->_currentPosition.y = frontDistance;

        if (closeToRightWall && leftDistanceValid) {
            this->_currentPosition.x = mazeWidth - leftDistance;
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
        this->_currentPosition.y = mazeHeight - frontDistance;

        if (closeToLeftWall && leftDistanceValid) {
            this->_currentPosition.x = leftDistance;
            return 2;
        }
        return 1;

    } else if ((facingRight) && (closeToRightWall)) {
        this->_currentPosition.x = mazeWidth - frontDistance;

        if (closeToTopWall && leftDistanceValid) {
            this->_currentPosition.y = mazeHeight - leftDistance;
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

int32_t MotionTracker::_averageTravelDistance() {
    int32_t leftTravelDistance = this->_leftMotorPtr->getDistanceTraveled();
    int32_t rightTravelDistance = this->_rightMotorPtr->getDistanceTraveled();

    int32_t averageTravelDistance =
        (leftTravelDistance + rightTravelDistance) / 2;

    return averageTravelDistance;
}
