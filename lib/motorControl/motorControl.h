#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

typedef enum { left_motor,
               right_motor } Motors;



/**
 * @brief Initializes the motors and sets up the necessary pins and configurations.
 * 
 * This function should be called once during the setup phase of the program.
 * It initializes the motors and sets up the necessary pins and configurations
 * for the motor control to work properly.
 */
void setupMotors();

//TODO write documentation
//Takes a motor and a value from -100 to 100
int setMotorSpeed(Motors motor, int formatted_speed);

#endif  // MOTOR_CONTROL_H
