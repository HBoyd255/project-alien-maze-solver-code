

#include "brick.h"

#include "comparison.h"
#include "map.h"
#include "mazeConstants.h"

#define ROBOT_RADIUS 120

/**
 * @brief Construct a new Brick object.
 *
 * @param brickPosition The position of the brick.
 * @param isVertical The rotation of the brick, horizontal by default.
 */
Brick::Brick(Position brickPosition, bool isVertical)
    : position(brickPosition),
      isVertical(isVertical),
      length(BRICK_LENGTH),
      width(BRICK_WIDTH) {}

/**
 * @brief Get the position of the bricks bottom left corner.
 *
 * @return (Position) The bricks bottom left corner.
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
 * @brief Get the position of the bricks top right corner.
 *
 * @return (Position) The bricks top right corner.
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
 * current brick.
 *
 * @param target the position to test.
 * @return (Zone) The zone of the position, relative to the current brick.
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
 * of this brick and a provided target position.
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
 * param zone_IP
 * @param dx_P A pointer to return the difference in x positions.
 * @param dy_P A pointer to return the difference in y positions.
 * @return (float) The square of the euclidean distance to the target
 * position from the edge of this brick, in millimeters.
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
 * @brief Converts the current brick into a human readable string.
 *
 * @return (String) The current brick as a human readable string.
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

BrickList::BrickList() { this->_addWalls(); };

int BrickList::getBrickCount() { return this->_brickCount; }

Brick BrickList::getBrick(int index) {
    if (index > this->getBrickCount()) {
        Serial.println("Brick out of range");
        return Brick(Position(-1, -1), 0);
    }
    return this->_brickArray[index];
}

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

bool BrickList::_attemptAppendBrick(Brick brickToAdd) {
    int closestDistance = this->lowestDistance(brickToAdd.position);

    // TODO improve brick self coition detection.

    if (closestDistance > 0) {
        this->_appendBrick(brickToAdd);
        return true;
    }
    return false;
}

void BrickList::setPreprogrammedMazeData() {
    // This is the hard coded data for the "seen maze"
    // This data is only used for algorithm testing,
    // it is at no point used during the demonstration.

    // this->_appendBrick(Brick(125, 460, false));
    // this->_appendBrick(Brick(375, 460, false));
    // this->_appendBrick(Brick(40, 1000, true));
    // this->_appendBrick(Brick(125, 1460, false));
    // this->_appendBrick(Brick(375, 1460, false));
    // this->_appendBrick(Brick(540, 1545, true));
    // this->_appendBrick(Brick(1460, 1545, true));
    // this->_appendBrick(Brick(1295, 1460, false));
    // this->_appendBrick(Brick(1130, 1375, true));
    // this->_appendBrick(Brick(1460, 625, true));
    // this->_appendBrick(Brick(1295, 540, false));
    // this->_appendBrick(Brick(1130, 455, true));
    // this->_appendBrick(Brick(460, 1000, true));
    // this->_appendBrick(Brick(625, 915, false));
    // this->_appendBrick(Brick(875, 915, false));
}

/**
 * @brief checks if a seen brick corner falls withing the outer bounds of
 * the maze, and if so places a brick in the brick list, flat against the
 * wall.
 *
 * @param brickCorner The position of the corner of the brick that was seen
 * by a sensor.
 * @return (true) If a brick has been added to the list.
 * @return (false) If a brick has been not been added to the list.
 */
bool BrickList::handleBrickFromWallPosition(Position brickOuterCorner) {
    // How close, in millimeters, a corner needs to be to the wall, to be
    // considered to be flat against the wall.
    const int wallTolerance = 150;

    bool onBottomWall = within(brickOuterCorner.y, 0, wallTolerance);
    bool onLeftWall = within(brickOuterCorner.x, 0, wallTolerance);
    bool onTopWall = within(brickOuterCorner.y, MAZE_LENGTH, wallTolerance);
    bool onRightWall = within(brickOuterCorner.y, MAZE_WIDTH, wallTolerance);

    // This could be improved with trigonometry, but is perfectly functional
    // and readable as is, so doesn't need refactoring at this stage.
    if (onBottomWall) {
        Brick brickToAdd;

        brickToAdd.position.x = brickOuterCorner.x - 125;
        brickToAdd.position.y = 45;
        brickToAdd.isVertical = false;

        // return a boolean indicating if a brick has been added to the list.
        return this->_attemptAppendBrick(brickToAdd);
    }
    if (onLeftWall) {
        Brick brickToAdd;

        brickToAdd.position.x = 45;
        brickToAdd.position.y = brickOuterCorner.y + 125;
        brickToAdd.isVertical = true;

        // return a boolean indicating if a brick has been added to the list.
        return this->_attemptAppendBrick(brickToAdd);
    }
    if (onTopWall) {
        Brick brickToAdd;

        brickToAdd.position.x = brickOuterCorner.x + 125;
        brickToAdd.position.y = 1955;
        brickToAdd.isVertical = false;

        // return a boolean indicating if a brick has been added to the list.
        return this->_attemptAppendBrick(brickToAdd);
    }
    if (onRightWall) {
        Brick brickToAdd;

        brickToAdd.position.x = 1455;
        brickToAdd.position.y = brickOuterCorner.y - 125;
        brickToAdd.isVertical = true;

        // return a boolean indicating if a brick has been added to the list.
        return this->_attemptAppendBrick(brickToAdd);
    }

    // return false as no bricks have been added to the list.
    return false;
}

/**
 * @brief Takes a line, provided as a start and end position, and calculates
 * if a brick can be approximated from it.
 *
 * @param robotPosition The current position of the robot.
 * @param statingCorner The position that the seen line starts.
 * @param lineEnd  The position that the seen line ends.
 * @return (true) If a brick has been added to the list.
 * @return (false) If a brick has been not been added to the list.
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

    // This could be improved with trigonometry, but is perfectly functional
    // and readable as is, so doesn't need refactoring at this stage.

    if (seenShortSide && lineIsVertical) {
        Brick brickToAdd;

        if (robotIsLeftOfLine) {
            brickToAdd.position.x = lineCentrePosition.x + (BRICK_LENGTH / 2);
        } else {
            brickToAdd.position.x = lineCentrePosition.x - (BRICK_LENGTH / 2);
        }

        brickToAdd.position.y = lineCentrePosition.y;
        brickToAdd.isVertical = false;

        return this->_attemptAppendBrick(brickToAdd);
    }

    if (seenLongSide && lineIsVertical) {
        Brick brickToAdd;

        brickToAdd.position.y = lineCentrePosition.y;
        brickToAdd.isVertical = true;

        if (robotIsLeftOfLine) {
            brickToAdd.position.x = lineCentrePosition.x + (BRICK_WIDTH / 2);
        } else {
            brickToAdd.position.x = lineCentrePosition.x - (BRICK_WIDTH / 2);
        }
        return this->_attemptAppendBrick(brickToAdd);
    }

    if (seenShortSide && !lineIsVertical) {
        Brick brickToAdd;

        brickToAdd.position.x = lineCentrePosition.x;
        brickToAdd.isVertical = true;

        if (robotIsBelowLine) {
            brickToAdd.position.y = lineCentrePosition.y + (BRICK_LENGTH / 2);
        } else {
            brickToAdd.position.y = lineCentrePosition.y - (BRICK_LENGTH / 2);
        }
        return this->_attemptAppendBrick(brickToAdd);
    }
    if (seenLongSide && !lineIsVertical) {
        Brick brickToAdd;

        brickToAdd.position.x = lineCentrePosition.x;
        brickToAdd.isVertical = false;

        if (robotIsBelowLine) {
            brickToAdd.position.y = lineCentrePosition.y + (BRICK_WIDTH / 2);
        } else {
            brickToAdd.position.y = lineCentrePosition.y - (BRICK_WIDTH / 2);
        }
        return this->_attemptAppendBrick(brickToAdd);
    }

    return false;
}

/**
 * @brief
 *
 * @param robotPosition
 * @param angleOfSensor
 * @param measuredDistance
 * @param tolerance
 * @param map_P
 * @return (true) If a brick has been added to the list.
 * @return (false) If a brick has been not been added to the list.
 */
bool BrickList::handleBrickFromSensorAndMap(Position robotPosition,
                                            Angle angleOfSensor,
                                            int measuredDistance, int tolerance,
                                            Map* map_P) {
    bool needsRecalibration = false;

    // This code needs refactoring badly but I ran out of time.

    if (!angleOfSensor.isOrthogonal(tolerance)) {
        // This function only works with orthogonal angles
        return false;
    }

    if (measuredDistance > 400) {
        return false;
    }

    bool pointingDown = angleOfSensor.isPointingDown();
    bool pointingLeft = angleOfSensor.isPointingLeft();
    bool pointingUp = angleOfSensor.isPointingUp();
    bool pointingRight = angleOfSensor.isPointingRight();

    Position brickEdgePosition;

    // TODO improve with trigonometry
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

    map_P->seenPosition(brickEdgePosition);

    int comparison =
        this->_compare(robotPosition, angleOfSensor, measuredDistance);

    if (comparison == 1) {
        Position positionA;
        Position positionB;

        Position positionC;
        Position positionD;

        if (pointingDown) {
            if (brickEdgePosition.y < 250) {
                return false;
            }

            positionA.x = brickEdgePosition.x + BRICK_WIDTH / 2;
            positionA.y = brickEdgePosition.y;
            positionB.x = brickEdgePosition.x - BRICK_WIDTH / 2;
            positionB.y = brickEdgePosition.y - BRICK_LENGTH;

            positionC.x = brickEdgePosition.x + BRICK_LENGTH / 2;
            positionC.y = brickEdgePosition.y;
            positionD.x = brickEdgePosition.x - BRICK_LENGTH / 2;
            positionD.y = brickEdgePosition.y - BRICK_WIDTH;
        }

        if (pointingLeft) {
            if (brickEdgePosition.x < 250) {
                return false;
            }
            positionA.x = brickEdgePosition.x;
            positionA.y = brickEdgePosition.y - BRICK_WIDTH / 2;
            positionB.x = brickEdgePosition.x - BRICK_LENGTH;
            positionB.y = brickEdgePosition.y + BRICK_WIDTH / 2;

            positionC.x = brickEdgePosition.x;
            positionC.y = brickEdgePosition.y - BRICK_LENGTH / 2;
            positionD.x = brickEdgePosition.x - BRICK_WIDTH;
            positionD.y = brickEdgePosition.y + BRICK_LENGTH / 2;
        }
        if (pointingUp) {
            if (brickEdgePosition.y > 1750) {
                return false;
            }
            positionA.x = brickEdgePosition.x - BRICK_WIDTH / 2;
            positionA.y = brickEdgePosition.y;
            positionB.x = brickEdgePosition.x + BRICK_WIDTH / 2;
            positionB.y = brickEdgePosition.y + BRICK_LENGTH;

            positionC.x = brickEdgePosition.x - BRICK_LENGTH / 2;
            positionC.y = brickEdgePosition.y;
            positionD.x = brickEdgePosition.x + BRICK_LENGTH / 2;
            positionD.y = brickEdgePosition.y + BRICK_WIDTH;
        }
        if (pointingRight) {
            if (brickEdgePosition.x > 1250) {
                return false;
            }
            positionA.x = brickEdgePosition.x;
            positionA.y = brickEdgePosition.y + BRICK_WIDTH / 2;
            positionB.x = brickEdgePosition.x + BRICK_LENGTH;
            positionB.y = brickEdgePosition.y - BRICK_WIDTH / 2;

            positionC.x = brickEdgePosition.x;
            positionC.y = brickEdgePosition.y + BRICK_LENGTH / 2;
            positionD.x = brickEdgePosition.x + BRICK_WIDTH;
            positionD.y = brickEdgePosition.y - BRICK_LENGTH / 2;
        }

        bool hasRanOffMaze = false;
        bool brickCouldBeVert =
            map_P->safeForBrick(positionA, positionB, &hasRanOffMaze);

        // disregard any values that are too close to the edge.
        if (hasRanOffMaze) {
            return false;
        }

        // TODO change to int 0 for not safe, 1 for safe, -1 for ran off
        // maze.
        bool brickCouldBeHor =
            map_P->safeForBrick(positionC, positionD, &hasRanOffMaze);

        if (hasRanOffMaze) {
            return false;
        }

        if (brickCouldBeVert) {
        } else if (brickCouldBeHor) {
            this->_setBrickFromEdge(brickEdgePosition, angleOfSensor,
                                    tolerance);
        }
    } else if (comparison == -1) {
        needsRecalibration = true;
    }
    return needsRecalibration;
}

int BrickList::_getOrthogonalBrickDistance(Position robotPosition,
                                           Angle directionOfBrick) {
    // This function assumes that the angle provided is pretty orthogonal

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
    int closestDistance = startingValue;

    int brickCount = this->getBrickCount();

    for (int i = 0; i < brickCount; i++) {
        Brick currentBrick = this->getBrick(i);
        Zone zoneFromBrick = currentBrick.calculateZone(robotPosition);
        if (zoneFromBrick == targetZone) {
            if (currentBrick.distanceTo(robotPosition) < closestDistance) {
                closestDistance = currentBrick.distanceTo(robotPosition);
            }
        }
    }

    if (closestDistance == startingValue) {
        return -1;  // FAIL
    }

    return closestDistance;
}

/**
 * @brief
 *
 * @return (1) If a brick need placing.
 * @return (0) If noting needs doing.
 * @return (-1) If Position need recalibrating.
 * @return (-2) Any other error.
 */
int BrickList::_compare(Position robotPosition, Angle angleOfSensor,
                        int measuredDistance) {
    const int driftTolerance = 20;

    if (!angleOfSensor.isOrthogonal()) {
        // This function only works with orthogonal
        return 0;
    }

    int storedDistance =
        this->_getOrthogonalBrickDistance(robotPosition, angleOfSensor);

    if (storedDistance < 1) {
        return -2;
    }

    // TODO document this better.
    if (abs(storedDistance - measuredDistance) < driftTolerance) {
        // IF storedDistance ≈≈ measuredDistance:
        //     Brick is already stored

        // Everything is fine
        return 0;
    } else if (storedDistance > measuredDistance) {
        // IF storedDistance >≈ measuredDistance:
        //     THere is a brick that is not stored

        // Need to ad a brick
        return 1;
    } else if (storedDistance < measuredDistance) {
        // IF storedDistance <≈ measuredDistance:
        //     SOmething has gone wrong, robot has drifted,

        // Need to recalibrate
        return -1;
    }

    // -2 is an error value;
    return -2;
}

void BrickList::_setBrickFromEdge(Position brickEdgePosition,
                                  Angle angleOfSensor, int tolerance) {
    if (!angleOfSensor.isOrthogonal(tolerance)) {
        // This function only works when the sensor is orthogonal.
        return;
    }

    bool pointingLeft = angleOfSensor.isPointingLeft();
    bool pointingRight = angleOfSensor.isPointingRight();

    bool isVertical = (pointingLeft || pointingRight);

    Brick newBrick(brickEdgePosition, isVertical);

    Position offset = Position(newBrick.width / 2, 0).rotate(angleOfSensor);

    newBrick.position.offset(offset);

    this->_attemptAppendBrick(newBrick);
}

int BrickList::lowestDistance(Position testPosition, int* indexOfClosestBrick_P,
                              int* zoneFromClosestBrick_P) {
    int brickCount = this->getBrickCount();

    int indexOfClosestBrick_L = -1;
    int zoneFromClosestBrick_L = -1;
    int lowestSquaredDistance = INT32_MAX;

    for (int i = 0; i < brickCount; i++) {
        Brick brick = this->getBrick(i);

        int local_zoneFromBrick = -1;

        int squaredDistanceToBrick =
            brick.squaredDistanceTo(testPosition, &local_zoneFromBrick);

        if (squaredDistanceToBrick < lowestSquaredDistance) {
            lowestSquaredDistance = squaredDistanceToBrick;
            indexOfClosestBrick_L = i;
            zoneFromClosestBrick_L = local_zoneFromBrick;
        }
    }
    if (indexOfClosestBrick_L != -1) {
        *indexOfClosestBrick_P = indexOfClosestBrick_L;
    }

    if (zoneFromClosestBrick_L != -1) {
        *zoneFromClosestBrick_P = zoneFromClosestBrick_L;
    }

    // TODO make this just get the lowest Brick, then do stuff to it.
    return sqrt(lowestSquaredDistance);
}

void BrickList::_appendBrick(Brick brickToAdd) {
    if (this->_brickCount < MAX_BRICK_COUNT) {
        this->_brickArray[this->_brickCount] = brickToAdd;
        this->_brickCount += 1;
    }
}

void BrickList::_addWalls() {
    Brick bottomWall = Brick(Position(750, -40), false);
    bottomWall.length = 1500;
    bottomWall.width = 80;
    this->_appendBrick(bottomWall);

    Brick leftWall = Brick(Position(-40, 1000), true);
    leftWall.length = 2000;
    leftWall.width = 80;

    this->_appendBrick(leftWall);

    Brick topWall = Brick(Position(750, 2040), false);
    topWall.length = 1500;
    topWall.width = 80;

    this->_appendBrick(topWall);

    Brick rightWall = Brick(Position(1540, 1000), true);
    rightWall.length = 2000;
    rightWall.width = 80;

    this->_appendBrick(rightWall);
}