

#include "grid.h"

#include <Arduino.h>

#include "angleAndPosition.h"
#include "bluetoothLowEnergy.h"
#include "obstacles.h"

const String edgeOfMapCharacter = "B";
const String obstacleCharacters[4] = {"░", "▒", "▓", "█"};
const String originCharacter = "S";
const String startingPositionCharacter = "X";

Grid::Grid(BluetoothLowEnergy* blePtr) : _blePtr(blePtr) {}

uint8_t Grid::getSensorType(Position position) {
    int16_t yIndex = this->_getYIndexFromMillimeter(position.y);
    int16_t xIndex = this->_getXIndexFromMillimeter(position.x);

    if ((yIndex < 0) || (yIndex < 0)) {
        Serial.println("not in grid");
        return -1;
    }

    return _getData(yIndex, xIndex);
}

void Grid::setObstacle(Obstacle obstacleToAdd, bool sendOverBLE) {
    int16_t yIndex = this->_getYIndexFromMillimeter(obstacleToAdd.position.y);
    int16_t xIndex = this->_getXIndexFromMillimeter(obstacleToAdd.position.x);

    if ((yIndex < 0) || (yIndex < 0)) {
        Serial.println("Not in grid");
        return;
    }

    this->_setData(yIndex, xIndex, obstacleToAdd.sensorType);

    if (sendOverBLE) {
        uint16_t totalIndex = yIndex * GRID_DIMENSION + xIndex;

        this->_blePtr->sendGridChunk(obstacleToAdd.sensorType, totalIndex, 1);
    }
}

void Grid::updateObstacle(Obstacle obstacle, bool sendOverBLE) {
    uint8_t existingValue = this->getSensorType(obstacle.position);
    uint8_t newValue = obstacle.sensorType;

    // TODO set this back
    //  if (newValue > existingValue) {
    if (1) {
        this->setObstacle(obstacle, sendOverBLE);
    }
}

void Grid::sendEntireGridOverBLE() {
    uint8_t recordedCharacter = this->_getData(0, 0);
    uint32_t startIndex = 0;
    uint32_t count = 0;

    for (uint32_t index = GRID_LOWER_INDEX; index < GRID_ITEM_COUNT; index++) {
        uint16_t y = index / GRID_DIMENSION;
        uint16_t x = index % GRID_DIMENSION;

        uint8_t valueAtCurrentIndex = this->_getData(y, x);

        if (valueAtCurrentIndex != recordedCharacter) {
            count = index - startIndex;

            this->_blePtr->sendGridChunk(recordedCharacter, startIndex, count);

            startIndex = index;

            recordedCharacter = valueAtCurrentIndex;
        }
    }

    count = GRID_ITEM_COUNT - startIndex;

    this->_blePtr->sendGridChunk(recordedCharacter, startIndex, count);
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

    if (yUpperIndex == GRID_UPPER_INDEX) {
        if (xLowerIndex == GRID_LOWER_INDEX) {
            Serial.print(edgeOfMapCharacter);
        }
        for (uint16_t xIndex = xLowerIndex; xIndex <= xUpperIndex; xIndex++) {
            Serial.print(edgeOfMapCharacter);
        }
        if (xUpperIndex == GRID_UPPER_INDEX) {
            Serial.print(edgeOfMapCharacter);
        }
        Serial.println();
    }

    for (int16_t yIndex = yLowerIndex; yIndex <= yUpperIndex; yIndex++) {
        if (xLowerIndex == GRID_LOWER_INDEX) {
            Serial.print(edgeOfMapCharacter);
        }
        for (int16_t xIndex = xLowerIndex; xIndex <= xUpperIndex; xIndex++) {
            if ((xIndex == GRID_ORIGIN_INDEX) &&
                (yIndex == GRID_ORIGIN_INDEX)) {
                Serial.print(originCharacter);
            } else if ((xIndex == xCentreIndex) && (yIndex == yCentreIndex)) {
                Serial.print(startingPositionCharacter);
            } else {
                Serial.print(
                    obstacleCharacters[this->_getData(yIndex, xIndex)]);
            }
        }

        if (xUpperIndex == GRID_UPPER_INDEX) {
            Serial.print(edgeOfMapCharacter);
        }
        Serial.println();
    }

    if (yLowerIndex == GRID_LOWER_INDEX) {
        if (xLowerIndex == GRID_LOWER_INDEX) {
            Serial.print(edgeOfMapCharacter);
        }
        for (uint16_t xIndex = xLowerIndex; xIndex <= xUpperIndex; xIndex++) {
            Serial.print(edgeOfMapCharacter);
        }
        if (xUpperIndex == GRID_UPPER_INDEX) {
            Serial.print(edgeOfMapCharacter);
        }
        Serial.println();
    }

    Serial.print("\n");
}

#include "binary.h"
void Grid::_setData(uint16_t yIndex, uint16_t xIndex, uint8_t value) {
    // Look up table for the bitmasks to clear the data in the target position.
    const uint8_t punchMasks[4] = {0b11111100, 0b11110011, 0b11001111,
                                   0b00111111};

    const uint8_t dataMasks[4][4] = {
        {0b00000000, 0b00000001, 0b00000010, 0b00000011},
        {0b00000000, 0b00000100, 0b00001000, 0b00001100},
        {0b00000000, 0b00010000, 0b00100000, 0b00110000},
        {0b00000000, 0b01000000, 0b10000000, 0b11000000}};

    // DOing this little endian
    uint16_t outerXIndex = xIndex;
    uint16_t innerXIndex = outerXIndex >> 2;
    uint16_t innerBlobIndex = outerXIndex & 3;

    uint8_t blob = _data[yIndex][innerXIndex];

    Serial.print(" xIndex:");
    Serial.print(xIndex);
    Serial.print(" yIndex:");
    Serial.print(yIndex);
    Serial.print(" innerXIndex:");
    Serial.print(innerXIndex);
    Serial.print(" innerBlobIndex:");
    Serial.print(innerBlobIndex);

    Serial.print(" Data :");
    Serial.print(value);
    Serial.println();

    Serial.print(" blob:");
    printByte(blob);

    blob &= punchMasks[innerBlobIndex];
    Serial.print(" Punch:");
    printByte(punchMasks[innerBlobIndex]);

    Serial.print(" blob:");
    printByte(blob);

    blob |= dataMasks[innerBlobIndex][value];
    Serial.print(" dataMasks:");
    printByte(dataMasks[innerBlobIndex][value]);

    Serial.print(" blob:");
    printByte(blob);

    this->_data[yIndex][innerXIndex] = blob;

    Serial.print(" Now To read :");
    Serial.println(_getData(yIndex, xIndex));
    Serial.println();
}
uint8_t Grid::_getData(uint16_t yIndex, uint16_t xIndex) {
    uint16_t outerXIndex = xIndex;
    uint16_t innerXIndex = outerXIndex >> 2;
    uint16_t innerBlobIndex = outerXIndex & 3;

    uint8_t blob = _data[yIndex][innerXIndex];

    uint8_t value = blob >> (innerBlobIndex * 2) & 3;

    return value;
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