#ifndef GRID_H
#define GRID_H

#define GRID_RADIUS_CM 200
#define GRID_DIMENSION (GRID_RADIUS_CM * 2) + 1
#define GRID_LOWER_INDEX 0
#define GRID_UPPER_INDEX GRID_DIMENSION - 1

#define GRID_ORIGIN_INDEX GRID_RADIUS_CM

#include "Arduino.h"
#include "angleAndPosition.h"
#include "obstacles.h"

class Grid {
   public:
    Grid(/* args */);

    uint8_t getSensorType(Position position);

    void setObstacle(Obstacle obstacleToAdd);

    void updateObstacle(Obstacle obstacleToAdd);

    void print(Position position, uint8_t radius);

   private:
    // The indexes of the data in this array is proportional to the position of
    // the data.
    uint8_t _data[GRID_DIMENSION][GRID_DIMENSION];

    int16_t _roundToNearestCentimeter(int16_t valueInMM);
    int16_t _getIndexFromMillimeter(int16_t distanceMM);
};

#endif  // GRID_H