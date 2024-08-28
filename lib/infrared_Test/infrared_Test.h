/**
 * @file infrared_Test.h
 * @brief Tests for the Infrared class.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-08-28
 * @copyright Copyright (c) 2024
 */
#ifndef INFRARED_TEST_H
#define INFRARED_TEST_H

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
                   Infrared* frontRightInfrared, Infrared* rightInfrared);

#endif  // INFRARED_H