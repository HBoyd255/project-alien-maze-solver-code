

#include "grid.h"

#include <Arduino.h>

#include "angleAndPosition.h"
#include "bluetoothLowEnergy.h"
#include "obstacles.h"
#include "schedule.h"

// TODO add a value for "no data read" and combine usonic and ir into one "Range
// sensor" data origin.

const String edgeOfMapCharacter = "B";
const String originCharacter = "S";
const String startingPositionCharacter = "X";

Grid::Grid(BluetoothLowEnergy* blePtr) : _blePtr(blePtr) {
    this->_fillGrid(-1);
}

int8_t Grid::getFromPosition(Position position) {
    int16_t yIndex = this->_getYIndexFromMillimeter(position.y);
    int16_t xIndex = this->_getXIndexFromMillimeter(position.x);

    if ((yIndex < 0) || (yIndex < 0)) {
        Serial.println("not in grid");
        return -1;
    }

    return _getData(yIndex, xIndex);
}

void Grid::setFromPosition(Position position, int8_t value) {
    int16_t yIndex = this->_getYIndexFromMillimeter(position.y);
    int16_t xIndex = this->_getXIndexFromMillimeter(position.x);

    if ((yIndex < 0) || (yIndex < 0)) {
        Serial.println("not in grid");
        return;
    }

    _setAndSend(yIndex, xIndex, value);
}

void Grid::setObstacle(Obstacle obstacleToAdd, bool sendOverBLE) {
    int16_t yIndex = this->_getYIndexFromMillimeter(obstacleToAdd.position.y);
    int16_t xIndex = this->_getXIndexFromMillimeter(obstacleToAdd.position.x);

    if ((yIndex < 0) || (yIndex < 0)) {
        Serial.println("Not in grid");
        return;
    }

    int8_t value;

    if (obstacleToAdd.sensorType == 2) {
        value += 1;
    }

    value = constrain(value, 0, 100);

    this->_setAndSend(yIndex, xIndex, value);
}

void Grid::updateObstacle(Obstacle obstacle, bool sendOverBLE) {
    this->setObstacle(obstacle, sendOverBLE);
}

void Grid::startDripFeed() { this->_fullMapsToSend = 2; }

// TODO fix this logic when I have more brain cells
// Wow this is written awfully, but it works so it is not a now issue.
void Grid::pollDripFeed() {
    static uint32_t dripFeedIndex = 0;

    static PassiveSchedule dripFeedScheduler(25);

    // if more iterations of the grid need sending.
    if (this->_fullMapsToSend) {
        // if 25 miliseconds has passed since the last data was sent from this
        // function
        if (dripFeedScheduler.isReadyToRun()) {
            uint32_t startIndex = dripFeedIndex;
            int8_t currentValue = this->_getDataAtMegaIndex(startIndex);

            while (dripFeedIndex < GRID_ITEM_COUNT) {
                dripFeedIndex++;

                if (this->_getDataAtMegaIndex(dripFeedIndex) != currentValue) {
                    uint32_t count = dripFeedIndex - startIndex;

                    this->_blePtr->sendGridChunk(currentValue, startIndex,
                                                 count);

                    return;
                }
            }
            // If hit the end of the grid
            if (dripFeedIndex == GRID_ITEM_COUNT) {
                // send the data
                uint32_t count = dripFeedIndex - startIndex;
                this->_blePtr->sendGridChunk(currentValue, startIndex, count);

                // reset the index;
                dripFeedIndex = 0;
                // reduce the number of maps that need sending;
                this->_fullMapsToSend -= 1;

                return;
            }
        }
    }
}

void Grid::print(Position centrePosition, uint8_t radius) {
    int16_t xCentreIndex = this->_getXIndexFromMillimeter(centrePosition.x);
    int16_t yCentreIndex = this->_getYIndexFromMillimeter(centrePosition.y);

    if ((xCentreIndex < 0) || (yCentreIndex < 0)) {
        Serial.println("Not In Grid");
        return;
    }

    uint16_t xLowerIndex =
        constrain(xCentreIndex - radius, GRID_LOWER_INDEX, GRID_UPPER_INDEX);
    uint16_t xUpperIndex =
        constrain(xCentreIndex + radius, GRID_LOWER_INDEX, GRID_UPPER_INDEX);

    uint16_t yUpperIndex =
        constrain(yCentreIndex + radius, GRID_LOWER_INDEX, GRID_UPPER_INDEX);
    uint16_t yLowerIndex =
        constrain(yCentreIndex - radius, GRID_LOWER_INDEX, GRID_UPPER_INDEX);

    for (int16_t yIndex = yLowerIndex; yIndex <= yUpperIndex; yIndex++) {
        for (int16_t xIndex = xLowerIndex; xIndex <= xUpperIndex; xIndex++) {
            Serial.print("[");
            if ((xIndex == GRID_ORIGIN_INDEX) &&
                (yIndex == GRID_ORIGIN_INDEX)) {
                Serial.print(originCharacter);
            } else if ((xIndex == xCentreIndex) && (yIndex == yCentreIndex)) {
                Serial.print(startingPositionCharacter);
            } else {
                Serial.print(this->_getData(yIndex, xIndex));
            }
            Serial.print("]");
        }
        Serial.println();
    }

    Serial.print("\n");
}

#include "binary.h"
void Grid::_setData(uint16_t yIndex, uint16_t xIndex, int8_t value) {
    // TODO check if the data is between -1 abd 100

    if (value < -1 || value > 100) {
        Serial.println("Data is the wrong size");
    }
    this->_data[yIndex][xIndex] = value;
}
int8_t Grid::_getData(uint16_t yIndex, uint16_t xIndex) {
    int8_t value = _data[yIndex][xIndex];

    return value;
}

void Grid::_setAndSend(uint16_t yIndex, uint16_t xIndex, int8_t newValue) {
    int8_t oldValue = this->_getData(yIndex, xIndex);

    // TODO make this only send new values, or dont.
    if (newValue != oldValue) {
        this->_setData(yIndex, xIndex, newValue);

        uint32_t totalIndex = (yIndex * GRID_DIMENSION) + xIndex;
        this->_blePtr->sendGridChunk(newValue, totalIndex, 1);
    }
}

int16_t Grid::_roundToNearestCentimeter(int16_t valueInMM) {
    // offset the value in mm so that it is rounded to the nearest value,
    // not
    valueInMM += (valueInMM > 0) ? 5 : -4;

    int16_t valueInCM = valueInMM / 10;

    return valueInCM;
}

int16_t Grid::_getYIndexFromMillimeter(int16_t distanceMM) {
    int16_t distanceCM = this->_roundToNearestCentimeter(distanceMM);

    int16_t index = GRID_RADIUS_CM - distanceCM;

    if ((index < GRID_LOWER_INDEX) || (index > GRID_UPPER_INDEX)) {
        return -1;
    }

    return index;
}

int16_t Grid::_getXIndexFromMillimeter(int16_t distanceMM) {
    int16_t distanceCM = this->_roundToNearestCentimeter(distanceMM);

    int16_t index = GRID_RADIUS_CM + distanceCM;

    if ((index < GRID_LOWER_INDEX) || (index > GRID_UPPER_INDEX)) {
        return -1;
    }

    return index;
}

// TODO Fix THis
int8_t Grid::_getDataAtMegaIndex(uint32_t megaIndex) {
    uint16_t y = megaIndex / GRID_DIMENSION;
    uint16_t x = megaIndex % GRID_DIMENSION;

    return this->_getData(y, x);
}

void Grid::_fillGrid(int8_t value) {
    for (uint16_t yIndex = GRID_LOWER_INDEX; yIndex <= GRID_UPPER_INDEX;
         yIndex++) {
        for (uint16_t xIndex = GRID_LOWER_INDEX; xIndex <= GRID_UPPER_INDEX;
             xIndex++) {
            this->_setData(yIndex, xIndex, value);
        }
    }
}
