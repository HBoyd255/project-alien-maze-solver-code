
// https://www.arduino.cc/reference/en/
#include <Arduino.h>

#include "angleAndPosition.h"
#include "binary.h"
#include "bluetoothLowEnergy.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "grid.h"
#include "history.h"
#include "infrared.h"
#include "motionTracker.h"
#include "motor.h"
#include "obstacles.h"
#include "pixels.h"
#include "schedule.h"
#include "systemInfo.h"
#include "ultrasonic.h"
#include "vectorUpgrades.h"

ErrorIndicator errorIndicator(LED_BUILTIN, SERIAL_BAUD_RATE);

Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_SPEED_PIN,
                LEFT_MOTOR_ENCODER_A_PIN, LEFT_MOTOR_ENCODER_B_PIN,
                LEFT_MOTOR_ROTATION_INVERTED);

Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_SPEED_PIN,
                 RIGHT_MOTOR_ENCODER_A_PIN, RIGHT_MOTOR_ENCODER_B_PIN,
                 RIGHT_MOTOR_ROTATION_INVERTED);

Drive drive(&leftMotor, &rightMotor);

Pixels pixels(PIXELS_DATA_PIN, LED_COUNT, LED_ROTATION_OFFSET);
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO,
                      ULTRASONIC_TIMEOUT_MICROSECONDS, ULTRASONIC_MAX_DISTANCE,
                      ULTRASONIC_DATA_SHELF_LIFE);

// TODO Change this class so that 639 is not passed in, as 639mm is not a user
// preference, it is a hardware limitation.
Infrared leftInfrared(&errorIndicator, LEFT_INFRARED_INDEX, 639);
Infrared rightInfrared(&errorIndicator, RIGHT_INFRARED_INDEX, 639);
Infrared frontLeftInfrared(&errorIndicator, FRONT_LEFT_INFRARED_INDEX, 639);
Infrared frontRightInfrared(&errorIndicator, FRONT_RIGHT_INFRARED_INDEX, 639);

Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN,
              BUMPER_ROTATION_OFFSET);

BluetoothLowEnergy bluetoothLowEnergy(&errorIndicator, MAIN_SERVICE_UUID,
                                      OBSTACLE_POSITION_UUID, ROBOT_POSE_UUID);

MotionTracker motionTracker(&leftMotor, &rightMotor, &frontLeftInfrared,
                            &frontRightInfrared);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    errorIndicator.assignPixels(&pixels);
    errorIndicator.assignDrive(&drive);
    errorIndicator.setup();

    leftMotor.setup([]() { leftMotor.isr(); });
    rightMotor.setup([]() { rightMotor.isr(); });

    pixels.setup();

    leftInfrared.setup([]() { leftInfrared.routineFunction(); });
    rightInfrared.setup([]() { rightInfrared.routineFunction(); });
    frontLeftInfrared.setup([]() { frontLeftInfrared.routineFunction(); });
    frontRightInfrared.setup([]() { frontRightInfrared.routineFunction(); });

    ultrasonic.setup([]() { ultrasonic.isr(); });

    bumper.setup([]() { bumper.isr(); });

    bluetoothLowEnergy.setup(BLE_DEVICE_NAME, BLE_MAC_ADDRESS);
}

ObstacleDetector usonicOD(&motionTracker, &ultrasonic, {{0, 85}, 90});

ObstacleDetector FLIROD(&motionTracker, &frontLeftInfrared, {{-36, 64}, 90});
ObstacleDetector FRIROD(&motionTracker, &frontRightInfrared, {{36, 64}, 90});
ObstacleDetector LIROD(&motionTracker, &leftInfrared, {{-85, 0}, 180});
ObstacleDetector RIROD(&motionTracker, &rightInfrared, {{85, 0}, 0});

ObstacleDetector bumperOD(&motionTracker, &bumper, {{0, 125}, 90});

void polls() {
    frontLeftInfrared.poll();
    frontRightInfrared.poll();
    leftInfrared.poll();
    rightInfrared.poll();

    ultrasonic.poll();

    motionTracker.poll();

    bluetoothLowEnergy.poll();
}

Grid ObstacleGrid;

void loop() {
    polls();

    Serial.println("Start");
    Serial.println();

    Obstacle testObstacle = {{31, 0}, 3};

    ObstacleGrid.updateObstacle(testObstacle);

    Obstacle testObstacle2 = {{33, 0}, 2};

    ObstacleGrid.updateObstacle(testObstacle2);

    Serial.println("9");
    ObstacleGrid.print({10, 10}, 9);

    Serial.println("End");

    //
    //     ObstacleVector obstacles;
    //
    //     FLIROD.addObstaclesToVector(&obstacles);
    //     FRIROD.addObstaclesToVector(&obstacles);
    //     LIROD.addObstaclesToVector(&obstacles);
    //     RIROD.addObstaclesToVector(&obstacles);
    //
    //     usonicOD.addObstaclesToVector(&obstacles);
    //
    //     bumperOD.addObstaclesToVector(&obstacles);
    //
    //     for (const Obstacle obstacle : obstacles) {
    //         Serial.print(obstacle);
    //         Serial.print(",");
    //
    //         bluetoothLowEnergy.sendObstacle(obstacle);
    //     }
    //     Serial.println();
    //
    //     bluetoothLowEnergy.sendRobotPose(motionTracker.getPose());

    //
    //     static Pose UsonicPose = {{0, 85}, 90};
    //     Pose robotPose = motionTracker.getPose();
    //
    //     int16_t distance = ultrasonic.read();
    //
    //     if (distance > 0) {
    //         Position positionToReturn;
    //
    //         // Set the position of the detected obstacle relative to the
    //         sensor. positionToReturn.y = distance;
    //
    //         // Transform the position from being relative to the sensor, to
    //         being
    //         // relative to the robot.
    //         positionToReturn.transformByPose(UsonicPose);
    //
    //         // Transform the position from being relative to the robot, to
    //         being
    //         // relative to the global coordinate system.
    //         positionToReturn.transformByPose(robotPose);
    //
    //         bluetoothLowEnergy.sendObstaclePosition(positionToReturn, 2);
    //
    //         Serial.print(" Obstacle:");
    //         Serial.print(positionToReturn);
    //         Serial.print(" Robot:");
    //         Serial.println(robotPose);
    //     }
    //
    //     // Serial.println(motionTracker.getPose());
    //
    //     bluetoothLowEnergy.sendRobotPose(motionTracker.getPose());
    //
    // delay(10);
}