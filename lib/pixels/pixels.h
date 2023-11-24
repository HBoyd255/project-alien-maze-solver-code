
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

#include "systemInfo.h"

// The number of LED groups.
#define LED_GROUP_COUNT 8

// The number of LEDs per group.
#define LEDS_PER_GROUP 2

// Enumerated type for the LED groups.
enum LEDGroups {
    FRONT_LED_GROUP,
    FRONT_RIGHT_LED_GROUP,
    RIGHT_LED_GROUP,
    BACK_RIGHT_LED_GROUP,
    BACK_LED_GROUP,
    BACK_LEFT_LED_GROUP,
    LEFT_LED_GROUP,
    FRONT_LEFT_LED_GROUP
};

/**
 * @class Pixels
 * @brief Class for controlling an LED strip.
 *
 * The Pixels class provides methods for setting the color of individual LEDs,
 * groups of LEDs, and all LEDs on the strip. It also provides methods for
 * clearing the strip and showing the changes made.
 */
class Pixels {
   public:
    // TODO Make the pins configurable, then use the consts in the initializer.
    /**
     * @brief Constructor for Pixels class.
     */
    Pixels(): led_strip(LED_GROUP_COUNT * LEDS_PER_GROUP, PIXELS_DATA_PIN) {};

    /**
     * @brief Initializes the LED strip.
     */
    void setup();

    /**
     * @brief Sets the color of an individual LED.
     *
     * @param pixel The index of the LED to set the color for.
     * @param r The red value of the color.
     * @param g The green value of the color.
     * @param b The blue value of the color.
     */
    void setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Sets the color of an individual LED, with the option to show the
     * changes immediately.
     *
     * @param pixel The index of the LED to set the color for.
     * @param r The red value of the color.
     * @param g The green value of the color.
     * @param b The blue value of the color.
     * @param show Whether to show the changes immediately.
     */
    void setPixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b, bool show);

    /**
     * @brief Sets the color of a group of LEDs.
     *
     * @param group The index of the group to set the color for.
     * @param r The red value of the color.
     * @param g The green value of the color.
     * @param b The blue value of the color.
     */
    void setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Sets the color of a group of LEDs with the option to show the
     * changes immediately.
     *
     * @param group The index of the group to set the color for.
     * @param r The red value of the color.
     * @param g The green value of the color.
     * @param b The blue value of the color.
     * @param show Whether to show the changes immediately.
     */
    void setGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b, bool show);

    /**
     * @brief Sets the color of all LEDs.
     *
     * @param r The red value of the color.
     * @param g The green value of the color.
     * @param b The blue value of the color.
     */
    void setAll(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Sets the color of all LEDs with the option to show the changes
     * immediately.
     *
     * @param r The red value of the color.
     * @param g The green value of the color.
     * @param b The blue value of the color.
     * @param show Whether to show the changes immediately.
     */
    void setAll(uint8_t r, uint8_t g, uint8_t b, bool show);

    /**
     * @brief Clears the LED strip.
     *
     */
    void clear();

    /**
     * @brief Clears the LED strip.
     *
     * @param show Whether to show the changes immediately.
     */
    void clear(bool show);

    /**
     * @brief Shows the changes made to the LED strip.
     */
    void show();

   private:
    NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> led_strip;
};

#endif  // PIXELS_H