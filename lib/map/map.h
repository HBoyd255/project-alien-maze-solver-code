/**
 * @file map.h
 * @brief Declarations of the Mapping objects, MapPoint, MapItem and Map,
 * responsible for implementing a modified version of a flood fill algorithm.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-01-19
 * @copyright Copyright (c) 2024
 */
#ifndef MAP_H
#define MAP_H

#include <Arduino.h>

/**
 * @brief The number of rows in the map, equal to the hight of the map in
 * centimeters.
 */
#define MAP_HEIGHT_CM 200
/**
 * @brief The number of columns in the map, equal to the width of the map in
 * centimeters.
 */
#define MAP_WIDTH_CM 150

// Forwards declaration of BrickList, Position and Angle class.
class BrickList;
class Position;
class Angle;

/**
 * @brief Stores x and y positions in centimeters, as integers.
 *
 * Primarily used to stores the 2d coordinates of the items in map array.
 */
struct MapPoint {
    /**
     * @brief The x coordinate of a point in centimeters.
     */
    int x;

    /**
     * @brief The y coordinate of a point in centimeters.
     */
    int y;

    /**
     * @brief Construct a new Map Point object
     *
     * @param x The x coordinate in centimeters.
     * @param y The y coordinate in centimeters.
     */
    MapPoint(int x = 0, int y = 0);

    /**
     * @brief Gets the square of the euclidean distance in centimeters, from the
     * current MapPoint to another.
     *
     * @param otherPoint The point to measure the squared distance to.
     * @return (int) The square of the euclidean distance in centimeters
     */
    int squaredDistanceTo(MapPoint otherPoint);

    /**
     * @brief Creates a new instance of the Position struct, based on the value
     * of this Point.
     *
     * The x and y values of the Position struct are stored in millimeters.
     * where as the x and y values of the MapPoint are stored in centimeters.
     *
     * @return (Position) The newly created position.
     */
    Position createPosition() const;

    /**
     * @brief Sets the value of the current MapPoint based on a provided
     * Position.
     *
     * The x and y values of the Position struct are stored in millimeters,
     * where as the x and y values of the MapPoint are stored in centimeters.
     *
     * @param position The position to update the x and y values from.
     */
    void setFromPosition(Position position);

    /**
     * @brief Converts the current MapPoint into a human readable string.
     *
     * @return (String) The current MapPoint as a human readable string.
     */
    String toString() const;

    /**
     * @brief Overloaded addition operator, used to add two MapPoints together
     * by summing their x and y components.
     * @param PointToAdd The MapPoint on the right hand side of the operator.
     * @return The MapPoint created by summing the two provided MapPoints.
     */
    MapPoint operator+(const MapPoint& PointToAdd) const;

    /**
     * @brief Overloaded compound addition operator, used to add a MapPoint to
     * the current MapPoint by summing their x and y components.
     *
     * @param PointToAdd The MapPoint to add to the current Position.
     * @return reference to the modified MapPoint.
     */
    MapPoint& operator+=(const MapPoint& PointToAdd);

    /**
     * @brief Overloaded equality operator, used to compare two MapPoints by
     * comparing their x and y components.
     * Tow points are considered equal if both their x and y values match.
     *
     * @param PointToCompare The MapPoints to compare to the current MapPoints.
     * @return (true) If the MapPoints are equal.
     * @return (false) If the MapPoints are unequal.
     */
    bool operator==(const MapPoint& PointToCompare) const;
};

/**
 * @brief The bitfield struct that the makes up the cells in the map.
 */
struct __attribute__((packed)) MapItem {
    /**
     * @brief A boolean keeping track of if the robot has been to the current
     * cell.
     */
    unsigned int been : 1;

    /**
     * @brief A boolean keeping track if current cell is blocked.
     */
    unsigned int blocked : 1;

    /**
     * @brief The direction to drive from a given point, 0 is right and
     * each increment rotated clockwise, 1 is top right, 2 is up and so on.
     * Stored as a 3 bit unsigned integer, 0 - 8,
     */
    unsigned int direction : 3;

    /**
     * @brief The distance to the goal from the current cell, via a calculated
     * path,not in centimeter, but still linear, as an 11 bit unsigned integer,
     * 0 - 2047.
     */
    unsigned int distanceToGoal : 11;

    /**
     * @brief The distance to the closest wall from the current cell, in
     * centimeters, as an 8 bit unsigned integer, 0 - 255.
     */
    unsigned int distanceToWall : 8;

    /**
     * @brief how mant times a cell has been seen by a sensor , as an 8 bit
     * unsigned integer, 0 - 255.
     */
    unsigned int seen : 8;
};

class Map {
   public:
    /**
     * @brief Construct a new Map object and resets its data.
     */
    Map();

    /**
     * @brief Plots out a rough outline of the robot onto the "been" layer of
     * the map.
     *
     * @param robotPosition The position of the robot.
     */
    void plotVisitedPointsOnMap(Position robotPosition);

    /**
     * @brief Increments the value of the seen feild of the map at a given
     * Position.
     *
     * @param seenPosition The Position that has been seen by a sensor.
     */
    void seenPosition(Position seenPosition);

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
    int checkIfSafeForBrick(Position positionA, Position positionB);

    /**
     * @brief Solves the Map, calculating the distance from evert point to the
     * provided endPosition. Followed by calculating the direction to drive at
     * each point on the map, so that both these values can just be read from
     * memory when needed, instead of being calculated on the fly every time.
     *
     * @param brickList The BrickList to plot out on the Map.
     * @param endPosition The Position that the Map should lead to.
     */
    void solve(BrickList brickList, Position endPosition);

    /**
     * @brief Calculates the euclidean distance to the end point of the current
     * map.
     *
     * @param robotPosition TThe position of the robot.
     * @return (float) The distance in millimeters that the robot is from the
     * end point of the map.
     */
    float getCrowDistanceToEnd(Position robotPosition);

    /**
     * @brief Update via reference, the direction to drive, based on the
     * current position of the robot.
     *
     * @param robotPosition The current position of the robot.
     * @param angleToUpdate_P The pointer used to update the angle to drive.
     */
    void updateAngleToDrive(Position robotPosition, Angle* angleToUpdate_P);

    /**
     * @brief Send the entire contents of the Map over the serial port.
     */
    void sendOverSerial();

   private:
    /**
     * @brief 2D array for storing the contents of the map data.
     */
    MapItem _mapData[MAP_HEIGHT_CM][MAP_WIDTH_CM];

    /**
     * @brief The most recently requested endpoint that the Map should lead to.
     */
    MapPoint _endPoint = MapPoint(0, 0);

    /**
     * @brief The width of the map in centimeters.
     */
    const int _width = MAP_WIDTH_CM;

    /**
     * @brief The height of the map in centimeters.
     */

    const int _height = MAP_HEIGHT_CM;

    /**
     * @brief The number of items in the map.
     */
    const int _dimension = MAP_WIDTH_CM * MAP_HEIGHT_CM;

    /**
     * @brief The local position of the 8 outer points, relative to the inner
     * point
     */
    const MapPoint _neighbors[8] = {{1, 0},  {1, 1},   {0, 1},  {-1, 1},
                                    {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

    /**
     * @brief Iterates through each point in the map and calculates which
     * direction should be taken to reach the goal.
     */
    void _populateDirections();

    /**
     * @brief Takes a given point and test whether it is with the bounds of the
     * map.
     *
     * @param pointToTest The point to check against the map.
     * @return (true) If the point within the bounds of the map.
     * @return (false) If the point is not within the bounds of the map.
     */
    bool _validatePoint(MapPoint pointToTest);

    /**
     * @brief Sets up the map data ready to solve the maze. This is done by,
     * checking if each point is blocked, setting the inital direction to move
     * away from the walls, setting the distance to the wall value, and
     * setting the distance to the goal to a maximum value.
     *
     * @param brickList The BrickList to base the inital map data on.
     */
    void _primeFromBrickList(BrickList brickList);

    /**
     * @brief Gets the "been" value at given point on the map.
     *
     * @param point The given point of the map.
     * @return (true)  If the robot has been to the given point.
     * @return (false) If the robot has not been to the given point.
     */
    bool _getBeen(MapPoint point);

    /**
     * @brief Sets the "been" value at given point on the map.
     *
     * @param point The given point of the map.
     * @param beenStatus The value to set.
     */
    void _setBeen(MapPoint point, bool beenStatus);

    /**
     * @brief Gets the "blocked" value at given point on the map.
     *
     * @param point The given point of the map.
     * @return (true)  If the point is blocked.
     * @return (false) If the point is not blocked.
     */
    bool _getBlocked(MapPoint point);

    /**
     * @brief Sets the "blocked" value at given point on the map.
     *
     * @param point The given point of the map.
     * @param blockedStatus The value to set.
     */
    void _setBlocked(MapPoint point, bool blockedStatus);

    /**
     * @brief Gets the "direction" value at given point on the map.
     *
     * @param point The given point of the map.
     * @return (uint8_t) The index of the direction to go at the given point.
     */
    uint8_t _getDirection(MapPoint point);

    /**
     * @brief Sets the "direction" value at given point on the map.
     *
     * @param point The given point of the map.
     * @param newDirection The value to set.
     */
    void _setDirection(MapPoint point, uint8_t newDirection);

    /**
     * @brief Gets the "distanceToGoal" value at given point on the map.
     *
     * @param point The given point of the map.
     * @return (uint16_t) The distance to the goal, via the shortest path.
     */
    uint16_t _getDistanceToGoal(MapPoint point);

    /**
     * @brief Sets the "distanceToGoal" value at given point on the map.
     *
     * @param point The given point of the map.
     * @param newDistance The value to set.
     */
    void _setDistanceToGoal(MapPoint point, uint16_t newDistance);

    /**
     * @brief Gets the "distanceToWall" value at given point on the map.
     *
     * @param point The given point of the map.
     * @return (uint8_t) The distance to the closest wall.
     */
    uint8_t _getDistanceToWall(MapPoint point);

    /**
     * @brief Sets the "distanceToWall" value at given point on the map.
     *
     * @param point The given point of the map.
     * @param newDistanceToWall The distance to the closest wall.
     */
    void _setDistanceToWall(MapPoint point, uint8_t newDistanceToWall);

    /**
     * @brief Gets the "seen" value at given point on the map.
     *
     * @param point The given point of the map.
     * @return (uint8_t) The number of times the given position has been
     * seen by a sensor.
     */
    uint8_t _getSeen(MapPoint point);

    /**
     * @brief Sets the "seen" value at given point on the map.
     *
     * @param point The given point of the map.
     * @param seenOccurrence The number of times the given position has been
     * seen by a sensor.
     */
    void _setSeen(MapPoint point, uint8_t seenOccurrence);

    /**
     * @brief Resets all the data in a Map by setting evert value of every layer
     * to 0, aside from the distanceToGoal layer, in which evert value is set
     * to max.
     */
    void _resetData();
};

#endif  // MAP_H
