/**
 * @file map.cpp
 * @brief Definitions of the Mapping objects, MapPoint, MapItem and Map,
 * responsible for implementing a modified version of a flood fill algorithm.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-01-19
 * @copyright Copyright (c) 2024
 */

#include "map.h"

#include <queue>

#include "angleAndPosition.h"
#include "brick.h"
#include "errorIndicator.h"

/**
 * @brief The maximum value of an 11 bit unsigned integer, equivalent to 2047
 */
#define UINT11_MAX (0x7ff)

#define ROBOT_RADIUS 120

// 2 and 3 are roughly proportional to 1 and sqrt(2), while remaining integers.
#define ORTHOGONAL_DISTANCE 2
#define DIAGONAL_DISTANCE 3

/**
 * @brief Construct a new Map Point object
 *
 * @param x The x coordinate in centimeters.
 * @param y The y coordinate in centimeters.
 */
MapPoint::MapPoint(int x, int y) : x(x), y(y) {}

/**
 * @brief Gets the square of the euclidean distance in centimeters, from the
 * current point to another.
 *
 * @param otherPoint The point to measure the squared distance to.
 * @return (int) The square of the euclidean distance in centimeters
 */
int MapPoint::squaredDistanceTo(MapPoint otherPoint) {
    int dx = abs(this->x - otherPoint.x);
    int dy = abs(this->y - otherPoint.y);

    return dx * dx + dy * dy;
}

/**
 * @brief Creates a new instance of the Position struct, based on the value
 * of this Point.
 *
 * The x and y values of the Position struct are stored in millimeters.
 *
 * @return (Position) The newly created position.
 */
Position MapPoint::createPosition() const {
    float xValueInMM = this->x * 10.0f;
    float yValueInMM = this->y * 10.0f;

    return Position(xValueInMM, yValueInMM);
}

/**
 * @brief Sets the value of the current point based on a provided Position.
 *
 * The x and y values of the Position struct are stored in millimeters,
 * where as the x and y values of the Point are stored in centimeters.
 *
 * @param position The position to update the x and y values from.
 */
void MapPoint::setFromPosition(Position position) {
    int xValueInCM = position.x / 10.0f;
    int yValueInCM = position.y / 10.0f;

    this->x = xValueInCM;
    this->y = yValueInCM;
}

/**
 * @brief Converts the current MapPoint into a human readable string.
 *
 * @return (String) The current MapPoint as a human readable string.
 */
String MapPoint::toString() const {
    String stringToReturn = "MP:(";
    stringToReturn += this->x;
    stringToReturn += ",";
    stringToReturn += this->y;
    stringToReturn += ")";

    return stringToReturn;
}

/**
 * @brief Overloaded addition operator, used to add two MapPoints together
 * by summing their x and y components.
 * @param PointToAdd The MapPoint on the right hand side of the operator.
 * @return The MapPoint created by summing the two provided MapPoints.
 */
MapPoint MapPoint::operator+(const MapPoint& PointToAdd) const {
    return MapPoint(this->x + PointToAdd.x, this->y + PointToAdd.y);
}

/**
 * @brief Overloaded compound addition operator, used to add a MapPoint to
 * the current MapPoint by summing their x and y components.
 *
 * @param PointToAdd The MapPoint to add to the current Position.
 * @return reference to the modified MapPoint.
 */
MapPoint& MapPoint::operator+=(const MapPoint& PointToAdd) {
    this->x += PointToAdd.x;
    this->y += PointToAdd.y;

    return *this;
}

/**
 * @brief Overloaded equality operator, used to compare two MapPoints by
 * comparing their x and y components.
 * Tow points are considered equal if both their x and y values match.
 *
 * @param PointToCompare The MapPoints to compare to the current MapPoints.
 * @return (true) If the MapPoints are equal.
 * @return (false) If the MapPoints are unequal.
 */
bool MapPoint::operator==(const MapPoint& PointToCompare) const {
    return ((this->x == PointToCompare.x) && (this->y == PointToCompare.y));
}

/**
 * @brief Construct a new Map object and resets its data.
 */
Map::Map() {
    // Reset the data in the map.
    this->_resetData();
}

/**
 * @brief Plots out a rough outline of the robot onto the "been" layer of
 * the map.
 *
 * @param robotPosition The position of the robot.
 */
void Map::plotVisitedPointsOnMap(Position robotPosition) {
    // Create a MapPoint based on the given robot Position.
    MapPoint robotCentrePoint;
    robotCentrePoint.setFromPosition(robotPosition);

    const int circlePointCount = 9;

    // draws a rough outline of a 10 cm radius circle around the robot on the
    // map.
    const MapPoint circlePoints[circlePointCount]{{10, 0},  {-10, 0}, {0, 10},
                                                  {0, -10}, {0, 0},   {7, 7},
                                                  {-7, 7},  {7, -7},  {-7, -7}};

    // Iterate over the circle Points, relative to the robots current position.
    for (int i = 0; i < circlePointCount; i++) {
        MapPoint iterativeCirclePoint = robotCentrePoint + circlePoints[i];

        // If the point is a valid point on the map,
        if (this->_validatePoint(iterativeCirclePoint)) {
            // set the "been" feild of that point on the map to true.
            this->_setBeen(iterativeCirclePoint, true);
        }
    }
}

/**
 * @brief Increments the value of the seen feild of the map at a given
 * Position.
 *
 * @param seenPosition The Position that has been seen by a sensor.
 */
void Map::incrementSeenPosition(Position seenPosition) {
    // Create a MapPoint based on the given seen Position.
    MapPoint seenPoint;
    seenPoint.setFromPosition(seenPosition);

    // If the given position is not on the map, return immediately.
    // This should not break the program, as the seen feild is only used to
    // loosely track the points that the robot has measured with it's sensors.
    // Occasionally, the robot may measure a point that is out of bounds of the
    // map. This is not cause for concern.
    if (!this->_validatePoint(seenPoint)) {
        return;
    }

    // If the given position has been seen less times than its max value,
    // increment its count.
    int currentSeenCount = this->_getSeen(seenPoint);
    if (currentSeenCount < UINT8_MAX) {
        this->_setSeen(seenPoint, currentSeenCount + 1);
    };
}

/**
 * @brief Checks if an area (the zone between the two provided points) is
 * safe to place a brick into. This is determined based on, if the area is
 * fully on the map and if the robot has already visited any of the points
 * within the area.
 *
 * The area is only valid is the robot has not visited any of the points
 * within it.
 *
 * @param positionA The bottom left Position of the area.
 * @param positionB The top right Position of the area.
 * @return (-1) If the area falls out of bounds of the map.
 * @return (0) If the area is in bounds but a brick cannot be placed.
 * @return (1) If the area is safe to place a brick.
 */
int Map::checkIfSafeForBrick(Position positionA, Position positionB) {
    // TODO use this function to test if a brick can be added to the list.
    MapPoint pointA;
    pointA.setFromPosition(positionA);

    MapPoint pointB;
    pointB.setFromPosition(positionB);

    int minX = min(pointA.x, pointB.x);
    int maxX = max(pointA.x, pointB.x);

    int minY = min(pointA.y, pointB.y);
    int maxY = max(pointA.y, pointB.y);

    bool robotHasBeenInArea = false;
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            MapPoint scanPoint = MapPoint(x, y);

            bool pointOutOfRange = !this->_validatePoint(scanPoint);

            // If any of the points in ther area are invalid return -1
            // immediately.
            if (pointOutOfRange) {
                return -1;
            }

            // If the robot has visited any point within the area, raise the
            // robotHasBeenInArea boolean, but keep iterating through the area
            // to check if any points fall out of bounds.
            if (!robotHasBeenInArea) {
                robotHasBeenInArea = this->_getBeen(scanPoint);
            }
        }
    }

    // If the robot has visited any of the points in the area, return zero to
    // signify that a brick cannot be placed.
    if (robotHasBeenInArea) {
        return 0;
    }

    // return 1 to signify that a brick can be placed in this zone.
    return 1;
}

/**
 * @brief Solves the Map, calculating the distance from evert point to the
 * provided endPosition. Followed by calculating the direction to drive at
 * each point on the map, so that both these values can just be read from
 * memory when needed, instead of being calculated on the fly every time.
 *
 * @param brickList The BrickList to plot out on the Map.
 * @param endPosition The Position that the Map should lead to.
 */
void Map::solve(BrickList brickList, Position endPosition) {
    // Create a MapPoint based on the given end Position.
    MapPoint endPoint;
    endPoint.setFromPosition(endPosition);

    // Set up the initial values based on the bricks in the proved BrickList.
    // Primarily setting every value on the map to max value.
    this->_primeFromBrickList(brickList);

    // IF the end point is invalid,
    if (!_validatePoint(endPoint)) {
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__,
                                       "Point out of bounds.");
    }

    // Start of the flood fill implementation

    // Create the queue to store the next points to iterate through.
    // TODO Make this more robust to reduce the dependance of dynamic memory
    // allocation.
    std::queue<MapPoint> pointQueue;
    pointQueue.push(endPoint);

    // Set the end point to 0 distance
    this->_setDistanceToGoal(endPoint, 0);

    // store the _endPoint as a class method.
    this->_endPoint = endPoint;

    // While the queue still has items in it.
    while (!pointQueue.empty()) {
        // Get the inner point, the current point that is being operated on, as
        // opposed to the outer point, which refers to one of the 8 points
        // surrounding the inner point.
        MapPoint innerPoint = pointQueue.front();
        uint16_t innerValue = this->_getDistanceToGoal(innerPoint);

        // remove the innerPoint from the queue.
        pointQueue.pop();

        // Iterate thought the 8 outer points.
        for (int direction_I = 0; direction_I < 8; direction_I++) {
            MapPoint outerPoint = innerPoint + this->_neighbors[direction_I];

            // TODO re-document.

            // If the outer point is not on the map, move onto the next point.
            if (!(this->_validatePoint(outerPoint))) {
                continue;
            }

            uint16_t existingOuterValue = this->_getDistanceToGoal(outerPoint);

            // If the outer Point is diagonal, increase it by DIAGONAL_DISTANCE,
            // if it's orthogonal increase it ORTHOGONAL_DISTANCE
            int distanceToOuterPoint =
                (direction_I & 1) ? DIAGONAL_DISTANCE : ORTHOGONAL_DISTANCE;

            int newOuterValue = innerValue + distanceToOuterPoint;

            bool outerNotBlocked = !(this->_getBlocked(outerPoint));
            bool newOuterIsSmaller = newOuterValue < existingOuterValue;

            // If the outer point is valid, is not blocked and is smaller than
            // the current value stored in that cell, set the outer cell's value
            // to the new calculated value and push the outer cell to the queue.
            if (outerNotBlocked && newOuterIsSmaller) {
                this->_setDistanceToGoal(outerPoint, newOuterValue);
                pointQueue.push(outerPoint);
            }
        }
    }

    // After the distances in the map have all been solved, set all the
    // directions.
    this->_populateDirections();
}

/**
 * @brief Calculates the euclidean distance to the end point of the current
 * map.
 *
 * @param robotPosition TThe position of the robot.
 * @return (float) The distance in millimeters that the robot is from the
 * end point of the map.
 */
float Map::getCrowDistanceToEnd(Position robotPosition) {
    MapPoint endPoint = this->_endPoint;

    Position endPosition = endPoint.createPosition();

    return robotPosition.distanceTo(endPosition);
}

/**
 * @brief Update via reference, the direction to drive, based on the
 * current position of the robot.
 *
 * @param robotPosition The current position of the robot.
 * @param angleToUpdate_P The pointer used to update the angle to drive.
 */
void Map::updateAngleToDrive(Position robotPosition, Angle* angleToUpdate_P) {
    // Create a MapPoint based on the given robot Position.
    MapPoint robotPoint;
    robotPoint.setFromPosition(robotPosition);

    // If the robotPosition is off the map, return nothing.
    if (!this->_validatePoint(robotPoint)) {
        return;
    }

    // Return by reference, the direction to drive at the current point.
    int direction_I = this->_getDirection(robotPoint);
    *angleToUpdate_P = direction_I * 45;
}

/**
 * @brief Send the entire contents of the Map over the serial port.
 */
void Map::sendOverSerial() {
    // Create a new stuck to bundle the x and y position of a MapItem, along
    // with the data of the map item.
    struct __attribute__((packed)) Bundle {
        uint8_t x;
        uint8_t y;
        MapItem data;
    };

    // Iterate through every point on the map.
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        Bundle bundledMapItem;
        bundledMapItem.x = x;
        bundledMapItem.y = y;
        bundledMapItem.data = this->_mapData[y][x];

        uint8_t* dataToSend = (uint8_t*)&bundledMapItem;

        // Write the current struct to the serial port.
        Serial.write(dataToSend, sizeof(Bundle));
    }
}

/**
 * @brief Iterates through each point in the map and calculates which
 * direction should be taken to reach the goal.
 */
void Map::_populateDirections() {
    // Iterate through every point on the map.
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = MapPoint(x, y);

        // If the point is blocked, skip this iteration.
        if (this->_getBlocked(scanPoint)) {
            continue;
        }

        // Initialize the variables required to calculate the direction to go,
        uint16_t innerValue = this->_getDistanceToGoal(scanPoint);
        uint16_t LowestValue = innerValue;
        uint8_t lowestDistanceToWall = 0;
        uint16_t lowestSquaredDistToEnd =
            scanPoint.squaredDistanceTo(this->_endPoint);
        uint8_t directionOfLowest = 0;

        // Iterate through each of the 8 outer points.
        for (uint8_t direction_I = 0; direction_I < 8; direction_I++) {
            MapPoint outerPoint = scanPoint + this->_neighbors[direction_I];

            // If the outer point is not on the map, move onto the next point.
            if (!(this->_validatePoint(outerPoint))) {
                continue;
            }

            // If the outer point blocked by a brick, move onto the next point.
            if (this->_getBlocked(outerPoint)) {
                continue;
            }

            // If the outer Point is diagonal, increase it by DIAGONAL_DISTANCE,
            // if it's orthogonal increase it ORTHOGONAL_DISTANCE
            int distanceToOuterPoint =
                (direction_I & 1) ? DIAGONAL_DISTANCE : ORTHOGONAL_DISTANCE;

            int outerPointPathLength =
                this->_getDistanceToGoal(outerPoint) + distanceToOuterPoint;

            // Ihe new path is lower than the current lowest,
            if (outerPointPathLength <= LowestValue) {
                // read how close the current outer point is to the closes
                // wall.
                int newPointDistanceToWall =
                    this->_getDistanceToWall(outerPoint);

                // If the new point if further from the wall than the last outer
                // point,

                if (newPointDistanceToWall > lowestDistanceToWall) {
                    // store the new lowest direction, and the other values to
                    // go with it.
                    directionOfLowest = direction_I;
                    LowestValue = outerPointPathLength;
                    lowestDistanceToWall = this->_getDistanceToWall(outerPoint);
                    lowestSquaredDistToEnd =
                        scanPoint.squaredDistanceTo(this->_endPoint);

                    // If the new outer point is the same distance to a wall as
                    // existing lowest direction,
                } else if (newPointDistanceToWall == lowestDistanceToWall) {
                    int newSquaredDistToEnd =
                        outerPoint.squaredDistanceTo(this->_endPoint);

                    if (newSquaredDistToEnd < lowestSquaredDistToEnd) {
                        // then go with whichever point is closer to the goal.
                        LowestValue = outerPointPathLength;
                        directionOfLowest = direction_I;
                        lowestDistanceToWall =
                            this->_getDistanceToWall(outerPoint);
                        lowestSquaredDistToEnd =
                            scanPoint.squaredDistanceTo(this->_endPoint);
                    }
                }
            }
        }

        // After iterating through the 8 outer points, set the direction to the
        // one that provided the most optimal route.
        this->_setDirection(scanPoint, directionOfLowest);
    }
}

/**
 * @brief Takes a given point and test whether it is with the bounds of the
 * map.
 *
 * @param pointToTest The point to check against the map.
 * @return (true) If the point within the bounds of the map.
 * @return (false) If the point is not within the bounds of the map.
 */
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

/**
 * @brief Sets up the map data ready to solve the maze. This is done by,
 * checking if each point is blocked, setting the inital direction to move
 * away from the walls, setting the distance to the wall value, and
 * setting the distance to the goal to a maximum value.
 *
 * @param brickList The BrickList to base the inital map data on.
 */
void Map::_primeFromBrickList(BrickList brickList) {
    // Iterate through every point on the map.
    for (int index = 0; index < this->_dimension; index++) {
        int x = index % this->_width;
        int y = index / this->_width;

        MapPoint scanPoint = MapPoint(x, y);
        Position scanPosition = scanPoint.createPosition();

        int lowestZone_I = 0;

        // Gets the distance to the closest brick, along with the index of the
        // zone from the brick.
        int lowestDistance =
            brickList.lowestDistance(scanPosition, nullptr, &lowestZone_I);

        lowestDistance = constrain(lowestDistance, 0, UINT8_MAX);

        // The point is considered blocked if the closest brick is closer than
        // the radius of a robot, this could cause a collision.
        bool pointIsBlocked = (lowestDistance < ROBOT_RADIUS);

        // The lowest zone index is used to calculate which direction to drive
        // to move away from teh closest brick.
        int initialDirection = constrain(lowestZone_I - 1, 0, 7);

        this->_setBlocked(scanPoint, pointIsBlocked);
        this->_setDirection(scanPoint, initialDirection);
        this->_setDistanceToWall(scanPoint, lowestDistance);
        this->_setDistanceToGoal(scanPoint, UINT11_MAX);
    }
}

/**
 * @brief Gets the "been" value at given point on the map.
 *
 * @param point The given point of the map.
 * @return (true)  If the robot has been to the given point.
 * @return (false) If the robot has not been to the given point.
 */
bool Map::_getBeen(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].been;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
        return 0;
    }
}

/**
 * @brief Sets the "been" value at given point on the map.
 *
 * @param point The given point of the map.
 * @param beenStatus The value to set.
 */
void Map::_setBeen(MapPoint point, bool beenStatus) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].been = beenStatus;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
    }
}

/**
 * @brief Gets the "blocked" value at given point on the map.
 *
 * @param point The given point of the map.
 * @return (true)  If the point is blocked.
 * @return (false) If the robot is not blocked.
 */
bool Map::_getBlocked(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].blocked;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
        return 0;
    }
}

/**
 * @brief Sets the "blocked" value at given point on the map.
 *
 * @param point The given point of the map.
 * @param blockedStatus The value to set.
 */
void Map::_setBlocked(MapPoint point, bool blockedStatus) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].blocked = blockedStatus;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
    }
}

/**
 * @brief Gets the "direction" value at given point on the map.
 *
 * @param point The given point of the map.
 * @return (uint8_t) The index of the direction to go at the given point.
 */
uint8_t Map::_getDirection(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].direction;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
        return 0;
    }
}

/**
 * @brief Sets the "direction" value at given point on the map.
 *
 * @param point The given point of the map.
 * @param newDirection The value to set.
 */
void Map::_setDirection(MapPoint point, uint8_t newDirection) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].direction = newDirection;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
    }
}

/**
 * @brief Gets the "distanceToGoal" value at given point on the map.
 *
 * @param point The given point of the map.
 * @return (uint16_t) The distance to the goal, via the shortest path.
 */
uint16_t Map::_getDistanceToGoal(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].distanceToGoal;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
        return 0;
    }
}

/**
 * @brief Sets the "distanceToGoal" value at given point on the map.
 *
 * @param point The given point of the map.
 * @param newDistance The value to set.
 */
void Map::_setDistanceToGoal(MapPoint point, uint16_t newDistance) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].distanceToGoal = newDistance;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
    }
}

/**
 * @brief Gets the "distanceToWall" value at given point on the map.
 *
 * @param point The given point of the map.
 * @return (uint8_t) The distance to the closest wall.
 */
uint8_t Map::_getDistanceToWall(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].distanceToWall;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
        return 0;
    }
}

/**
 * @brief Sets the "distanceToWall" value at given point on the map.
 *
 * @param point The given point of the map.
 * @param newDistanceToWall The distance to the closest wall.
 */
void Map::_setDistanceToWall(MapPoint point, uint8_t newDistanceToWall) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].distanceToWall = newDistanceToWall;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
    }
}

/**
 * @brief Gets the "seen" value at given point on the map.
 *
 * @param point The given point of the map.
 * @return (uint8_t) Then number of times the given position has been
 * seen by a sensor.
 */
uint8_t Map::_getSeen(MapPoint point) {
    if (this->_validatePoint(point)) {
        return this->_mapData[point.y][point.x].seen;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
        return 0;
    }
}

/**
 * @brief Sets the "seen" value at given point on the map.
 *
 * @param point The given point of the map.
 * @param seenOccurrence Then number of times the given position has been
 * seen by a sensor.
 */
void Map::_setSeen(MapPoint point, uint8_t seenOccurrence) {
    if (this->_validatePoint(point)) {
        this->_mapData[point.y][point.x].seen = seenOccurrence;
    } else {
        String errorMessage = "";
        errorMessage += point.toString();
        errorMessage += " is out of range.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, errorMessage);
    }
}

/**
 * @brief Resets all the data in a Map by setting evert value of every layer
 * to 0, aside from the distanceToGoal layer, in which evert value is set
 * to max.
 */
void Map::_resetData() {
    // Iterate through every point on the map.
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
