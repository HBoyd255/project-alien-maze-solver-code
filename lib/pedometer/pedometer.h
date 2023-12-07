

#include "motor.h"

class Pedometer {
   public:
    Pedometer(Motor* leftMotorPtr, Motor* rightMotorPtr);

    int32_t getDifference();

    void normalizeAngle(int16_t* anglePrt);
    int16_t calculateAngle();

   private:
    Motor* leftMotorPtr;
    Motor* rightMotorPtr;
};
