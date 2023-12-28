

#include "grid.h"

const String edgeOfMapCharacter = "B";
const String obstacleCharacters[4] = {"0", "1", "2", "3"};
const String originCharacter = "S";
const String startingPositionCharacter = "X";

Grid::Grid(/* args */) {}

uint8_t Grid::getSensorType(Position position) {
    int16_t xIndex = this->_getIndexFromMillimeter(position.x);
    int16_t yIndex = this->_getIndexFromMillimeter(position.y);

    if ((xIndex < 0) || (yIndex < 0)) {
        Serial.println("Not in grid");
        return -1;
    }

    return this->_data[xIndex][yIndex];
}

void Grid::setObstacle(Obstacle obstacleToAdd) {
    int16_t xIndex = this->_getIndexFromMillimeter(obstacleToAdd.position.x);
    int16_t yIndex = this->_getIndexFromMillimeter(obstacleToAdd.position.y);

    if ((xIndex < 0) || (yIndex < 0)) {
        Serial.println("Not in grid");
        return;
    }

    this->_data[xIndex][yIndex] = obstacleToAdd.sensorType;
}

void Grid::updateObstacle(Obstacle obstacle) {
    uint8_t existingValue = this->getSensorType(obstacle.position);
    uint8_t newValue = obstacle.sensorType;

    if (newValue > existingValue) {
        this->setObstacle(obstacle);
    }
}

void Grid::print(Position centrePosition, uint8_t radius) {
    int16_t xCentreIndex = this->_getIndexFromMillimeter(centrePosition.x);
    int16_t yCentreIndex = this->_getIndexFromMillimeter(centrePosition.y);

    if ((xCentreIndex < 0) || (yCentreIndex < 0)) {
        Serial.println("Not in grid");
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

    for (int16_t yIndex = yUpperIndex; yIndex >= yLowerIndex; yIndex--) {
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
                Serial.print(obstacleCharacters[this->_data[yIndex][xIndex]]);
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

int16_t Grid::_roundToNearestCentimeter(int16_t valueInMM) {
    // offset the value in mm so that it is rounded to the nearest value,
    // not
    valueInMM += (valueInMM > 0) ? 5 : -4;

    int16_t valueInCM = valueInMM / 10;

    return valueInCM;
}

int16_t Grid::_getIndexFromMillimeter(int16_t distanceMM) {
    int16_t distanceCM = this->_roundToNearestCentimeter(distanceMM);

    int16_t index = GRID_RADIUS_CM + distanceCM;

    if ((index < GRID_LOWER_INDEX) || (index > GRID_UPPER_INDEX)) {
        return -1;
    }

    return index;
}