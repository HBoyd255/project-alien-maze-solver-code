#ifndef PIXELS_H
#define PIXELS_H

#include <Arduino.h>
#include <NeoPixelBus.h>


//TODO write documentation and improve modularity
class Pixels {
   public:
    Pixels(uint8_t data_pin);
    void test();
    void setup();
    void setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b);
    void setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b, bool show);
    void setAll(uint8_t r, uint8_t g, uint8_t b);
    void setAll(uint8_t r, uint8_t g, uint8_t b, bool show);
    void clear();
    void show();
    NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip;

   private:
    uint8_t data_pin;
};

#endif  // PIXELS_H