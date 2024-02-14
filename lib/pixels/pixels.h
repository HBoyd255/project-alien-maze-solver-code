
#ifndef PIXELS_H
#define PIXELS_H

#include <Arduino.h>

// This module acts as a wrapper for the NeoPixelBus library written by
// Michael C. Mille - https://github.com/Makuna/NeoPixelBus
#include <NeoPixelBus.h>

#include "angleAndPosition.h"

struct Colour {
    Colour(byte red, byte green, byte blue);
    Colour(String nameOrHex);
    void set(byte red, byte green, byte blue);

    String toString();

    byte red;
    byte green;
    byte blue;
};

class Pixels {
   public:
    Pixels(uint8_t dataPin, uint8_t ledCount, Angle rotationOffset);

    void setup();

    void setPixel(int index, Colour colour, bool show = false);

    void setAll(Colour colour, bool show = false);

    void clear(bool show = false);

    void show();

    void point(Angle angle);

    uint16_t getLedCount();

   private:
    uint8_t _ledCount;
    NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> _ledStrip;
    Angle _rotationOffset;
};

// void pixelsTest();

#endif  // PIXELS_H