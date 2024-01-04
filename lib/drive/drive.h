#ifndef DRIVE_H
#define DRIVE_H

// Forward declaration.
class Motor;

class Drive {
   public:
    Drive(Motor* leftMotorPtr, Motor* rightMotorPtr);

    void setVelocity(int velocity);
    void setRotationalVelocity(int rotationalVelocity);

    void forwards();
    void back();
    void left();
    void right();
    void stop();

   private:
    Motor* _leftMotorPtr;
    Motor* _rightMotorPtr;

    int _minSpeed = 40;
};

#endif  // DRIVE_H