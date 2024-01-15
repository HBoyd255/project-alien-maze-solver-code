#ifndef BRICK_H
#define BRICK_H

#include "angleAndPosition.h"
#include "mazeConstants.h"

#define BRICK_COUNT 15

enum Zone {
    CentreZone,
    RightZone,
    TopRightZone,
    TopZone,
    TopLeftZone,
    LeftZone,
    BottomLeftZone,
    BottomZone,
    BottomRightZone
};

// TODO Turn Brick into a class.
struct Brick {
    Position position = {0, 0};

    bool isVertical = 0;

    Position getBottomLeft();
    Position getTopRight();

    Zone calculateZone(Position testPosition);

    int squaredDistanceTo(Position testPosition);

    operator String() const;
};

class BrickList {
   public:
    BrickList();

    int getBrickCount();
    Brick getBrick(int index);

    String toString();

    // bool inDeadzone(Position positionToTest);
    // bool nearDeadzone(Position positionToTest);

    void TEMP_addBrick(Brick brickToAdd, int index);
    void TEMP_fillWithTestData();

   private:
    Brick _brickArray[BRICK_COUNT];
    int _brickCount = 0;
    
    // bool _inZone(Position positionToTest, int radius);
};

#endif  // BRICK_H
