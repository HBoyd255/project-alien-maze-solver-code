#ifndef MOTORS_H
#define MOTORS_H

#define LEFT_MOTOR_DIRECTION_PIN A0
#define RIGHT_MOTOR_DIRECTION_PIN A1

#define LEFT_MOTOR_SPEED_PIN D9
#define RIGHT_MOTOR_SPEED_PIN D10

enum Motors { left_motor, right_motor };

/**
 * @brief Initializes the motors and sets up the necessary pins and
 * configurations.
 *
 * This function should be called once during the setup phase of the program.
 * It initializes the motors and sets up the necessary pins and configurations
 * for the motor control to work properly.
 */
void setupMotors();

// TODO write documentation
// Takes a motor and a value from -100 to 100
int setMotorSpeed(Motors motor, int formatted_speed);

#endif  // MOTORS_H
