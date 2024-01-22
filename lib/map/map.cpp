

#include "map.h"

#include <queue>

#include "angleAndPosition.h"
#include "brick.h"

#define UINT11_MAX (0x7ff)

#define ROBOT_RADIUS 120

#define DIAGONAL_DISTANCE 3
#define ORTHOGONAL_DISTANCE 2

#define SQUARED_ROBOT_RADIUS ROBOT_RADIUS* ROBOT_RADIUS

/**
 * @brief Construct a new Map Point object
 *
 * @param x The x coordinate in centimeters.
 * @param y The y coordinate in centimeters.
 */
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

        MapPoint scanPoint = MapPoint(x, y);
        Position scanPosition = scanPoint.toPosition();

        int lowestZone = 0;

        int lowestDistance =
            brickList.lowestDistance(scanPosition, nullptr, &lowestZone);

        lowestDistance = constrain(lowestDistance, 0, UINT8_MAX);
        bool pointIsBlocked = (lowestDistance < ROBOT_RADIUS);

        int initialDirection = constrain(lowestZone - 1, 0, 7);

        this->_setBlocked(scanPoint, pointIsBlocked);
        this->_setDirection(scanPoint, initialDirection);
        this->_setDistanceToWall(scanPoint, lowestDistance);
        this->_setDistanceToGoal(scanPoint, UINT11_MAX);
    }
}

void Map::_snowPlow2(MapPoint innerPoint) {
    MapPoint circlePoints[120] = {
        {-4, -11}, {-3, -11}, {-2, -11}, {-1, -11}, {0, -11},  {1, -11},
        {2, -11},  {3, -11},  {4, -11},  {-6, -10}, {-5, -10}, {-4, -10},
        {-3, -10}, {-2, -10}, {-1, -10}, {1, -10},  {2, -10},  {3, -10},
        {4, -10},  {5, -10},  {6, -10},  {-7, -9},  {-6, -9},  {-5, -9},
        {5, -9},   {6, -9},   {7, -9},   {-8, -8},  {-7, -8},  {7, -8},
        {8, -8},   {-9, -7},  {-8, -7},  {8, -7},   {9, -7},   {-10, -6},
        {-9, -6},  {9, -6},   {10, -6},  {-10, -5}, {-9, -5},  {9, -5},
        {10, -5},  {-11, -4}, {-10, -4}, {10, -4},  {11, -4},  {-11, -3},
        {-10, -3}, {10, -3},  {11, -3},  {-11, -2}, {-10, -2}, {10, -2},
        {11, -2},  {-11, -1}, {-10, -1}, {10, -1},  {11, -1},  {-11, 0},
        {11, 0},   {-11, 1},  {-10, 1},  {10, 1},   {11, 1},   {-11, 2},
        {-10, 2},  {10, 2},   {11, 2},   {-11, 3},  {-10, 3},  {10, 3},
        {11, 3},   {-11, 4},  {-10, 4},  {10, 4},   {11, 4},   {-10, 5},
        {-9, 5},   {9, 5},    {10, 5},   {-10, 6},  {-9, 6},   {9, 6},
        {10, 6},   {-9, 7},   {-8, 7},   {8, 7},    {9, 7},    {-8, 8},
        {-7, 8},   {7, 8},    {8, 8},    {-7, 9},   {-6, 9},   {-5, 9},
        {5, 9},    {6, 9},    {7, 9},    {-6, 10},  {-5, 10},  {-4, 10},
        {-3, 10},  {-2, 10},  {-1, 10},  {1, 10},   {2, 10},   {3, 10},
        {4, 10},   {5, 10},   {6, 10},   {-4, 11},  {-3, 11},  {-2, 11},
        {-1, 11},  {0, 11},   {1, 11},   {2, 11},   {3, 11},   {4, 11}};

    for (int i = 0; i < 120; i++) {
        MapPoint outerPoint = innerPoint + circlePoints[i];

        bool outerIsValid = this->_validatePoint(outerPoint);

        if (outerIsValid) {
            this->_setBlocked(outerPoint, true);
        }
    }
}

void Map::_primeFromSeen() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;
        MapPoint scanPoint = MapPoint(x, y);

        if (x <= 12) {
            this->_setBlocked(scanPoint, true);
        }
        if (x >= 138) {
            this->_setBlocked(scanPoint, true);
        }
        if (y <= 12) {
            this->_setBlocked(scanPoint, true);
        }
        if (y >= 188) {
            this->_setBlocked(scanPoint, true);
        }

        if (this->_getSeen(scanPoint)) {
            this->_snowPlow2(scanPoint);
        }
    }
}

void Map::solve(BrickList brickList, Position endPosition) {
    MapPoint endPoint;
    endPoint.setFromPosition(endPosition);

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

        for (int direction_I = 0; direction_I < 8; direction_I++) {
            // TODO make this better
            bool goingDiagonal = direction_I & 1;

            MapPoint outerPoint = innerPoint + this->_neighbors[direction_I];
            uint16_t existingOuterValue = this->_getDistanceToGoal(outerPoint);

            int distanceToOuterPoint =
                (goingDiagonal) ? DIAGONAL_DISTANCE : ORTHOGONAL_DISTANCE;

            int newOuterValue = innerValue + distanceToOuterPoint;

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

void Map::solveFromSeen(Position endPosition) {
    MapPoint endPoint;
    endPoint.setFromPosition(endPosition);

    this->_primeFromSeen();

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

        for (int direction_I = 0; direction_I < 8; direction_I++) {
            // TODO make this better
            bool goingDiagonal = direction_I & 1;

            MapPoint outerPoint = innerPoint + this->_neighbors[direction_I];
            uint16_t existingOuterValue = this->_getDistanceToGoal(outerPoint);

            int distanceToOuterPoint =
                (goingDiagonal) ? DIAGONAL_DISTANCE : ORTHOGONAL_DISTANCE;

            int newOuterValue = innerValue + distanceToOuterPoint;

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

void Map::getAngle(Position robotPosition, Angle* angleToUpdate_P) {
    MapPoint robotPoint;
    robotPoint.setFromPosition(robotPosition);

    if (this->_getBlocked(robotPoint)) {
        return;
    }
    if (!this->_validatePoint(robotPoint)) {
        return;
    }

    int direction_I = this->_getDirection(robotPoint);

    *angleToUpdate_P = direction_I * 45;
}

float Map::getEuclideanDistanceToEnd(Position robotPosition) {
    MapPoint robotPoint;
    robotPoint.setFromPosition(robotPosition);

    int squaredDistanceInCentimeters =
        robotPoint.squaredDistanceTo(this->_endPoint);

    //
    return sqrt(squaredDistanceInCentimeters) * 10;
}

struct __attribute__((packed)) Bundle {
    uint8_t x;
    uint8_t y;
    MapItem data;
};

void Map::sendOverSerial() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = MapPoint(x, y);

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

void Map::primeForTracking() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = MapPoint(x, y);

        this->_setBeen(scanPoint, false);
        this->_setSeen(scanPoint, false);
    }
}

void Map::TEMP_cutMagicLine() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;
        MapPoint scanPoint = MapPoint(x, y);

        if ((y > 80) && (y < 90)) {
            this->_setBlocked(scanPoint, false);
        }
        if ((x > 60) && (x < 70)) {
            this->_setBlocked(scanPoint, false);
        }
    }
}

void Map::snowPlow(Position robotPosition) {
    MapPoint robotCentrePoint;
    robotCentrePoint.setFromPosition(robotPosition);

    const int circlePointCount = 8;

    const MapPoint circlePoints[circlePointCount]{{10, 0},  {-10, 0}, {0, 10},
                                                  {0, -10}, {7, 7},   {-7, 7},
                                                  {7, -7},  {-7, -7}};

    for (int i = 0; i < circlePointCount; i++) {
        MapPoint iterativeCirclePoint = robotCentrePoint + circlePoints[i];

        if (this->_validatePoint(iterativeCirclePoint)) {
            this->_setBeen(iterativeCirclePoint, true);
        }
    }
}

bool Map::safeForBrick(Position positionA, Position positionB,
                       bool* hitWall_P) {
    bool hitWall_L = false;

    MapPoint pointA;
    pointA.setFromPosition(positionA);

    MapPoint pointB;
    pointB.setFromPosition(positionB);

    int minX = min(pointA.x, pointB.x);
    int maxX = max(pointA.x, pointB.x);

    int minY = min(pointA.y, pointB.y);
    int maxY = max(pointA.y, pointB.y);

    bool safeForBrick = true;

    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            MapPoint scanPoint = MapPoint(x, y);

            bool pointOutOfRange = !this->_validatePoint(scanPoint);
            bool hasBeenToPoint = this->_getBeen(scanPoint);

            if (pointOutOfRange) {
                hitWall_L = true;
            }

            if (pointOutOfRange || hasBeenToPoint) {
                safeForBrick = false;
                return safeForBrick;
            }
        }
    }

    *hitWall_P = hitWall_L;

    return safeForBrick;
}

void Map::seenPosition(Position seenPosition) {
    MapPoint seenPoint;
    seenPoint.setFromPosition(seenPosition);

    this->_setSeen(seenPoint, true);
}

// TODO refactor this function
void Map::_populateDirections() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = MapPoint(x, y);

        if (this->_getBlocked(scanPoint)) {
            continue;
        }

        // TODO come back and refactor all this.
        uint16_t innerValue = this->_getDistanceToGoal(scanPoint);

        uint16_t LowestValue = innerValue;
        uint8_t lowestDistanceToWall = 0;
        uint16_t lowestSquaredDistToEnd =
            scanPoint.squaredDistanceTo(this->_endPoint);
        uint8_t directionOfLowest = 0;

        for (uint8_t direction_I = 0; direction_I < 8; direction_I++) {
            MapPoint outerPoint = scanPoint + this->_neighbors[direction_I];

            bool goingDiagonal = direction_I & 1;

            int distanceToOuterPoint =
                (goingDiagonal) ? DIAGONAL_DISTANCE : ORTHOGONAL_DISTANCE;

            int directionPathLength =
                this->_getDistanceToGoal(outerPoint) + distanceToOuterPoint;

            bool outerIsValid = this->_validatePoint(outerPoint);
            bool outerNotBlocked = !(this->_getBlocked(outerPoint));
            bool newPathEqualOrLower = directionPathLength <= LowestValue;

            // if the outer point is bad, break the loop.
            if (!(outerIsValid && outerNotBlocked)) {
                continue;
            }

            // TODO refactor this when I have more brain power.
            if (newPathEqualOrLower) {
                int newPointDistanceToWall =
                    this->_getDistanceToWall(outerPoint);
                if (newPointDistanceToWall > lowestDistanceToWall) {
                    LowestValue = directionPathLength;
                    directionOfLowest = direction_I;
                    lowestDistanceToWall = this->_getDistanceToWall(outerPoint);
                    lowestSquaredDistToEnd =
                        scanPoint.squaredDistanceTo(this->_endPoint);
                } else if (newPointDistanceToWall == lowestDistanceToWall) {
                    int newSquaredDistToEnd =
                        outerPoint.squaredDistanceTo(this->_endPoint);

                    if (newSquaredDistToEnd < lowestSquaredDistToEnd) {
                        LowestValue = directionPathLength;
                        directionOfLowest = direction_I;
                        lowestDistanceToWall =
                            this->_getDistanceToWall(outerPoint);
                        lowestSquaredDistToEnd =
                            scanPoint.squaredDistanceTo(this->_endPoint);
                    }
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

bool Map::_getBeen(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].been;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
        return 0;
    }
}
void Map::_setBeen(MapPoint point, bool beenStatus) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].been = beenStatus;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
    }
}

bool Map::_getBlocked(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].blocked;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
        return 0;
    }
}
void Map::_setBlocked(MapPoint point, bool blockedStatus) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].blocked = blockedStatus;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
    }
}

uint8_t Map::_getDirection(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].direction;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
        return 0;
    }
}
void Map::_setDirection(MapPoint point, uint8_t newDirection) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].direction = newDirection;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
    }
}

uint16_t Map::_getDistanceToGoal(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].distanceToGoal;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
        return 0;
    }
}
void Map::_setDistanceToGoal(MapPoint point, uint16_t newValue) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].distanceToGoal = newValue;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
    }
}

uint8_t Map::_getDistanceToWall(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].distanceToWall;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
        return 0;
    }
}
void Map::_setDistanceToWall(MapPoint point, uint8_t newIncrease) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].distanceToWall = newIncrease;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
    }
}

bool Map::_getSeen(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].seen;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
        return 0;
    }
}
void Map::_setSeen(MapPoint point, bool seenStatus) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].seen = seenStatus;
    } else {
        Serial.println("Out of range");  // TODO handle this error properly.
    }
}

void Map::_resetData() {
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = {x, y};

        this->_setBeen(scanPoint, 0);
        this->_setBlocked(scanPoint, 0);
        this->_setDirection(scanPoint, 0);
        this->_setDistanceToGoal(scanPoint, UINT11_MAX);
        this->_setDistanceToWall(scanPoint, 0);
    }
}
