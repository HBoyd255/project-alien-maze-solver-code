

#include "map.h"

#include <queue>

#include "angleAndPosition.h"
#include "brick.h"

#define UINT12_MAX (0xfff)

#define ROBOT_RADIUS 120

#define DIAGONAL_DISTANCE 3
#define ORTHOGONAL_DISTANCE 2

#define SQUARED_ROBOT_RADIUS ROBOT_RADIUS* ROBOT_RADIUS

MapPoint::MapPoint(int x, int y) : x(x), y(y) {}

int MapPoint::squaredDistanceTo(MapPoint otherPoint) {
    int xDiff = abs(this->x - otherPoint.x);
    int yDiff = abs(this->y - otherPoint.y);

    return xDiff * xDiff + yDiff * yDiff;
}

Position MapPoint::toPosition() const {
    float xValueInMM = this->x * 10.0f;
    float yValueInMM = this->y * 10.0f;

    Position positionToReturn;
    positionToReturn.x = xValueInMM;
    positionToReturn.y = yValueInMM;

    return positionToReturn;
}

void MapPoint::setFromPosition(Position position) {
    int xValueInCM = position.x / 10.0f;
    int yValueInCM = position.y / 10.0f;

    this->x = xValueInCM;
    this->y = yValueInCM;
}

String MapPoint::toString() const {
    String stringToReturn = "MP:(";
    stringToReturn += this->x;
    stringToReturn += ",";
    stringToReturn += this->y;
    stringToReturn += ")";

    return stringToReturn;
}

MapPoint MapPoint::operator+(const MapPoint& PointToAdd) const {
    return MapPoint(this->x + PointToAdd.x, this->y + PointToAdd.y);
}
MapPoint& MapPoint::operator+=(const MapPoint& PointToAdd) {
    this->x += PointToAdd.x;
    this->y += PointToAdd.y;

    return *this;
}

bool MapPoint::operator==(const MapPoint& PointToCompare) const {
    return ((this->x == PointToCompare.x) && (this->y == PointToCompare.y));
}

Map::Map() { this->_resetData(); }

void Map::_primeFromBrickList(BrickList brickList) {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = {x, y};
        Position scanPosition = scanPoint.toPosition();

        int brickCount = brickList.getBrickCount();

        int lowestSquaredDistance = INT32_MAX;

        for (int i = 0; i < brickCount; i++) {
            Brick brick = brickList.getBrick(i);

            int squaredDistanceToBrick = brick.squaredDistanceTo(scanPosition);

            if (squaredDistanceToBrick < lowestSquaredDistance) {
                lowestSquaredDistance = squaredDistanceToBrick;
            }
        }

        int lowestDistance = sqrt(lowestSquaredDistance);

        lowestDistance = constrain(lowestDistance, 0, UINT8_MAX);
        bool pointIsBlocked = (lowestDistance < ROBOT_RADIUS);

        this->_setBlocked(scanPoint, pointIsBlocked);
        this->_setDirection(scanPoint, 0);
        this->_setDistanceToWall(scanPoint, lowestDistance);
        this->_setDistanceToGoal(scanPoint, UINT12_MAX);
    }
}

void Map::solve(BrickList brickList, MapPoint endPoint) {

    this->_primeFromBrickList(brickList);

    if (!_validatePoint(endPoint)) {
        Serial.println("Point out of bounds.");

        return;
    }
    std::queue<MapPoint> pointQueue;
    pointQueue.push(endPoint);
    this->_setDistanceToGoal(endPoint, 0);

    this->_endPoint = endPoint;

    while (!pointQueue.empty()) {
        MapPoint innerPoint = pointQueue.front();
        uint16_t innerValue = this->_getDistanceToGoal(innerPoint);

        pointQueue.pop();

        for (int D = 0; D < 8; D++) {
            // TODO make this better
            bool goingDiag = D & 1;

            MapPoint outerPoint = innerPoint + this->_localNeighbors[D];
            uint16_t existingOuterValue = this->_getDistanceToGoal(outerPoint);

            int localDistanceIncrease =
                (goingDiag) ? DIAGONAL_DISTANCE : ORTHOGONAL_DISTANCE;

            int newOuterValue = innerValue + localDistanceIncrease;

            bool outerIsValid = this->_validatePoint(outerPoint);
            bool outerNotBlocked = !(this->_getBlocked(outerPoint));
            bool newOuterIsSmaller = newOuterValue < existingOuterValue;

            if (outerIsValid && outerNotBlocked && newOuterIsSmaller) {
                this->_setDistanceToGoal(outerPoint, newOuterValue);
                pointQueue.push(outerPoint);
            }
        }
    }
    this->_populateDirections();
}

void Map::maybeUpdateAngle(Position robotPosition, Angle* angleToUpdatePtr) {
    MapPoint currentPoint;

    currentPoint.setFromPosition(robotPosition);

    if (this->_getBlocked(currentPoint)) {
        return;
    }
    if (!this->_validatePoint(currentPoint)) {
        return;
    }

    int directionIndex = this->_getDirection(currentPoint);

    Angle angleToDrive = directionIndex * 45;

    *angleToUpdatePtr = angleToDrive;
}

// void Map::print() {
//     for (int y = 0; y < this->_height; y++) {
//         for (int x = 0; x < this->_width; x++) {
//             MapPoint currentPoint = {x, y};
//
//             if (this->_getBlocked(currentPoint)) {
//                 Serial.print("B ");
//             } else {
//                 Serial.print(this->_getDistanceToGoal(currentPoint));
//                 Serial.print(" ");
//             }
//         }
//         Serial.println();
//     }
//     Serial.println();
// }
// void Map::printDir() {
//     for (int y = 0; y < this->_height; y++) {
//         for (int x = 0; x < this->_width; x++) {
//             MapPoint currentPoint = {x, y};
//
//             if (this->_getBlocked(currentPoint)) {
//                 Serial.print("B ");
//             } else {
//                 Serial.print(this->_getDirection(currentPoint));
//                 Serial.print(" ");
//             }
//         }
//         Serial.println();
//     }
//     Serial.println();
// }
//
// void Map::printJank() {
//     for (int y = 0; y < this->_height; y++) {
//         for (int x = 0; x < this->_width; x++) {
//             MapPoint currentPoint = {x, y};
//
//             if (this->_getBlocked(currentPoint)) {
//                 this->_jankyPrintPoint(currentPoint);
//             }
//         }
//         Serial.println();
//     }
//     Serial.println();
// }
//
// void Map::JankyPrintPath(MapPoint startPoint) {
//     MapPoint innerPoint = startPoint;
//
//     bool pointBlocked = this->_getBlocked(innerPoint);
//     bool pointValid = this->_validatePoint(innerPoint);
//
//     bool pointIsGood = pointValid && !pointBlocked;
//
//     while (pointIsGood) {
//         this->_jankyPrintPoint(innerPoint);
//
//         uint8_t direction = this->_getDirection(innerPoint);
//         MapPoint localNextPoint = this->_localNeighbors[direction];
//
//         innerPoint += localNextPoint;
//
//         pointBlocked = this->_getBlocked(innerPoint);
//         pointValid = this->_validatePoint(innerPoint);
//
//         pointIsGood = pointValid && !pointBlocked;
//
//         if (innerPoint == this->_endPoint) {
//             return;
//         };
//     }
// }
//
// void Map::JankyPrintPath2(MapPoint startPoint) {
//     MapPoint innerPoint = startPoint;
//
//     bool pointBlocked = this->_getBlocked(innerPoint);
//     bool pointValid = this->_validatePoint(innerPoint);
//
//     bool pointIsGood = pointValid && !pointBlocked;
//
//     MapPoint lastPoint = innerPoint;
//
//     int distanceTolerance = 10;
//     int squaredDistTolerance = distanceTolerance * distanceTolerance;
//
//     this->_jankyPrintPoint(lastPoint);
//     while (pointIsGood) {
//         uint8_t direction = this->_getDirection(innerPoint);
//         MapPoint localNextPoint = this->_localNeighbors[direction];
//
//         innerPoint += localNextPoint;
//
//         if (innerPoint.squaredDistanceTo(lastPoint) > squaredDistTolerance) {
//             lastPoint = innerPoint;
//             this->_jankyPrintPoint(lastPoint);
//         }
//
//         pointBlocked = this->_getBlocked(innerPoint);
//         pointValid = this->_validatePoint(innerPoint);
//
//         pointIsGood = pointValid && !pointBlocked;
//
//         if (innerPoint == this->_endPoint) {
//             return;
//         };
//     }
// }
//
// void Map::JankyPrintBlockData() {
//     for (int index = 0; index < this->_dimension; index++) {
//         int x = index % this->_width;
//         int y = index / this->_width;
//
//         MapPoint scanPoint = {x, y};
//
//         if (this->_getBlocked(scanPoint)) {
//             this->_jankyPrintPoint(scanPoint, 12);
//         }
//     }
// }

struct __attribute__((packed)) Bundle {
    uint8_t x;
    uint8_t y;
    MapItem data;
};

void Map::sendOverSerial() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = {x, y};

        if (this->_validatePoint(scanPoint)) {
            Bundle binaryBundle;

            binaryBundle.x = x;
            binaryBundle.y = y;
            binaryBundle.data = this->_mapData[y][x];

            uint8_t* dataToSend = (uint8_t*)&binaryBundle;

            Serial.write(dataToSend, sizeof(Bundle));
        }
    }
}

void Map::_populateDirections() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = {x, y};

        // Serial.print(this->_dimension);
        // Serial.print("  ");
        // Serial.print(index);
        // Serial.print("  ");
        // Serial.println(scanPoint.toString());

        if (this->_getBlocked(scanPoint)) {
            continue;
            ;
        }

        // TODO come back and refactor all this.
        uint16_t innerValue = this->_getDistanceToGoal(scanPoint);

        uint16_t LowestValue = innerValue;
        uint8_t lowestDistanceToWall = 0;
        uint8_t directionOfLowest = 0;

        for (uint8_t direction = 0; direction < 8; direction++) {
            MapPoint outerPoint = scanPoint + this->_localNeighbors[direction];

            bool goingDiag = direction & 1;

            int localDistanceIncrease =
                (goingDiag) ? DIAGONAL_DISTANCE : ORTHOGONAL_DISTANCE;

            int directionPathLength =
                this->_getDistanceToGoal(outerPoint) + localDistanceIncrease;

            bool outerIsValid = this->_validatePoint(outerPoint);
            bool outerNotBlocked = !(this->_getBlocked(outerPoint));
            bool newPathEqualOrLower = directionPathLength <= LowestValue;

            // if the outer point is bad, break the loop.
            if (!(outerIsValid && outerNotBlocked)) {
                continue;
            }

            if (newPathEqualOrLower) {
                int newPointDistanceTOWall =
                    this->_getDistanceToWall(outerPoint);
                if (newPointDistanceTOWall > lowestDistanceToWall) {
                    LowestValue = directionPathLength;
                    directionOfLowest = direction;
                    lowestDistanceToWall = this->_getDistanceToWall(outerPoint);
                }
            }
        }
        this->_setDirection(scanPoint, directionOfLowest);
    }
}
bool Map::_validatePoint(MapPoint pointToTest) {
    bool withinLowerXRange = (pointToTest.x >= 0);
    bool withinUpperXRange = (pointToTest.x < this->_width);
    bool withinLowerYRange = (pointToTest.y >= 0);
    bool withinUpperYRange = (pointToTest.y < this->_height);

    bool xCoordIsValid = withinLowerXRange && withinUpperXRange;
    bool yCoordIsValid = withinLowerYRange && withinUpperYRange;

    bool pointIsValid = xCoordIsValid && yCoordIsValid;

    return pointIsValid;
}

bool Map::_getBlocked(MapPoint point) {
    return this->_mapData[point.y][point.x].isBlocked;
}
void Map::_setBlocked(MapPoint point, bool blockedStatus) {
    this->_mapData[point.y][point.x].isBlocked = blockedStatus;
}

uint8_t Map::_getDirection(MapPoint point) {
    return this->_mapData[point.y][point.x].direction;
}
void Map::_setDirection(MapPoint point, uint8_t newDirection) {
    this->_mapData[point.y][point.x].direction = newDirection;
}

uint16_t Map::_getDistanceToGoal(MapPoint point) {
    return this->_mapData[point.y][point.x].distanceToGoal;
}
void Map::_setDistanceToGoal(MapPoint point, uint16_t newValue) {
    this->_mapData[point.y][point.x].distanceToGoal = newValue;
}

uint8_t Map::_getDistanceToWall(MapPoint point) {
    return this->_mapData[point.y][point.x].distanceToWall;
}
void Map::_setDistanceToWall(MapPoint point, uint8_t newIncrease) {
    this->_mapData[point.y][point.x].distanceToWall = newIncrease;
}

// void Map::jankyPrintPosition(Position position, int extra) {
//     MapPoint point = {0, 0};
//
//     point.x = (position.x / 10);
//     point.y = (position.y / 10);
//
//     this->_jankyPrintPoint(point, extra);
// }
//
// void Map::_jankyPrintPoint(MapPoint point, int extra) {
//     Serial.print("        self.coordsToPoint(");
//     Serial.print(point.x);
//     Serial.print(", ");
//     Serial.print(point.y);
//
//     if (extra != -1) {
//         Serial.print(", ");
//         Serial.print(extra);
//     }
//     Serial.println(")");
// }

void Map::_resetData() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = {x, y};

        this->_setBlocked(scanPoint, 0);
        this->_setDirection(scanPoint, 0);
        this->_setDistanceToGoal(scanPoint, UINT12_MAX);
        this->_setDistanceToWall(scanPoint, 0);
    }
}

int Map::_countBlocks() {
    int count = 0;

    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = {x, y};

        if (this->_getBlocked(scanPoint)) {
            count++;
        }
    }
    return count;
}
