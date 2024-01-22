

#include "brick.h"

#define ROBOT_RADIUS 120

const String rotationStrings[2] = {"horizontal", "vertical"};

// TODO make this better
Brick::Brick(int x, int y, bool isVertical) {
    this->position.x = x;
    this->position.y = y;
    this->isVertical = isVertical;
}

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

Zone Brick::calculateZone(Position testPosition) {
    bool onLeft = testPosition.x < this->getBottomLeft().x;
    bool onRight = testPosition.x > this->getTopRight().x;

    bool onBottom = testPosition.y < this->getBottomLeft().y;
    bool onTop = testPosition.y > this->getTopRight().y;

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

int Brick ::distanceTo(Position testPosition) {
    int squaredDistance = this->squaredDistanceTo(testPosition);

    // TODO remove the sqrt if possible
    int distance = sqrt(squaredDistance);

    // if dx == 0:
    //     return dy
    // if dy == 0:
    //     return dx

    return distance;
}

// TODO add the pointers to the crow distance to x and y, so that calculating
// distance can be optimized by avoiding using sqrt()
int Brick::squaredDistanceTo(Position testPosition, int* zoneIndexPrt) {
    Zone testPositionZone = this->calculateZone(testPosition);

    *zoneIndexPrt = (int)testPositionZone;

    if (testPositionZone == CentreZone) {
        return 0;
    }

    Position PositionToMeasureFrom;

    switch (testPositionZone) {
        case CentreZone:
            PositionToMeasureFrom.x = testPosition.x;
            PositionToMeasureFrom.y = testPosition.y;
            break;
        case RightZone:
            PositionToMeasureFrom.x = this->getTopRight().x;
            PositionToMeasureFrom.y = testPosition.y;
            break;
        case TopRightZone:
            PositionToMeasureFrom.x = this->getTopRight().x;
            PositionToMeasureFrom.y = this->getTopRight().y;
            break;
        case TopZone:
            PositionToMeasureFrom.x = testPosition.x;
            PositionToMeasureFrom.y = this->getTopRight().y;
            break;
        case TopLeftZone:
            PositionToMeasureFrom.x = this->getBottomLeft().x;
            PositionToMeasureFrom.y = this->getTopRight().y;
            break;
        case LeftZone:
            PositionToMeasureFrom.x = this->getBottomLeft().x;
            PositionToMeasureFrom.y = testPosition.y;
            break;
        case BottomLeftZone:
            PositionToMeasureFrom.x = this->getBottomLeft().x;
            PositionToMeasureFrom.y = this->getBottomLeft().y;
            break;
        case BottomZone:
            PositionToMeasureFrom.x = testPosition.x;
            PositionToMeasureFrom.y = this->getBottomLeft().y;
            break;
        case BottomRightZone:
            PositionToMeasureFrom.x = this->getTopRight().x;
            PositionToMeasureFrom.y = this->getBottomLeft().y;
            break;
    }
    return PositionToMeasureFrom.calculateSquaredDistanceTo(testPosition);
}

String Brick::toString() {
    String stringToReturn;
    stringToReturn += "Brick[";
    stringToReturn += this->position;
    stringToReturn += ",";
    stringToReturn += rotationStrings[this->isVertical];
    stringToReturn += "]";
    return stringToReturn;
}

BrickList::BrickList() {  // TODO Add a Better wall adder
    this->_addWalls();
};

int BrickList::getBrickCount() { return this->_brickCount; }

Brick BrickList::getBrick(int index) {
    // TODO add the bit that checks if index is in range.

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

void BrickList::attemptAppendBrick(Brick brickToAdd) {
    int closestDistance = this->lowestDistance(brickToAdd.position);

    // TODO make this more strict, at the moment there are a lot of overlapping
    // bricks.

    if (closestDistance > 0) {
        this->_appendBrick(brickToAdd);
    }
}

void BrickList::setPreprogrammedMazeData() {
    this->_appendBrick(Brick(125, 460, false));
    this->_appendBrick(Brick(375, 460, false));
    this->_appendBrick(Brick(40, 1000, true));
    this->_appendBrick(Brick(125, 1460, false));
    this->_appendBrick(Brick(375, 1460, false));
    this->_appendBrick(Brick(540, 1545, true));
    this->_appendBrick(Brick(1460, 1545, true));
    this->_appendBrick(Brick(1295, 1460, false));
    this->_appendBrick(Brick(1130, 1375, true));
    this->_appendBrick(Brick(1460, 625, true));
    this->_appendBrick(Brick(1295, 540, false));
    this->_appendBrick(Brick(1130, 455, true));
    this->_appendBrick(Brick(460, 1000, true));
    this->_appendBrick(Brick(625, 915, false));
    this->_appendBrick(Brick(875, 915, false));
}

void BrickList::_getOrthogonalBrickIndicies(Position robotPosition,
                                            int* bottom_IP, int* left_IP,
                                            int* top_IP, int* right_IP) {
    int bottom_IL = -1;
    int left_IL = -1;
    int top_IL = -1;
    int right_IL = -1;

    int brickCount = this->getBrickCount();

    // TODO optimize this
    for (int i = 0; i < brickCount; i++) {
        Brick brick = this->getBrick(i);
        Zone zoneFromBrick = brick.calculateZone(robotPosition);

        if (zoneFromBrick == TopZone) {
            if (bottom_IL != -1) {
                int oldSquaredDistance =
                    this->getBrick(bottom_IL).squaredDistanceTo(robotPosition);
                int newSquaredDistance =
                    this->getBrick(i).squaredDistanceTo(robotPosition);

                if (newSquaredDistance < oldSquaredDistance) {
                    bottom_IL = i;
                }
            } else {
                bottom_IL = i;
            }
        }
        if (zoneFromBrick == RightZone) {
            if (left_IL != -1) {
                int oldSquaredDistance =
                    this->getBrick(left_IL).squaredDistanceTo(robotPosition);
                int newSquaredDistance =
                    this->getBrick(i).squaredDistanceTo(robotPosition);

                if (newSquaredDistance < oldSquaredDistance) {
                    left_IL = i;
                }
            } else {
                left_IL = i;
            }
        }
        if (zoneFromBrick == BottomZone) {
            if (top_IL != -1) {
                int oldSquaredDistance =
                    this->getBrick(top_IL).squaredDistanceTo(robotPosition);
                int newSquaredDistance =
                    this->getBrick(i).squaredDistanceTo(robotPosition);

                if (newSquaredDistance < oldSquaredDistance) {
                    top_IL = i;
                }
            } else {
                top_IL = i;
            }
        }

        if (zoneFromBrick == LeftZone) {
            if (right_IL != -1) {
                int oldSquaredDistance =
                    this->getBrick(right_IL).squaredDistanceTo(robotPosition);
                int newSquaredDistance =
                    this->getBrick(i).squaredDistanceTo(robotPosition);

                if (newSquaredDistance < oldSquaredDistance) {
                    right_IL = i;
                }
            } else {
                right_IL = i;
            }
        }
    }

    *bottom_IP = bottom_IL;
    *left_IP = left_IL;
    *top_IP = top_IL;
    *right_IP = right_IL;
}

// void BrickList::_tryAndFitBrick(Position brickEdgePosition, ) {
//
//     Serial.print(" Trying to fit brick:");
//     Serial.print(brickEdgePosition);
//     Serial.print(" :");
//     Serial.println();
// }

// TODO could make this better by reading in each orthogonal distance at
// once;
int BrickList::compare(Position robotPosition, Angle angleOfSensor,
                       int measuredDistance) {
    const int driftTolerance = 20;

    // TODO refactor
    bool pointingDown = angleOfSensor.isOrthogonallyDown();
    bool pointingLeft = angleOfSensor.isOrthogonallyLeft();
    bool pointingUp = angleOfSensor.isOrthogonallyUp();
    bool pointingRight = angleOfSensor.isOrthogonallyRight();

    if (!angleOfSensor.isOrthogonal()) {
        // This function only works with orthogonal
        return 0;
    }
    int bottomBrick_I = -1;
    int leftBrick_I = -1;
    int topBrick_I = -1;
    int rightBrick_I = -1;

    this->_getOrthogonalBrickIndicies(robotPosition, &bottomBrick_I,
                                      &leftBrick_I, &topBrick_I, &rightBrick_I);

    Brick targetBrick;

    if (pointingDown && (bottomBrick_I != -1)) {
        targetBrick = this->getBrick(bottomBrick_I);
    } else if (pointingLeft && (leftBrick_I != -1)) {
        targetBrick = this->getBrick(leftBrick_I);
    } else if (pointingUp && (topBrick_I != -1)) {
        targetBrick = this->getBrick(topBrick_I);
    } else if (pointingRight && (rightBrick_I != -1)) {
        targetBrick = this->getBrick(rightBrick_I);
    } else {
        return -2;
    }

    int storedDistance = targetBrick.distanceTo(robotPosition);

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

void BrickList::setBrickFromApproximation(Position brickEdgePosition,
                                          Angle angleOfSensor) {
    const int snapTolerance = 125;

    if (!angleOfSensor.isOrthogonal()) {
        // THis function only works when the sensor is orthogonal.
        return;
    }
    bool pointingDown = angleOfSensor.isOrthogonallyDown();
    bool pointingLeft = angleOfSensor.isOrthogonallyLeft();
    bool pointingUp = angleOfSensor.isOrthogonallyUp();
    bool pointingRight = angleOfSensor.isOrthogonallyRight();

    Position newBrickPosition;
    bool isNewBrickVertical;

    if (pointingDown) {
        isNewBrickVertical = false;
        newBrickPosition.x = brickEdgePosition.x;
        newBrickPosition.y = brickEdgePosition.y - 40;
    }
    if (pointingLeft) {
        isNewBrickVertical = true;
        newBrickPosition.x = brickEdgePosition.x - 40;
        newBrickPosition.y = brickEdgePosition.y;
    }
    if (pointingUp) {
        isNewBrickVertical = false;
        newBrickPosition.x = brickEdgePosition.x;
        newBrickPosition.y = brickEdgePosition.y + 40;
    }
    if (pointingRight) {
        isNewBrickVertical = true;
        newBrickPosition.x = brickEdgePosition.x + 40;
        newBrickPosition.y = brickEdgePosition.y;
    }

    int bottomBrick_I = -1;
    int leftBrick_I = -1;
    int topBrick_I = -1;
    int rightBrick_I = -1;

    this->_getOrthogonalBrickIndicies(newBrickPosition, &bottomBrick_I,
                                      &leftBrick_I, &topBrick_I, &rightBrick_I);

    int distanceToBrickOnLeft = 0;  // left being a local term of rotation

    if ((pointingDown) && (rightBrick_I != -1)) {
        distanceToBrickOnLeft =
            this->getBrick(rightBrick_I).distanceTo(newBrickPosition);
    }
    if ((pointingLeft) && (bottomBrick_I != -1)) {
        distanceToBrickOnLeft =
            this->getBrick(bottomBrick_I).distanceTo(newBrickPosition);
    }
    if ((pointingUp) && (leftBrick_I != -1)) {
        distanceToBrickOnLeft =
            this->getBrick(leftBrick_I).distanceTo(newBrickPosition);
    }
    if ((pointingRight) && (topBrick_I != -1)) {
        distanceToBrickOnLeft =
            this->getBrick(topBrick_I).distanceTo(newBrickPosition);
    }

    int horizontalDistanceFromSnapPosition = distanceToBrickOnLeft - 250;

    Serial.print(" DistOnLEft:");
    Serial.print(distanceToBrickOnLeft);
    Serial.print(" horizontalDistanceFromSnapPosition:");
    Serial.println(horizontalDistanceFromSnapPosition);

    bool canSnap = (abs(horizontalDistanceFromSnapPosition) < snapTolerance);

    if (pointingDown && canSnap) {
        Serial.println("Snapping down");
        newBrickPosition.x += horizontalDistanceFromSnapPosition;
    }
    if (pointingLeft && canSnap) {
        newBrickPosition.y += horizontalDistanceFromSnapPosition;
    }
    if (pointingUp && canSnap) {
        newBrickPosition.x -= horizontalDistanceFromSnapPosition;
    }
    if (pointingRight && canSnap) {
        newBrickPosition.y -= horizontalDistanceFromSnapPosition;
    }

    Brick brickToAdd;
    brickToAdd.position = newBrickPosition;
    brickToAdd.isVertical = isNewBrickVertical;
    this->_appendBrick(brickToAdd);
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
    Brick bottomWall;
    bottomWall.length = 1500;
    bottomWall.width = 80;
    bottomWall.position.x = 750;
    bottomWall.position.y = -40;
    bottomWall.isVertical = false;
    this->_appendBrick(bottomWall);

    Brick leftWall;
    leftWall.length = 2000;
    leftWall.width = 80;
    leftWall.position.x = -40;
    leftWall.position.y = 1000;
    leftWall.isVertical = true;
    this->_appendBrick(leftWall);

    Brick topWall;
    topWall.length = 1500;
    topWall.width = 80;
    topWall.position.x = 750;
    topWall.position.y = 2040;
    topWall.isVertical = false;
    this->_appendBrick(topWall);

    Brick rightWall;
    rightWall.length = 2000;
    rightWall.width = 80;
    rightWall.position.x = 1540;
    rightWall.position.y = 1000;
    rightWall.isVertical = true;
    this->_appendBrick(rightWall);
}