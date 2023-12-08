#include "pixels.h"

#define TOTAL_LED_COUNT LED_GROUP_COUNT* LEDS_PER_GROUP

// TODO change the set all to use the pixel function, not the group function
const uint8_t ledGroups[LED_GROUP_COUNT][LEDS_PER_GROUP] = {
    {7, 8}, {9, 10}, {11, 12}, {13, 14}, {15, 0}, {1, 2}, {3, 4}, {5, 6}};


//TODO come back and test this
Pixels::Pixels(uint8_t dataPin) : led_strip(TOTAL_LED_COUNT, dataPin) {
    this->dataPin = dataPin;
}

/**
 * @brief Sets up the pixels by calling the begin function on the led_strip
 * object from the NeoPixelBus library and then calling the show function to
 * flush the buffer.
 *
 */
void Pixels::setup() {
    led_strip.Begin();
    led_strip.Show();
}

void Pixels::setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
    this->setPixel(pixel, r, g, b, false);
}
void Pixels::setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b,
                      bool show) {
    this->led_strip.SetPixelColor(pixel, RgbColor(r, g, b));

    if (show) {
        this->show();
    }
}

/**
 * @brief Sets the specified group to the specified color.
 *
 * @param group The group to set.
 * @param r The red value to set.
 * @param g The green value to set.
 * @param b The blue value to set.
 */
void Pixels::setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b) {
    this->setGroup(group, r, g, b, false);
}

/**
 * @brief Sets the specified group to the specified color.
 *
 * @param group The group to set.
 * @param r The red value to set.
 * @param g The green value to set.
 * @param b The blue value to set.
 * @param show Whether or not to show the pixels after setting them.
 */
void Pixels::setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b,
                      bool show) {
    for (int i = 0; i < LEDS_PER_GROUP; i++) {
        this->led_strip.SetPixelColor(ledGroups[group][i], RgbColor(r, g, b));
    }

    if (show) this->show();
}

/**
 * @brief Sets all the pixels to the specified color.
 *
 * @param r The red value to set.
 * @param g The green value to set.
 * @param b The blue value to set.
 */
void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b) {
    this->setAll(r, g, b, false);
}

/**
 * @brief Sets all the pixels to the specified color, by iterating through all
 * the groups.
 * @param r The red value to set.
 * @param g The green value to set.
 * @param b The blue value to set.
 * @param show Whether or not to show the pixels after setting them.
 */
void Pixels::setAll(uint8_t r, uint8_t g, uint8_t b, bool show) {
    for (uint8_t i = 0; i < LED_GROUP_COUNT; i++) {
        this->setGroup(i, r, g, b);
    }

    if (show) {
        this->show();
    }
}

void Pixels::displayBumperSoft(u_int8_t bumperData) {
    for (int i = 0; i < 8; i++) {
        bool bit = (bumperData >> i) & 1;
        if (bit) {
            this->setGroup(i, 255, 0, 0);
        }
    }
}

void Pixels::displayBumperHard(u_int8_t bumperData) {
    this->clear();

    this->displayBumperSoft(bumperData);

    this->show();
}

/**
 * @brief Clears all the pixels by setting them to black with the option to
 * show the changes immediately.
 *
 * @param show - Whether to show the cleared pixels or not.
 */
void Pixels::clear(bool show) {
    this->setAll(0, 0, 0);
    if (show) {
        this->show();
    }
}

/**
 * @brief Clears all the pixels by setting them to black.
 *
 */
void Pixels::clear() { this->clear(false); }

/**
 * @brief Shows the pixels by calling the show function on the led_strip
 * object from the NeoPixelBus library.
 *
 */
void Pixels::show() { this->led_strip.Show(); }