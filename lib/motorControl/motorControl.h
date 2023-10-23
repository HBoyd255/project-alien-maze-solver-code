#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

typedef enum { forward,
               right,
               backwards,
               left } DriveDirection;

typedef enum { antiClockwise,
               clockwise } MotorDirection;

typedef enum { left_motor,
               right_motor } Motors;

void setupMotorControl();

void setDriveDirection(DriveDirection dir);
void setDriveSpeed(uint8_t speed);
void setMotorDirection(Motors motor, MotorDirection dir);
void setMotorSpeed(Motors motor, uint8_t speed);


#endif  // MOTOR_CONTROL_H