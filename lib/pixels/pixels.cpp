
#include "pixels.h"

#include <vector>

#define LED_GROUP_COUNT 8
#define MAX_LED_PER_GROUP 2
#define MAX_PIXEL_COUNT LED_GROUP_COUNT * MAX_LED_PER_GROUP

const std::vector<std::vector<uint8_t>> ledGroups = {
    {1, 2},
    {3, 4},
    {5},
    {6, 7},
    {8, 9},
    {10, 11},
    {12},
    {13, 0}};

Pixels::Pixels(uint8_t data_pin)
    : strip(MAX_PIXEL_COUNT, data_pin), data_pin(data_pin) {
}

void Pixels::setup() {
    strip.Begin();
    strip.Show();
}

void Pixels::test() {
    Serial.println("Test In");
    this->strip.SetPixelColor(3, RgbColor(255, 0, 255));
    this->strip.SetPixelColor(2, RgbColor(255, 0, 255));
    this->strip.SetPixelColor(1, RgbColor(255, 0, 255));
    this->strip.SetPixelColor(0, RgbColor(255, 0, 255));

    this->show();
}

void Pixels::setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b) {
    this->setGroup(group, r, g, b, false);
}

void Pixels::setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b, bool show) {
    for (unsigned int i = 0; i < ledGroups[group].size(); i++) {
        this->strip.SetPixelColor(ledGroups[group][i], RgbColor(r, g, b));
    }
    if (show) {
        this->show();
    }
}

void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b) {
    this->setAll(r, g, b, false);
}
void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b, bool show) {
    for (uint8_t i = 0; i < LED_GROUP_COUNT; i++) {
        this->setGroup(i, r, g, b);
    }
    if (show) {
        this->show();
    }
}

void Pixels::clear() {
    this->setAll(0, 0, 0);
}


void Pixels::show() {
    this->strip.Show();
}

// void setLedGroup(int groupNumber, RgbColor color) {
//     for (unsigned int i = 0; i < ledGroups[groupNumber].size(); i++) {
//         Serial.println(ledGroups[groupNumber][i]);
//     }
// }