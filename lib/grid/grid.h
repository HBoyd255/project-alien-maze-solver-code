#ifndef GRID_H
#define GRID_H

#define GRID_RADIUS_CM 200
#define GRID_DIMENSION ((GRID_RADIUS_CM * 2) + 1)
#define GRID_LOWER_INDEX 0
#define GRID_UPPER_INDEX (GRID_DIMENSION - 1)

#define GRID_ITEM_COUNT (GRID_DIMENSION * GRID_DIMENSION)

#define GRID_ORIGIN_INDEX GRID_RADIUS_CM

#include <Arduino.h>

// Forward Declaration
class BluetoothLowEnergy;
class Obstacle;
class Position;

class Grid {
   public:
    Grid(BluetoothLowEnergy* blePtr);

    uint8_t getSensorType(Position position);

    void setObstacle(Obstacle obstacleToAdd, bool sendOverBLE);

    void updateObstacle(Obstacle obstacleToAdd, bool sendOverBLE);

    void sendEntireGridOverBLE();

    void startDripFeed();
    void pollDripFeed();

    void print(Position position, uint8_t radius);

   private:
    // The indexes of the data in this array is proportional to the position of
    // the data.
    uint8_t _data[GRID_DIMENSION][GRID_DIMENSION >> 2];

    void _setData(uint16_t yIndex, uint16_t xIndex, uint8_t value);
    uint8_t _getData(uint16_t yIndex, uint16_t xIndex);
    uint8_t _getDataAtMegaIndex(uint32_t megaIndex);
    int16_t _roundToNearestCentimeter(int16_t valueInMM);

    int16_t _getYIndexFromMillimeter(int16_t distanceMM);
    int16_t _getXIndexFromMillimeter(int16_t distanceMM);

    BluetoothLowEnergy* _blePtr;
    
    uint32_t _dripFeedIndex = GRID_ITEM_COUNT;
};

#endif  // GRID_H