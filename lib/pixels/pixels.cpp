#include "pixels.h"

uint8_t getLargestValueInMatrix(const matrix& targetMatrix) {
    uint8_t maxValue = 0;

    for (const auto& row : targetMatrix) {
        for (const auto& item : row) {
            if (item > maxValue) {
                maxValue = item;
            }
        }
    }
    return maxValue;
}

Pixels::Pixels(matrix matrix, uint8_t dataPin)
    : _groupsMatrix(matrix),
      _maxLEDCount(getLargestValueInMatrix(matrix) + 1),
      _groupCount(matrix.size()),
      _ledStrip(_maxLEDCount, dataPin) {}

void Pixels::setup() {
    _ledStrip.Begin();
    _ledStrip.Show();
}

void Pixels::setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
    this->setPixel(pixel, r, g, b, false);
}
void Pixels::setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b,
                      bool show) {
    this->_ledStrip.SetPixelColor(pixel, RgbColor(r, g, b));

    if (show) {
        this->show();
    }
}

void Pixels::setGroup(uint8_t groupIndex, uint8_t r, uint8_t g, uint8_t b) {
    this->setGroup(groupIndex, r, g, b, false);
}

void Pixels::setGroup(uint8_t groupIndex, uint8_t r, uint8_t g, uint8_t b,
                      bool show) {
    for (const uint8_t& ledNumber : _groupsMatrix[groupIndex]) {
        this->_ledStrip.SetPixelColor(ledNumber, RgbColor(r, g, b));
    }
    if (show) this->show();
}

void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b) {
    this->setAll(r, g, b, false);
}

void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b, bool show) {
    for (uint8_t i = 0; i < _maxLEDCount; i++) {
        this->setPixel(i, r, g, b);
    }

    if (show) {
        this->show();
    }
}

// void Pixels::displayBumperSoft(u_int8_t bumperData) {
//     for (int i = 0; i < 8; i++) {
//         bool bit = (bumperData >> i) & 1;
//         if (bit) {
//             this->setGroup(i, 255, 0, 0);
//         }
//     }
// }
//
// void Pixels::displayBumperHard(u_int8_t bumperData) {
//     this->clear();
//
//     this->displayBumperSoft(bumperData);
//
//     this->show();
// }

void Pixels::clear(bool show) {
    this->setAll(0, 0, 0);
    if (show) {
        this->show();
    }
}

void Pixels::clear() { this->clear(false); }

void Pixels::show() { this->_ledStrip.Show(); }

void Pixels::stupidTest() {
    Serial.println(_maxLEDCount);
    Serial.println(_groupCount);
    Serial.println();
}

uint8_t Pixels::getGroupCount() { return _groupCount; }