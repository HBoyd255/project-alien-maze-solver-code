


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

   private:
    Motor* _leftMotorPtr;
    Motor* _rightMotorPtr;
    Infrared* _frontLeftInfraredPtr;
    Infrared* _frontRightInfraredPtr;
};
