/**
 * @file infrared_Test.h
 * @brief Tests for the Infrared class.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-08-28
 * @copyright Copyright (c) 2024
 */

#include "infrared.h"

/**
 * @brief Test the Infrared class by reading from all sensors and printing the
 * results to the serial monitor.
 *
 * @param leftInfrared The left infrared sensor.
 * @param frontLeftInfrared The front left infrared sensor.
 * @param frontRightInfrared The front right infrared sensor.
 * @param rightInfrared The right infrared sensor.
 */
void infrared_test(Infrared* leftInfrared, Infrared* frontLeftInfrared,
                   Infrared* frontRightInfrared, Infrared* rightInfrared) {
        int leftSensorReading = leftInfrared->read();
    int frontLeftSensorReading = frontLeftInfrared->read();
    int frontRightSensorReading = frontRightInfrared->read();
    int rightSensorReading = rightInfrared->read();

    Serial.print("IR sensors:");
    Serial.print(" Left :");
    Serial.print(leftSensorReading);
    Serial.print(" Front Left :");
    Serial.print(frontLeftSensorReading);
    Serial.print(" Front Right :");
    Serial.print(frontRightSensorReading);
    Serial.print(" Right :");
    Serial.println(rightSensorReading);
}