#ifndef GRID_H
#define GRID_H

// Ok new plan, each cell is a signed byte, 0 - 100 is a probability of a cell
// being populated.
// -1 mean that the data no value has been recorded.

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


    int8_t getFromPosition(Position position);
    void setFromPosition(Position position, int8_t value);

    void setObstacle(Obstacle obstacleToAdd, bool sendOverBLE);

    void updateObstacle(Obstacle obstacleToAdd, bool sendOverBLE);

    void startDripFeed();
    void pollDripFeed();

    void print(Position position, uint8_t radius);

    void _setAndSend(uint16_t yIndex, uint16_t xIndex, int8_t value);

   private:
    // The indexes of the data in this array is proportional to the position
    // of the data.
    int8_t _data[GRID_DIMENSION][GRID_DIMENSION];

    BluetoothLowEnergy* _blePtr;

    uint8_t _fullMapsToSend = 0;

    void _setData(uint16_t yIndex, uint16_t xIndex, int8_t value);
    int8_t _getData(uint16_t yIndex, uint16_t xIndex);
    int8_t _getDataAtMegaIndex(uint32_t megaIndex);
    int16_t _roundToNearestCentimeter(int16_t valueInMM);

    int16_t _getYIndexFromMillimeter(int16_t distanceMM);
    int16_t _getXIndexFromMillimeter(int16_t distanceMM);

    void _fillGrid(int8_t value);
};

#endif  // GRID_H