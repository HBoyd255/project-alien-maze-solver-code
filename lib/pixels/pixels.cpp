#include "pixels.h"

Pixels::Pixels(uint8_t dataPin, uint8_t ledCount, Angle rotationOffset)
    : _ledCount(ledCount),
      _ledStrip(_ledCount, dataPin),
      _rotationOffset(rotationOffset) {}

void Pixels::setup() {
    this->_ledStrip.Begin();
    this->_ledStrip.Show();
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

void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b) {
    this->setAll(r, g, b, false);
}

void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b, bool show) {
    for (uint8_t i = 0; i < this->_ledCount; i++) {
        this->setPixel(i, r, g, b);
    }

    if (show) {
        this->show();
    }
}

void Pixels::clear(bool show) {
    this->setAll(0, 0, 0);
    if (show) {
        this->show();
    }
}

void Pixels::clear() { this->clear(false); }

void Pixels::show() { this->_ledStrip.Show(); }

void Pixels::point(Angle angle) {
    angle += _rotationOffset;
    uint8_t index = angle.segmentIndex(this->_ledCount);

    this->clear();
    this->setPixel(index, 255, 0, 0, true);
}

uint16_t Pixels::getLedCount() { return this->_ledCount; }