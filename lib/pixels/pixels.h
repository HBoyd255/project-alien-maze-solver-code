
/**
 * @file pixels.h
 * @brief Header file for the Pixels class.
 *
 * This file contains the declaration of the Pixels class, which is responsible
 * for controlling an LED strip.
 */

#ifndef PIXELS_H
#define PIXELS_H

#include <Arduino.h>
#include <NeoPixelBus.h>

#include <vector>
typedef std::vector<std::vector<uint8_t>> matrix;

#include "systemInfo.h"

class Pixels {
   public:
    Pixels(matrix matrix, uint8_t dataPin);

    void setup();

    void setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);

    void setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b, bool show);

    void setGroup(uint8_t index, uint8_t r, uint8_t g, uint8_t b);

    void setGroup(uint8_t index, uint8_t r, uint8_t g, uint8_t b, bool show);

    void setAll(uint8_t r, uint8_t g, uint8_t b);

    void setAll(uint8_t r, uint8_t g, uint8_t b, bool show);

    //     void displayBumperSoft(uint8_t bumperData);
    //
    //     void displayBumperHard(uint8_t bumperData);

    void clear();

    void clear(bool show);

    void show();

    void stupidTest();

    uint8_t getGroupCount();

   private:
    matrix _groupsMatrix;
    uint8_t _maxLEDCount;
    uint8_t _groupCount;
    NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> _ledStrip;
};

#endif  // PIXELS_H