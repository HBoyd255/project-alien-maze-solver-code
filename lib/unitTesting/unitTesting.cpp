

#include "unitTesting.h"

#include "angleAndPosition.h"
#include "errorIndicator.h"

#define PRINT_TEST_RESULTS false

bool evaluateDistanceTest(int testNumber, float expected, float calculated) {
    bool currentTestFailed = (expected != calculated);

    if (PRINT_TEST_RESULTS) {
        if (currentTestFailed) {
            Serial.print("Test number ");
            Serial.print(testNumber);
            Serial.print(" failed,  expected: ");
            Serial.print(expected);
            Serial.print(" calculated: ");
            Serial.println(calculated);
        } else {
            Serial.print("Test number ");
            Serial.print(testNumber);
            Serial.println(" passed.");
        }
    }
    return currentTestFailed;
}

int positionDistanceTests() {
    // Test cases to implement.
    //  both positive, all 4 orth direction
    //  one positive, one negative, all 4 orth direction.
    //  Both neg
    //  45 degrees in each direction.

    int distanceTestNumber = 0;
    int failedDistanceTestCount = 0;

    Position testPositionA;
    Position testPositionB;
    float expectedDistance;
    float calculatedDistance;

    // Test 0 and 1
    // A at origin, B at x = 100, y = 0.
    // From A to B,
    // Form B to A.
    // Expected distance: 100mm

    testPositionA = Position(0, 0);
    testPositionB = Position(100, 0);
    expectedDistance = 100;

    calculatedDistance = testPositionA.distanceTo(testPositionB);
    failedDistanceTestCount += evaluateDistanceTest(
        distanceTestNumber, expectedDistance, calculatedDistance);
    distanceTestNumber++;

    calculatedDistance = testPositionB.distanceTo(testPositionA);
    failedDistanceTestCount += evaluateDistanceTest(
        distanceTestNumber, expectedDistance, calculatedDistance);
    distanceTestNumber++;

    // Test 2 and 3
    // A at origin, B at x = 0, y = 100.
    // From A to B,
    // Form B to A.
    // Expected distance: 100mm

    testPositionA = Position(0, 0);
    testPositionB = Position(0, 100);
    expectedDistance = 100;

    calculatedDistance = testPositionA.distanceTo(testPositionB);
    failedDistanceTestCount += evaluateDistanceTest(
        distanceTestNumber, expectedDistance, calculatedDistance);
    distanceTestNumber++;

    calculatedDistance = testPositionB.distanceTo(testPositionA);
    failedDistanceTestCount += evaluateDistanceTest(
        distanceTestNumber, expectedDistance, calculatedDistance);
    distanceTestNumber++;

    // Test 4 and 5
    // A at origin, B at x = 100, y = 100.
    // From A to B,
    // Form B to A.
    // Expected distance: 141mm

    testPositionA = Position(0, 0);
    testPositionB = Position(100, 100);
    expectedDistance = 141;

    calculatedDistance = testPositionA.distanceTo(testPositionB);
    failedDistanceTestCount += evaluateDistanceTest(
        distanceTestNumber, expectedDistance, calculatedDistance);
    distanceTestNumber++;

    calculatedDistance = testPositionB.distanceTo(testPositionA);
    failedDistanceTestCount += evaluateDistanceTest(
        distanceTestNumber, expectedDistance, calculatedDistance);
    distanceTestNumber++;

    return failedDistanceTestCount;
}

int positionStructUnitTests() {
    if (PRINT_TEST_RESULTS) {
        Serial.println();
        Serial.println("Position Tests");
    }

    int failedPositionTestCount = 0;

    failedPositionTestCount += positionDistanceTests();

    if (PRINT_TEST_RESULTS) {
        Serial.print("Position Tests Concluded. ");
        Serial.print(failedPositionTestCount);
        Serial.print(" test failed.");
        Serial.println();
    }

    return failedPositionTestCount;
}

int runAllTests(bool crashUponFail) {
    int failedTestCount = 0;

    failedTestCount += positionStructUnitTests();

    return failedTestCount;
}
