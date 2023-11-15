/**
 * File: pixels.cpp
 *
 *
 */

#include "pixels.h"

const char ledGroups[LED_GROUP_COUNT][LEDS_PER_GROUP] = {
    {1, 2},
    {3, 4},
    {5, 5},
    {6, 7},
    {8, 9},
    {10, 11},
    {12, 12},
    {13, 0}};

/**
 * @brief Default constructor for the Pixels class.
 *
 */
Pixels::Pixels()
    : led_strip(LED_GROUP_COUNT * LEDS_PER_GROUP, LED_STRIP_PIN) {
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
void Pixels::setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b, bool show) {
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
 * @brief Sets all the pixels to the specified color.
 *
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

/**
 * @brief Clears all the pixels by setting them to black.
 *
 */
void Pixels::clear() {
    this->setAll(0, 0, 0);
}

/**
 * @brief Shows the pixels by calling the show function on the led_strip object
 * from the NeoPixelBus library.
 *
 */
void Pixels::show() {
    this->led_strip.Show();
}