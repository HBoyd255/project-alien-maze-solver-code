#include <Arduino.h>
#include <NeoPixelBus.h>

#include "bleControl.h"
#include "bumperControl.h"
#include "motorControl.h"
#include "pixels.h"

#define GPIO1 A7
#define GPIO2 A6
#define GPIO3 A3
#define GPIO4 A2

#define DATA GPIO1   // Green wire
#define LOAD GPIO2   // Yellow wire
#define CLOCK GPIO3  // Orange wire

const uint16_t PixelCount = 14;  // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = GPIO4;  // make sure to set this to the correct pin, ignored for Esp8266

Bumper bumper = Bumper(GPIO1, LOAD, CLOCK);

Pixels pixels = Pixels(GPIO4);

void setup() {
    Serial.begin(115200);

    pixels.setup();

    setupMotors();
    setupBle();
}

/**
 * @brief Temporary function to test the RGB functionality.
 * 
 * It retrieves the 4 bytes from the ble connection and extracts the red, green
 * and blue values from it. Then it sets the pixels to the retrieved values.
 */
void testRgbCharacteristic() {
    unsigned int data = getLed();

    if (data) {
        uint8_t blue = (data & 0xFF);
        uint8_t green = ((data >> 8) & 0xFF);
        uint8_t red = ((data >> 16) & 0xFF);

        Serial.print("--------------------\n");
        Serial.print("Red: ");
        Serial.println(red);
        Serial.print("Green: ");
        Serial.println(green);
        Serial.print("Blue: ");
        Serial.println(blue);

        pixels.setAll(red, green, blue, true);
    }
}

/**
 * @brief Temporary function to test the motor override functionality.
 * 
 * Just works by getting the motor override values from the ble connection and
 * sends them to the motors. 
 */
void testMotorOverride() {

    signed char left_motor_value = getMotorOverrideValues(left_motor);
    signed char right_motor_value = getMotorOverrideValues(right_motor);

    setMotorSpeed(left_motor, left_motor_value);
    setMotorSpeed(right_motor, right_motor_value);
}

void loop() {
    bleUpdateBumper(bumper.getInstant());

    bleUpdateMillis(millis());

    testMotorOverride();

    testRgbCharacteristic();

    blePoll();
}
