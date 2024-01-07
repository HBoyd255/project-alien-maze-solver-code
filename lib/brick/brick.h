#ifndef BRICK_H
#define BRICK_H

#include "angleAndPosition.h"
#include "mazeConstants.h"

struct Brick {
    Position position = {0, 0};

    bool isVertical = 0;

    operator String() const;
};

#endif  // BRICK_H
