

#include "brick.h"

// TODO rename this
#define DEADZONE_RADIUS_INNER 120
#define DEADZONE_RADIUS_OUTER 150

#include "mazeConstants.h"

bool inRectangle(Position testPosition, Position bottomLeft,
                 Position topRight) {
    bool inFromLeft = testPosition.x >= bottomLeft.x;
    bool inFromRight = testPosition.x <= topRight.x;
    bool inFromBottom = testPosition.y >= bottomLeft.y;
    bool inFromTop = testPosition.y <= topRight.y;

    bool inZone = inFromLeft && inFromRight && inFromBottom && inFromTop;

    return inZone;
}

bool inRadius(Position testPosition, Position cornerPosition, int zoneRadius) {
    int xDifference = abs(cornerPosition.x - testPosition.x);
    int yDifference = abs(cornerPosition.y - testPosition.y);

    int squaredDistance = xDifference * xDifference + yDifference * yDifference;

    int squaredZoneRadius = zoneRadius * zoneRadius;

    bool withinRadius = squaredDistance <= squaredZoneRadius;

    return withinRadius;
}

const String rotationStrings[2] = {"horizontal", "vertical"};

/**
 * @brief Converts the Pose into a string.
 *
 * @return (String) The Pose as a string, in the format (X,Y)@Angle°
 */
Brick::operator String() const {
    String stringToReturn;
    stringToReturn += "Brick[";
    stringToReturn += this->position;
    stringToReturn += ",";
    stringToReturn += rotationStrings[this->isVertical];
    stringToReturn += "]";
    return stringToReturn;
}

Position Brick::getBottomLeft() {
    Position positionToReturn = this->position;

    if (this->isVertical) {
        positionToReturn.x -= (BRICK_WIDTH / 2);
        positionToReturn.y -= (BRICK_LENGTH / 2);
    } else {
        positionToReturn.x -= (BRICK_LENGTH / 2);
        positionToReturn.y -= (BRICK_WIDTH / 2);
    }
    return positionToReturn;
}

Position Brick::getTopRight() {
    Position positionToReturn = this->position;

    if (this->isVertical) {
        positionToReturn.x += (BRICK_WIDTH / 2);
        positionToReturn.y += (BRICK_LENGTH / 2);
    } else {
        positionToReturn.x += (BRICK_LENGTH / 2);
        positionToReturn.y += (BRICK_WIDTH / 2);
    }
    return positionToReturn;
}

Zone Brick::calculateZone(Position testPosition) {
    // i hate using one line if statements but this works,
    // TODO refactor;

    bool onLeft = testPosition.x < this->getBottomLeft().x;
    bool onRight = testPosition.x > this->getTopRight().x;

    bool onBottom = testPosition.y < this->getBottomLeft().y;
    bool onTop = testPosition.y > this->getTopRight().y;

    if (onTop) {
        if (onLeft) return TopLeftZone;
        if (onRight) return TopRightZone;
        return TopZone;
    }
    if (onBottom) {
        if (onLeft) return BottomLeftZone;
        if (onRight) return BottomRightZone;
        return BottomZone;
    }
    if (onLeft) return LeftZone;
    if (onRight) return RightZone;

    return CentreZone;
}

// COME HERE AND FINISH THIS FUNCTION.
int Brick::squaredDistanceTo(Position testPosition) {
    Zone testPositionZone = this->calculateZone(testPosition);

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

BrickList::BrickList(){};

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
        stringToReturn += (String)this->getBrick(i);

        if (i != (brickCount - 1)) {
            stringToReturn += ", ";
        }
    }

    stringToReturn += "}]";

    return stringToReturn;
}

// bool BrickList::inDeadzone(Position positionToTest) {
//     return this->_inZone(positionToTest, DEADZONE_RADIUS_INNER);
// }
// bool BrickList::nearDeadzone(Position positionToTest) {
//     return this->_inZone(positionToTest, DEADZONE_RADIUS_OUTER);
// }

// 
// // TODO refactor
// bool BrickList::_inZone(Position positionToTest, int radius) {
//     int brickCount = this->getBrickCount();
// 
//     for (int i = 0; i < brickCount; i++) {
//         bool collidedWithThisBrick = false;
// 
//         Brick brick = this->getBrick(i);
// 
//         Position brickBottomLeft = brick.getBottomLeft();
//         Position brickTopRight = brick.getTopRight();
// 
//         Position deadZoneBottomLeft;
//         deadZoneBottomLeft.x = brickBottomLeft.x - radius;
//         deadZoneBottomLeft.y = brickBottomLeft.y - radius;
// 
//         Position deadZoneTopRight;
//         deadZoneTopRight.x = brickTopRight.x + radius;
//         deadZoneTopRight.y = brickTopRight.y + radius;
// 
//         bool inLargerZoon =
//             inRectangle(positionToTest, deadZoneBottomLeft, deadZoneTopRight);
// 
//         // Serial.print(" inLargerZoon:");
//         // Serial.print(inLargerZoon);
// 
//         if (inLargerZoon) {
//             Position testZone1BL;  // VERT Zone
//             testZone1BL.x = brickBottomLeft.x;
//             testZone1BL.y = deadZoneBottomLeft.y;
//             Position testZone1TR;
//             testZone1TR.x = brickTopRight.x;
//             testZone1TR.y = deadZoneTopRight.y;
// 
//             // Serial.print(" testZone1BL");
//             // Serial.print(testZone1BL);
//             // Serial.print(" testZone1TR");
//             // Serial.print(testZone1TR);
// 
//             bool inTestZone1 =
//                 inRectangle(positionToTest, testZone1BL, testZone1TR);
// 
//             Position testZone2BL;  // VERT Zone
//             testZone2BL.x = deadZoneBottomLeft.x;
//             testZone2BL.y = brickBottomLeft.y;
//             Position testZone2TR;
//             testZone2TR.x = deadZoneTopRight.x;
//             testZone2TR.y = brickTopRight.y;
// 
//             // Serial.print(" testZone2BL");
//             // Serial.print(testZone2BL);
//             // Serial.print(" testZone2TR");
//             // Serial.print(testZone2TR);
// 
//             bool inTestZone2 =
//                 inRectangle(positionToTest, testZone2BL, testZone2TR);
// 
//             bool inTestZones = inTestZone1 || inTestZone2;
// 
//             // Serial.print(" inTestZones:");
//             // Serial.print(inTestZones);
// 
//             if (inTestZones) {
//                 return 1;
//             } else {
//                 Position brickTopLeft = {brickBottomLeft.x, brickTopRight.y};
//                 Position brickBottomRight = {brickTopRight.x,
//                                              brickBottomLeft.y};
// 
//                 bool inBL = inRadius(positionToTest, brickBottomLeft, radius);
//                 if (inBL) {
//                     return 1;
//                 }
//                 bool inTR = inRadius(positionToTest, brickTopRight, radius);
//                 if (inTR) {
//                     return 1;
//                 }
//                 bool inTL = inRadius(positionToTest, brickTopLeft, radius);
//                 if (inTL) {
//                     return 1;
//                 }
//                 bool inBR = inRadius(positionToTest, brickBottomRight, radius);
//                 if (inBR) {
//                     return 1;
//                 }
//             }
//         }
// 
//         if (collidedWithThisBrick) {
//             return true;
//         }
//     }
// 
//     return false;
// }

void BrickList::TEMP_addBrick(Brick brickToAdd, int index) {
    int newBrickCount = index + 1;
    int oldBrickCount = this->_brickCount;

    this->_brickCount = max(oldBrickCount, newBrickCount);

    this->_brickArray[index] = brickToAdd;
}
void BrickList::TEMP_fillWithTestData() {
    // this->_brickArray[0] = {{500, 300}, false};
    // this->_brickArray[1] = {{1000, 300}, true};
    // this->_brickCount = 2;

    this->_brickArray[0] = {{125, 460}, false};
    this->_brickArray[1] = {{375, 460}, false};
    this->_brickArray[2] = {{40, 1000}, true};
    this->_brickArray[3] = {{125, 1460}, false};
    this->_brickArray[4] = {{375, 1460}, false};
    this->_brickArray[5] = {{540, 1545}, true};
    this->_brickArray[6] = {{1460, 1545}, true};
    this->_brickArray[7] = {{1295, 1460}, false};
    this->_brickArray[8] = {{1130, 1375}, true};
    this->_brickArray[9] = {{1460, 625}, true};
    this->_brickArray[10] = {{1295, 540}, false};
    this->_brickArray[11] = {{1130, 455}, true};
    this->_brickArray[12] = {{460, 1000}, true};
    this->_brickArray[13] = {{625, 915}, false};
    this->_brickArray[14] = {{875, 915}, false};

    this->_brickCount = 15;
}