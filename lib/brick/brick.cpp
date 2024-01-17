

#include "brick.h"

// TODO rename this
#define DEADZONE_RADIUS_INNER 120

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

    // this->_brickArray[8] = {{1375, 995}, false};
    // this->_brickArray[12] = {{1125, 995}, false};

    this->_brickCount = 15;
}