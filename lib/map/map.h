/**
 * @file map.h
 * @brief Declarations of the Mapping objects, MapPoint, MapItem and Map.
 * Responsible for implementing a modified version of a flood fill algorithm.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2024-01-19
 * @copyright Copyright (c) 2024
 */
#ifndef MAP_H
#define MAP_H

#include <Arduino.h>

#define MAP_WIDTH_CM 150
#define MAP_HEIGHT_CM 200

// Forwards declaration of BrickList, Position and Angle class.
class BrickList;
class Position;
class Angle;

/**
 * @brief MapPoint struct, Sores an X and Y Positions in Centimeters, as
 * Integers.
 *
 * Primarily used to stores the 2d Coordinates of the items in map array.
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

    int squaredDistanceTo(MapPoint otherPoint);

    Position toPosition() const;
    void setFromPosition(Position position);

    String toString() const;

    MapPoint operator+(const MapPoint& PointToAdd) const;

    MapPoint& operator+=(const MapPoint& PointToAdd);

    bool operator==(const MapPoint& PointToCompare) const;
};

struct __attribute__((packed)) MapItem {
    unsigned int been : 1;
    unsigned int blocked : 1;
    unsigned int direction : 3;  // degrees / 45
    unsigned int distanceToGoal : 11;
    unsigned int distanceToWall : 8;
    unsigned int seen : 8;
};

class Map {
   public:
    Map();

    void solve(BrickList brickList, Position endPosition);
    void solveFromSeen(Position endPosition);

    void getAngle(Position robotPosition, Angle* angleToUpdatePtr);
    float getEuclideanDistanceToEnd(Position robotPosition);

    void sendOverSerial();

    void primeForTracking();

    void TEMP_cutMagicLine();

    void snowPlow(Position robotPosition);

    int countClearSpaces(Position positionA, Position positionB);

    bool safeForBrick(Position positionA, Position positionB, bool* hitWall_P);

    void seenPosition(Position seenPosition);

   private:
    MapItem _mapData[MAP_HEIGHT_CM][MAP_WIDTH_CM];

    MapPoint _endPoint = MapPoint(0, 0);

    const int _width = MAP_WIDTH_CM;
    const int _height = MAP_HEIGHT_CM;
    const int _dimension = MAP_WIDTH_CM * MAP_HEIGHT_CM;

    const MapPoint _neighbors[8] = {{1, 0},  {1, 1},   {0, 1},  {-1, 1},
                                    {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

    void _primeFromBrickList(BrickList brickList);

    void _snowPlow2(MapPoint point);
    void _primeFromSeen();

    void _populateDirections();

    bool _validatePoint(MapPoint pointToTest);

    bool _getBeen(MapPoint point);
    void _setBeen(MapPoint point, bool beenStatus);

    bool _getBlocked(MapPoint point);
    void _setBlocked(MapPoint point, bool blockedStatus);

    uint8_t _getDirection(MapPoint point);
    void _setDirection(MapPoint point, uint8_t newDirection);

    uint16_t _getDistanceToGoal(MapPoint point);
    void _setDistanceToGoal(MapPoint point, uint16_t newValue);

    uint8_t _getDistanceToWall(MapPoint point);
    void _setDistanceToWall(MapPoint point, uint8_t newIncrease);

    uint8_t _getSeen(MapPoint point);
    void _setSeen(MapPoint point, uint8_t seenStatus);

    // void _jankyPrintPoint(MapPoint point, int extraVal = -1);
    void _resetData();
};

#endif  // MAP_H
