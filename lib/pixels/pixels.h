#ifndef PIXELS_H
#define PIXELS_H

#include <Arduino.h>
#include <NeoPixelBus.h>

// I was going to make make the pin configurable, but I don't think its
// necessary, The way I see it, If its Hard-wired, it may as well be hard-coded.
// TODO Check this is actually good practice.

// The pin the LED strip is connected to.
#define PIXELS_PIN A2

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
 * @brief Class for controlling NeoPixel LED strips.
 */
class Pixels {
   public:
    /**
     * @brief Constructor for Pixels class.
     */
    Pixels();

    /**
     * @brief Initializes the LED strip.
     */
    void setup();

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
     */
    void clear();

    /**
     * @brief Shows the changes made to the LED strip.
     */
    void show();

   private:
    NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> led_strip;
};

#endif  // PIXELS_H
