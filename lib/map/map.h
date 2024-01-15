#ifndef MAP_H
#define MAP_H

#include <Arduino.h>

#define MAP_WIDTH 150
#define MAP_HEIGHT 200

// Forwards declaration of BrickList class.
class BrickList;
class Position;

struct MapPoint {
    int x;
    int y;

    MapPoint(int x, int y);

    int squaredDistanceTo(MapPoint otherPoint);

    Position toPosition() const;
    String toString() const;

    MapPoint operator+(const MapPoint& PointToAdd) const;

    MapPoint& operator+=(const MapPoint& PointToAdd);

    bool operator==(const MapPoint& PointToCompare) const;
};

struct __attribute__((packed)) MapItem {
    unsigned int isBlocked : 1;
    unsigned int direction : 3;  // degrees / 45
    unsigned int distanceToGoal : 12;
    unsigned int distanceToWall : 8;
};

class Map {
   public:
    Map();

    void fillFromBrickList(BrickList brickList);
    void solve(MapPoint endPoint);

    void print();
    void printDir();
    void printJank();

    void JankyPrintPath(MapPoint startPoint);
    void JankyPrintPath2(MapPoint startPoint);
    void JankyPrintBlockData();

    void jankyPrintPosition(Position position, int extra = -1);

    void printDataToCSV(int samplingInterval = 10);
    void printDataToBIN(int samplingInterval = 10);

   private:
    MapItem _mapData[MAP_HEIGHT][MAP_WIDTH];

    MapPoint _endPoint = MapPoint(0, 0);

    const int _width = MAP_WIDTH;
    const int _height = MAP_HEIGHT;
    const int _dimension = MAP_WIDTH * MAP_HEIGHT;

    const MapPoint _localNeighbors[8] = {{1, 0},  {1, 1},   {0, 1},  {-1, 1},
                                         {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

    void _populateDirections();

    bool _validatePoint(MapPoint pointToTest);

    bool _getBlocked(MapPoint point);
    void _setBlocked(MapPoint point, bool blockedStatus);

    uint8_t _getDirection(MapPoint point);
    void _setDirection(MapPoint point, uint8_t newDirection);

    uint16_t _getDistanceToGoal(MapPoint point);
    void _setDistanceToGoal(MapPoint point, uint16_t newValue);

    uint8_t _getDistanceToWall(MapPoint point);
    void _setDistanceToWall(MapPoint point, uint8_t newIncrease);

    void _jankyPrintPoint(MapPoint point, int extraVal = -1);
    void _resetData();

    int _countBlocks();
};

#endif  // MAP_H
