#ifndef MOTION_TRACKER_H
#define MOTION_TRACKER_H

#include "angleAndPosition.h"
#include "infrared.h"
#include "motor.h"

class MotionTracker {
   public:
    MotionTracker(Motor* leftMotorPtr, Motor* rightMotorPtr,
                  Infrared* frontLeftInfraredPtr,
                  Infrared* frontRightInfraredPtr);

    Angle angleFromOdometry();
    Angle angleFromFrontIR();

    void updateAngle();
    void updatePosition();

    void poll();

    Pose getPose();

   private:
    Motor* _leftMotorPtr;
    Motor* _rightMotorPtr;
    Infrared* _frontLeftInfraredPtr;
    Infrared* _frontRightInfraredPtr;

    Position _currentPosition;
    Angle _currentAngle = 0;

    int32_t _averageTravelDistance();
};

#endif  // MOTION_TRACKER_H