#ifndef BRICK_H
#define BRICK_H

#include "angleAndPosition.h"
#include "mazeConstants.h"

// 15 bricks + 4 walls
// Better to have too many bricks than not enough
#define MAX_BRICK_COUNT 40

//TODO remove
enum OrthogonalDirection {
    OrthogonalDown,
    OrthogonalLeft,
    OrthogonalUp,
    OrthogonalRight,
    OrthogonalCount
};

enum Zone {
    CentreZone,
    RightZone,
    TopRightZone,
    TopZone,
    TopLeftZone,
    LeftZone,
    BottomLeftZone,
    BottomZone,
    BottomRightZone,
};

// TODO Turn Brick into a class.
struct Brick {
    Position position = {0, 0};

    int length = BRICK_LENGTH;
    int width = BRICK_WIDTH;

    bool isVertical = 0;

    Brick(int x = 0, int y = 0, bool isVertical = 0);

    Position getBottomLeft();
    Position getTopRight();

    Zone calculateZone(Position testPosition);

    int distanceTo(Position testPosition);
    int squaredDistanceTo(Position testPosition, int* zoneIndexPrt = nullptr);

    String toString();
};

class BrickList {
   public:
    BrickList();

    int getBrickCount();
    Brick getBrick(int index);

    String toString();

    void attemptAppendBrick(Brick brickToAdd);

    void setPreprogrammedMazeData();

    int compare(Position robotPose, Angle angleOfSensor, int measuredDistance);

    void setBrickFromApproximation(Position brickEdgePosition, Angle angleOfSensor);

    int lowestDistance(Position testPosition,
                       int* indexOfClosestBrickPrt = nullptr,
                       int* zoneFromClosestBrickPrt = nullptr);

   private:
    Brick _brickArray[MAX_BRICK_COUNT];
    int _brickCount = 0;

    void _getOrthogonalBrickIndicies(Position robotPosition, int* bottom_IP,
                                     int* left_IP, int* top_IP, int* right_IP);
    void _appendBrick(Brick brickToAdd);
    void _addWalls();
};

#endif  // BRICK_H
