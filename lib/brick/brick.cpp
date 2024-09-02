/**
 * @file brick.cpp
 * @brief Definitions for the Brick struct and the BrickList Class.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-01-24
 * @copyright Copyright (c) 2024
 */

#include "brick.h"

#include "comparison.h"
#include "errorIndicator.h"
#include "map.h"
#include "mazeConstants.h"

// TODO read this value in.
#define ROBOT_RADIUS 120

/**
 * @brief Construct a new Brick object
 *
 * @param brickPosition The position of the Brick.
 * @param isVertical The rotation of the Brick, horizontal by default.
 */
Brick::Brick(Position brickPosition, bool isVertical)
    : position(brickPosition),
      isVertical(isVertical),
      length(BRICK_LENGTH),
      width(BRICK_WIDTH) {}

/**
 * @brief Get the position of the Brick struct's bottom left corner.
 *
 * @return (Position) The Brick struct's bottom left corner.
 */
Position Brick::getBottomLeft() {
    Position positionToReturn = this->position;

    if (this->isVertical) {
        positionToReturn.x -= (this->width / 2);
        positionToReturn.y -= (this->length / 2);
    } else {
        positionToReturn.x -= (this->length / 2);
        positionToReturn.y -= (this->width / 2);
    }
    return positionToReturn;
}

/**
 * @brief Get the position of the Brick struct's top right corner.
 *
 * @return (Position) The Brick struct's top right corner.
 */
Position Brick::getTopRight() {
    Position positionToReturn = this->position;

    if (this->isVertical) {
        positionToReturn.x += (this->width / 2);
        positionToReturn.y += (this->length / 2);
    } else {
        positionToReturn.x += (this->length / 2);
        positionToReturn.y += (this->width / 2);
    }
    return positionToReturn;
}

/**
 * @brief Calculates the zone that a given position is in, relative to the
 * current Brick.
 *
 * @param target the position to test.
 * @return (Zone) The zone of the position, relative to the current Brick.
 */
Zone Brick::calculateZone(Position target) {
    bool onLeft = target.x < this->getBottomLeft().x;
    bool onRight = target.x > this->getTopRight().x;

    bool onBottom = target.y < this->getBottomLeft().y;
    bool onTop = target.y > this->getTopRight().y;

    if (onBottom && onLeft) {
        return BottomLeftZone;
    } else if (onBottom && onRight) {
        return BottomRightZone;
    } else if (onTop && onLeft) {
        return TopLeftZone;
    } else if (onTop && onRight) {
        return TopRightZone;
    } else if (onBottom) {
        return BottomZone;
    } else if (onLeft) {
        return LeftZone;
    } else if (onTop) {
        return TopZone;
    } else if (onRight) {
        return RightZone;
    } else {
        return CentreZone;
    }
}

/**
 * @brief Calculates the square of the euclidean distance between the edge
 * of this Brick and a provided target position.
 *
 * This function uses less computation than distanceTo(), as no
 * square root required. For basic comparisons between two distances,
 * this function is more efficient.
 *
 * c * c == a * a + b * b
 * is a lot easier to compute than
 * c = (a * a + b * b) ^ 0.5
 *
 * @param target The other position to calculate the squared distance
 * @param zone_IP The pointer to return the zone from the Brick.
 * @param dx_P The pointer to return the difference in x positions.
 * @param dy_P The pointer to return the difference in y positions.
 * @return (float) The square of the euclidean distance to the target
 * position from the edge of this Brick, in millimeters.
 */
float Brick::squaredDistanceTo(Position target, int* zone_IP, int* dx_P,
                               int* dy_P) {
    Zone targetPositionZone = this->calculateZone(target);

    *zone_IP = (int)targetPositionZone;

    if (targetPositionZone == CentreZone) {
        return 0;
    }

    // The position to measure from,
    //  this will be a position that lies on the edge or corner of the brick.
    Position measuringPosition;

    switch (targetPositionZone) {
        case CentreZone:
            measuringPosition.x = target.x;
            measuringPosition.y = target.y;
            break;
        case RightZone:
            measuringPosition.x = this->getTopRight().x;
            measuringPosition.y = target.y;
            break;
        case TopRightZone:
            measuringPosition.x = this->getTopRight().x;
            measuringPosition.y = this->getTopRight().y;
            break;
        case TopZone:
            measuringPosition.x = target.x;
            measuringPosition.y = this->getTopRight().y;
            break;
        case TopLeftZone:
            measuringPosition.x = this->getBottomLeft().x;
            measuringPosition.y = this->getTopRight().y;
            break;
        case LeftZone:
            measuringPosition.x = this->getBottomLeft().x;
            measuringPosition.y = target.y;
            break;
        case BottomLeftZone:
            measuringPosition.x = this->getBottomLeft().x;
            measuringPosition.y = this->getBottomLeft().y;
            break;
        case BottomZone:
            measuringPosition.x = target.x;
            measuringPosition.y = this->getBottomLeft().y;
            break;
        case BottomRightZone:
            measuringPosition.x = this->getTopRight().x;
            measuringPosition.y = this->getBottomLeft().y;
            break;
    }

    return measuringPosition.squaredDistanceTo(target, dx_P, dy_P);
}

/**
 * @brief Calculates the euclidean distance between the edge of this Brick
 * and a provided target position using the pythagorean theorem.
 *
 * @param target The other position to calculate the distance to.
 * @return (float) The euclidean distance to the target position in
 * millimeters.
 */
float Brick::distanceTo(Position target) {
    // a, b and cSquared are from the Pythagorean theorem.
    // A^2 + B^2 = C^2,
    // If A == 0, C = B
    // If B == 0, C = A

    int a = -1;  // -1 represents an uninitialized state.
    int b = -1;
    int c;

    // calculate the squared distance from this brick to the target, and get the
    // x and y difference by reference.
    int cSquared = this->squaredDistanceTo(target, &a, &b);

    // A^2 + B^2 = C^2,

    // If a is 0,(and b is initialized) c is equal to b.
    if ((a == 0) && (b != -1)) {
        c = b;
    }
    // If b is 0, c is equal to a.
    else if ((b == 0) && (a != -1)) {
        c = a;
    }
    // If a and b are both not zero, calculate c
    else {
        c = sqrt(cSquared);
    }

    return c;
}

/**
 * @brief Converts the current Brick into a human readable string.
 *
 * @return (String) The current Brick as a human readable string.
 */
String Brick::toString() {
    String stringToReturn;
    stringToReturn += "Brick[";
    stringToReturn += this->position;
    stringToReturn += ",";

    if (this->isVertical) {
        stringToReturn += "Vertical";
    } else {
        stringToReturn += "Horizontal";
    }

    stringToReturn += "]";
    return stringToReturn;
}

/**
 * @brief Construct a new BrickList object, and appends the 4 starting walls.
 */
BrickList::BrickList() { this->_addWalls(); };

/**
 * @brief Get the number of Brick structs in the list.
 *
 * @return (int) The number of Brick structs in the list.
 */
int BrickList::getBrickCount() { return this->_brickCount; }

/**
 * @brief Get the Brick at a given index in the list.
 *
 * @param index The index of the Brick to return.
 * @return (Brick) The Brick at the given index in the list.
 */
Brick BrickList::getBrick(int index) {
    // If the requested brick is not in the list
    if (index > this->getBrickCount()) {
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__,
                                       "Brick out of range");
    }
    // If the requested brick is in the list, return it
    return this->_brickArray[index];
}

/**
 * @brief Converts the BrickList into a human readable string.
 *
 * @return (String) The BrickList as a human readable string.
 */
String BrickList::toString() {
    int brickCount = this->getBrickCount();

    String stringToReturn = "BrickList[";
    stringToReturn += String(brickCount);
    stringToReturn += " bricks,{";

    for (int i = 0; i < brickCount; i++) {
        stringToReturn += this->getBrick(i).toString();

        if (i != (brickCount - 1)) {
            stringToReturn += ", ";
        }
    }

    stringToReturn += "}]";

    return stringToReturn;
}

/**
 * @brief Takes the position of a corner seen by the robot, and decided if
 * it is along one of the walls of the maze.
 *
 * @param brickCorner The corner position seen by the robot,
 * @return (true) If a Brick is added to the list.
 * @return (false) If a Brick is not added to the list.
 */
bool BrickList::handleBrickFromWallPosition(Position brickOuterCorner) {
    // How close, in millimeters, a corner needs to be to the wall, to be
    // considered to be flat against the wall.
    const int wallTolerance = 150;

    bool onBottomWall = within(brickOuterCorner.y, 0, wallTolerance);
    bool onLeftWall = within(brickOuterCorner.x, 0, wallTolerance);
    bool onTopWall = within(brickOuterCorner.y, MAZE_LENGTH, wallTolerance);
    bool onRightWall = within(brickOuterCorner.y, MAZE_WIDTH, wallTolerance);

    bool onAWall = onBottomWall || onLeftWall || onTopWall || onRightWall;

    // If the brick that has been seen is not on an outer wall,
    if (!onAWall) {
        // return false to indicate that no brick have been added to the list.
        return false;
    }

    Brick brickToAdd;
    brickToAdd.isVertical = onLeftWall || onRightWall;
    if (onBottomWall) {
        brickToAdd.position.x = brickOuterCorner.x - (BRICK_LENGTH / 2);
        brickToAdd.position.y = (BRICK_WIDTH / 2) + 5;
    } else if (onLeftWall) {
        brickToAdd.position.x = (BRICK_WIDTH / 2) + 5;
        brickToAdd.position.y = brickOuterCorner.y + (BRICK_LENGTH / 2);
    } else if (onTopWall) {
        brickToAdd.position.x = brickOuterCorner.x + (BRICK_LENGTH / 2);
        brickToAdd.position.y = MAZE_LENGTH - (BRICK_WIDTH / 2) - 5;
    } else if (onRightWall) {
        brickToAdd.position.x = MAZE_WIDTH - (BRICK_WIDTH / 2) - 5;
        brickToAdd.position.y = brickOuterCorner.y - (BRICK_LENGTH / 2);
    }

    // Return true if a brick gets added to the list, and false if not.
    return this->_attemptAppendBrick(brickToAdd);
}

/**
 * @brief Takes a line, provided as a start and end position, and calculates
 * if a Brick can be approximated from it.
 *
 * @param robotPosition The current position of the robot.
 * @param statingCorner The position that the seen line starts.
 * @param lineEnd  The position that the seen line ends.
 * @return (true) If a Brick has been added to the list.
 * @return (false) If a Brick has been not been added to the list.
 */
bool BrickList::handleBrickFromLine(Position robotPosition,
                                    Position statingCorner,
                                    Position EndingCorner) {
    float xLength = abs(statingCorner.x - EndingCorner.x);
    float yLength = abs(statingCorner.y - EndingCorner.y);

    float adjacent = max(xLength, yLength);
    float opposite = min(xLength, yLength);

    // The tolerance for an line  to be considered straight is 9°
    // for the angle to be less than 9°, the adjacent side of the triangle must
    // be more than 6 times greater than the opposite.
    if (adjacent < (opposite * 6)) {
        // If the line is more than 9 degrees off, return false.
        return false;
    }

    float lineLength = adjacent;
    bool lineIsVertical = (yLength > xLength);

    Position lineCentrePosition;
    lineCentrePosition.x = (statingCorner.x + EndingCorner.x) / 2;
    lineCentrePosition.y = (statingCorner.y + EndingCorner.y) / 2;

    int lengthTolerance = 40;

    bool robotIsLeftOfLine = (robotPosition.x < lineCentrePosition.x);
    bool robotIsBelowLine = (robotPosition.y < lineCentrePosition.y);

    bool seenShortSide = within(lineLength, BRICK_WIDTH, lengthTolerance);
    bool seenLongSide = within(lineLength, BRICK_LENGTH, lengthTolerance);

    // check if the line see was withing tolerance of ther short of long side of
    // a brick.
    if (!seenShortSide && !seenLongSide) {
        return 0;
    }

    bool newBrickIsVertical = (seenShortSide != lineIsVertical);

    // This could be improved with trigonometry, but
    // is perfectly functional and readable as is, so
    // doesn't need refactoring at this stage.

    Brick brickToAdd;
    brickToAdd.isVertical = newBrickIsVertical;

    int centreOffset;

    if (seenShortSide) {
        centreOffset = BRICK_LENGTH / 2;
    } else {
        centreOffset = BRICK_WIDTH / 2;
    }

    if (lineIsVertical) {
        brickToAdd.position.y = lineCentrePosition.y;

        if (robotIsLeftOfLine) {
            brickToAdd.position.x = lineCentrePosition.x + centreOffset;
        } else {
            brickToAdd.position.x = lineCentrePosition.x - centreOffset;
        }
    } else {
        brickToAdd.position.x = lineCentrePosition.x;
        if (robotIsBelowLine) {
            brickToAdd.position.y = lineCentrePosition.y + seenShortSide;
        } else {
            brickToAdd.position.y = lineCentrePosition.y - seenShortSide;
        }
    }

    // Attempt to append the new brick to the list, and return its success.
    return this->_attemptAppendBrick(brickToAdd);
}

/**
 * @brief Takes a sensor reading from a position, and compared it to the
 * curent Map data and BrickList, and if applicable adds a new Brick to
 * the list.
 *
 * @param robotPosition The current position on the robot.
 * @param angleOfSensor The direction that the sensor that captured the data
 * is pointing.
 * @param measuredDistance The distance read by the sensor.
 * @param tolerance The maximum offset that the sensor can be from an
 * orthogonal direction, before readings start getting rejected;
 * @param map_P A pointer to the map data.
 * @return (-1) If the robot's position needs recalibrating.
 * @return (0) If no changes have been made.
 * @return (1) If a Brick has been added to the list.
 */
int BrickList::handleBrickFromSensorAndMap(Position robotPosition,
                                           Angle angleOfSensor,
                                           int measuredDistance, int tolerance,
                                           Map* map_P) {
    if (!angleOfSensor.isOrthogonal(tolerance)) {
        // This function only works with orthogonal angles
        return 0;
    }

    if (measuredDistance > 400) {
        return 0;
    }

    bool pointingDown = angleOfSensor.isPointingDown();
    bool pointingLeft = angleOfSensor.isPointingLeft();
    bool pointingUp = angleOfSensor.isPointingUp();
    bool pointingRight = angleOfSensor.isPointingRight();

    Position brickEdgePosition;

    // This could be improved with trigonometry down the line, but for now
    // this works, as it is supposed to assume that the angles are perfectly
    // orthogonal.
    if (pointingDown) {
        brickEdgePosition = {0, -(float)measuredDistance};
    } else if (pointingLeft) {
        brickEdgePosition = {-(float)measuredDistance, 0};
    } else if (pointingUp) {
        brickEdgePosition = {0, (float)measuredDistance};
    } else if (pointingRight) {
        brickEdgePosition = {(float)measuredDistance, 0};
    }
    brickEdgePosition += robotPosition;

    map_P->incrementSeenPosition(brickEdgePosition);

    int comparison =
        this->_compare(robotPosition, angleOfSensor, measuredDistance);

    // Check if the current bricks in the list already satisfy the data
    // obtained from the sensors.
    if (comparison == 0) {
        return 0;
    }

    // Check if the robot needs recalibrating.
    else if (comparison == -1) {
        // return -1 to tell to the calling function to recalibrate the
        // robot.
        return -1;
    }

    // These variables could do with better names,
    // RC stands for Relative corner, BL and TR stand for Bottom left and
    // Top right, and Vert and Hor stand for Vertical and horizontal.

    // these two pairs of coordinates plot of the corners of where a brick
    // would be palaced if it was either horizontal or vertical.

    // In this context, Bottom left and Top right, Vertical and horizontal
    // are all relative to the sensor that has seen the brick.

    Position RC_BL_Vert;
    Position RC_TR_Vert;

    Position RC_BL_Hor;
    Position RC_TR_Hor;

    // This chunk could also be improved using trig.
    if (pointingDown) {
        RC_BL_Vert.x = brickEdgePosition.x + BRICK_WIDTH / 2;
        RC_BL_Vert.y = brickEdgePosition.y;
        RC_TR_Vert.x = brickEdgePosition.x - BRICK_WIDTH / 2;
        RC_TR_Vert.y = brickEdgePosition.y - BRICK_LENGTH;

        RC_BL_Hor.x = brickEdgePosition.x + BRICK_LENGTH / 2;
        RC_BL_Hor.y = brickEdgePosition.y;
        RC_TR_Hor.x = brickEdgePosition.x - BRICK_LENGTH / 2;
        RC_TR_Hor.y = brickEdgePosition.y - BRICK_WIDTH;
    }

    if (pointingLeft) {
        RC_BL_Vert.x = brickEdgePosition.x;
        RC_BL_Vert.y = brickEdgePosition.y - BRICK_WIDTH / 2;
        RC_TR_Vert.x = brickEdgePosition.x - BRICK_LENGTH;
        RC_TR_Vert.y = brickEdgePosition.y + BRICK_WIDTH / 2;

        RC_BL_Hor.x = brickEdgePosition.x;
        RC_BL_Hor.y = brickEdgePosition.y - BRICK_LENGTH / 2;
        RC_TR_Hor.x = brickEdgePosition.x - BRICK_WIDTH;
        RC_TR_Hor.y = brickEdgePosition.y + BRICK_LENGTH / 2;
    }
    if (pointingUp) {
        RC_BL_Vert.x = brickEdgePosition.x - BRICK_WIDTH / 2;
        RC_BL_Vert.y = brickEdgePosition.y;
        RC_TR_Vert.x = brickEdgePosition.x + BRICK_WIDTH / 2;
        RC_TR_Vert.y = brickEdgePosition.y + BRICK_LENGTH;

        RC_BL_Hor.x = brickEdgePosition.x - BRICK_LENGTH / 2;
        RC_BL_Hor.y = brickEdgePosition.y;
        RC_TR_Hor.x = brickEdgePosition.x + BRICK_LENGTH / 2;
        RC_TR_Hor.y = brickEdgePosition.y + BRICK_WIDTH;
    }
    if (pointingRight) {
        RC_BL_Vert.x = brickEdgePosition.x;
        RC_BL_Vert.y = brickEdgePosition.y + BRICK_WIDTH / 2;
        RC_TR_Vert.x = brickEdgePosition.x + BRICK_LENGTH;
        RC_TR_Vert.y = brickEdgePosition.y - BRICK_WIDTH / 2;

        RC_BL_Hor.x = brickEdgePosition.x;
        RC_BL_Hor.y = brickEdgePosition.y + BRICK_LENGTH / 2;
        RC_TR_Hor.x = brickEdgePosition.x + BRICK_WIDTH;
        RC_TR_Hor.y = brickEdgePosition.y - BRICK_LENGTH / 2;
    }

    // Check if each the vertical and horizontal brick could fit on the map.
    int verticalViability = map_P->checkIfSafeForBrick(RC_BL_Vert, RC_TR_Vert);
    int horizontalViability = map_P->checkIfSafeForBrick(RC_BL_Hor, RC_TR_Hor);

    // If the vertical brick will not fit(but is not out of bounds) and the
    // horizontal brick will fit, place a horizontal brick in the list.
    if ((verticalViability == 0) && (horizontalViability == 1)) {
        // Creates a brick based on its edge position and the angle of the
        // sensor.
        Brick brickToAdd =
            this->_getBrickFromEdge(brickEdgePosition, angleOfSensor);

        // return 1 if a new brick is placed, 0 if it is not.
        return this->_attemptAppendBrick(brickToAdd);
    }

    // return 0 to indicate that no changes have been made to the list.
    return 0;
}

/**
 * @brief Returns the distance to the closest Brick in the list, from a
 * given target position.
 *
 * Also returns via reference, the index of this closest Brick, and the zone
 * from this closest Brick.
 *
 * @param target The position to calculate the distance to,
 * @param indexOfClosestBrick_P The pointer used to return the index of the
 * closest Brick.
 * @param zoneFromClosestBrick_P The pointer used to return the zone from
 * the closest Brick.
 * @return (int) The distance to the closest Brick.
 */
int BrickList::lowestDistance(Position testPosition, int* indexOfClosestBrick_P,
                              int* zoneFromClosestBrick_P) {
    // Local placeholders for the values to be returned via reference to
    // indexOfClosestBrick_P and zoneFromClosestBrick_P.
    int indexOfClosestBrick_L = -1;
    int zoneFromClosestBrick_L = -1;
    int lowestSquaredDistance = INT32_MAX;

    // iterate through the bricks in the list,
    for (int i = 0; i < this->getBrickCount(); i++) {
        Brick brick = this->getBrick(i);

        // The "zone from" refers to which of the 9 zones the target position is
        // in relative to the brick.
        // For example, given a test position and test brick, if the brick
        // is to the top right of the the test position, then the test position
        // is in the bricks bottom left zone.
        int zoneFromCurrentBrick = -1;

        // Get the squared distance to the current brick.
        int squaredDistanceToBrick =
            brick.squaredDistanceTo(testPosition, &zoneFromCurrentBrick);

        // If the current brick is the closest sofar.
        if (squaredDistanceToBrick < lowestSquaredDistance) {
            lowestSquaredDistance = squaredDistanceToBrick;
            // keep track of its index
            indexOfClosestBrick_L = i;
            // and its zone.
            zoneFromClosestBrick_L = zoneFromCurrentBrick;
        }
    }

    // if indexOfClosestBrick_L was set, return it via reference.
    if (indexOfClosestBrick_L != -1) {
        *indexOfClosestBrick_P = indexOfClosestBrick_L;
    }

    // if zoneFromClosestBrick_L was set, return it via reference.
    if (zoneFromClosestBrick_L != -1) {
        *zoneFromClosestBrick_P = zoneFromClosestBrick_L;
    }

    // TODO could make this run better using a similar method to the one used in
    // Position::distanceTo()
    return sqrt(lowestSquaredDistance);
}

#if DEBUG_ALLOW_PREFILLED_MAZE
/**
 * @brief populates the Brick list with a set of hard coded Brick structs
 * that represent the data in the provided "Seen Maze".
 *
 * This function should only be used for algorithm testing,
 */
void BrickList::setPreprogrammedMazeData() {
    // This is the hard coded data for the "seen maze"
    // This data is only used for algorithm testing,
    // it is at no point used during the demonstration.

    this->_appendBrick(Brick(Position(125, 460), false));
    this->_appendBrick(Brick(Position(375, 460), false));
    this->_appendBrick(Brick(Position(40, 1000), true));
    this->_appendBrick(Brick(Position(125, 1460), false));
    this->_appendBrick(Brick(Position(375, 1460), false));
    this->_appendBrick(Brick(Position(540, 1545), true));
    this->_appendBrick(Brick(Position(1460, 1545), true));
    this->_appendBrick(Brick(Position(1295, 1460), false));
    this->_appendBrick(Brick(Position(1130, 1375), true));
    this->_appendBrick(Brick(Position(1460, 625), true));
    this->_appendBrick(Brick(Position(1295, 540), false));
    this->_appendBrick(Brick(Position(1130, 455), true));
    this->_appendBrick(Brick(Position(460, 1000), true));
    this->_appendBrick(Brick(Position(625, 915), false));
    this->_appendBrick(Brick(Position(875, 915), false));
}
#endif

/**
 * @brief Compares a sensors reading the expected reading, obtained from the
 * existing items in the BrickList.
 *
 * @param robotPose The current pose of the robot.
 * @param angleOfSensor The direction that the sensor that captured the data
 * is pointing.
 * @param measuredDistance The distance read by the sensor.
 * @return (1) If a Brick need placing.
 * @return (0) If no changes need making.
 * @return (-1) If the robot's position need recalibrating.
 */
int BrickList::_compare(Position robotPosition, Angle angleOfSensor,
                        int measuredDistance) {
    const int driftTolerance = 20;

    if (!angleOfSensor.isOrthogonal()) {
        // This function only works with orthogonal
        return 0;
    }

    int expectedDistance =
        this->_getOrthogonalBrickDistance(robotPosition, angleOfSensor);

    if (expectedDistance < 1) {
        return -2;
    }

    // If the measured reading is withing a tolerance of the expected reading,
    if (within(expectedDistance, measuredDistance, driftTolerance)) {
        // return 0 to indicate that no changes need to be made.
        return 0;
    }
    // If the expected reading is greater than the measured distance,
    else if (expectedDistance > measuredDistance) {
        // return 1 to indicate that a brick could be added to the list.
        return 1;
    }
    // If the expected reading is smaller than the measured distance,
    else {
        // return -1 to indicate that the robots position needs to be
        // recalibrated.
        return -1;
    }
}

/**
 * @brief Creates a Brick based on the position of its edge, and the angle
 * of the sensor that saw it.
 *
 * The Brick will be places so that the position seen (brickEdgePosition)
 * will be on the centre of its long side, and will extrude away from the
 * sensor.
 *
 * @param brickEdgePosition The position seen by the sensor.
 * @param angleOfSensor The angle that the sensor was facing when the Brick
 * was seen.
 * @return (Brick) The newly created Brick.
 */
Brick BrickList::_getBrickFromEdge(Position brickEdgePosition,
                                   Angle angleOfSensor) {
    bool pointingLeft = angleOfSensor.isPointingLeft();
    bool pointingRight = angleOfSensor.isPointingRight();

    bool isVertical = (pointingLeft || pointingRight);

    Brick newBrick(brickEdgePosition, isVertical);

    Position offset = Position(newBrick.width / 2, 0).rotate(angleOfSensor);

    newBrick.position.offset(offset);

    return newBrick;
}

/**
 * @brief Get the distance to the closest Brick in a chosen direction,
 * Only works with orthogonal directions.
 *
 * @param robotPosition The position of the robot.
 * @param directionOfBrick The direction to look in, will be rounded to the
 * nearest right angle.
 * @return (int) The distance to the Brick in the chosen direction.
 * Returns -1 if no Brick was seen.
 */
int BrickList::_getOrthogonalBrickDistance(Position robotPosition,
                                           Angle directionOfBrick) {
    // In its current state this function can only get the distance to
    // orthogonal bricks, the input angle is rounded to the closest right
    // angle.

    // The "zone from" refers to which of the 9 zones the target position is
    // in relative to the brick.
    // For example, given a test position and test brick, if the brick
    // is to the top right of the the test position, then the test position
    // is in the bricks bottom left zone.

    // So, Finding all the bricks directly below the robot, involves finding all
    // the bricks that place the robot in their upper zone.

    Zone targetZone;

    if (directionOfBrick.isPointingDown()) {
        targetZone = TopZone;
    }
    if (directionOfBrick.isPointingLeft()) {
        targetZone = RightZone;
    }
    if (directionOfBrick.isPointingUp()) {
        targetZone = BottomZone;
    }
    if (directionOfBrick.isPointingRight()) {
        targetZone = LeftZone;
    }

    const int startingValue = UINT16_MAX;
    int shortestDistance = startingValue;

    // Iterate through each brick in the list
    for (int i = 0; i < this->getBrickCount(); i++) {
        Brick brick = this->getBrick(i);

        Zone zoneFromCurrentBrick = brick.calculateZone(robotPosition);

        // If the zone relative to the current brick matches the predetermined
        // target zone, then check if its distance is shorter than the current
        // shortest distance.
        if (zoneFromCurrentBrick == targetZone) {
            if (brick.distanceTo(robotPosition) < shortestDistance) {
                shortestDistance = brick.distanceTo(robotPosition);
            }
        }
    }

    // If the lowest value retains its starting value, then no bricks where
    // found in the chosen direction.
    if (shortestDistance == startingValue) {
        // Return -1 to indicate a failure.
        return -1;
    }

    return shortestDistance;
}

/**
 * @brief Add a Brick to the end of the BrickList, if there is space
 * remaining.
 *
 * @param brickToAdd The Brick to add.
 * @return (true) If the Brick was added to the list.
 * @return (false) If the list was already full.
 */
bool BrickList::_appendBrick(Brick brickToAdd) {
    // If the list is already full,
    if (this->_brickCount >= MAX_BRICK_COUNT) {
        // return false.
        return false;
    }

    // Add the new brick to the list.
    this->_brickArray[this->_brickCount] = brickToAdd;
    this->_brickCount += 1;

    // Return true to indicate that a brick was added to the list.
    return true;
}

/**
 * @brief Adds a Brick the the end of the BrickList, if it does not collide
 * with an existing Brick in the list.
 *
 * @param brickToAdd  The Brick to add.
 * @return (true) If the Brick was added to the list.
 * @return (false) If the Brick could not be added.
 */
bool BrickList::_attemptAppendBrick(Brick brickToAdd) {
    int shortestDistance = this->lowestDistance(brickToAdd.position);

    // TODO improve brick self coition detection.

    // If the centre point of the new brick is not inside an existing brick,
    if (shortestDistance > 0) {
        // add the new brick to the end of the list.
        // return true if the brick was added successfully.
        return this->_appendBrick(brickToAdd);
    }

    // Return false to indicate that no brick was added to the list.
    return false;
}

/**
 * @brief Appends 4 Brick structs to the list, representing the 4 boundary
 * walls.
 */
void BrickList::_addWalls() {
    Brick bottomWall = Brick(Position(MAZE_WIDTH / 2, -BRICK_WIDTH / 2), false);
    bottomWall.length = MAZE_WIDTH;
    bottomWall.width = BRICK_WIDTH;
    this->_appendBrick(bottomWall);

    Brick leftWall = Brick(Position(-BRICK_WIDTH / 2, MAZE_LENGTH / 2), true);
    leftWall.length = MAZE_LENGTH;
    leftWall.width = BRICK_WIDTH;
    this->_appendBrick(leftWall);

    Brick topWall =
        Brick(Position(MAZE_WIDTH / 2, MAZE_LENGTH + (BRICK_WIDTH / 2)), false);
    topWall.length = MAZE_WIDTH;
    topWall.width = BRICK_WIDTH;
    this->_appendBrick(topWall);

    Brick rightWall =
        Brick(Position(MAZE_WIDTH + (BRICK_WIDTH / 2), MAZE_LENGTH / 2), true);
    rightWall.length = MAZE_LENGTH;
    rightWall.width = BRICK_WIDTH;
    this->_appendBrick(rightWall);
}
