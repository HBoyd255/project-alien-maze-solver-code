
#ifndef PIXELS_H
#define PIXELS_H

#include <Arduino.h>

// This Class builds off the NeoPixelBus library written by Michael C. Mille
//  https://github.com/Makuna/NeoPixelBus
#include <NeoPixelBus.h>

#include "angleAndPosition.h"

class Pixels {
   public:
    Pixels(uint8_t dataPin, uint8_t ledCount, Angle rotationOffset);

    void setup();

    void setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);

    void setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b, bool show);

    void setAll(uint8_t r, uint8_t g, uint8_t b);

    void setAll(uint8_t r, uint8_t g, uint8_t b, bool show);

    void clear();

    void clear(bool show);

    void show();

    void point(Angle angle);

    uint16_t getLedCount();

   private:
    uint8_t _ledCount;
    NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> _ledStrip;
    Angle _rotationOffset;
};

#endif  // PIXELS_H