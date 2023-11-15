#include <Arduino.h>
#include "pixels.h"

Pixels pixels = Pixels();

void setup() {
    Serial.begin(115200);

    pixels.setup();
}

void loop() {
    int red = millis() >> 4 & 255;

    int green = 100;

    int blue = 0;

    Serial.print(red);
    Serial.print(",");
    Serial.print(green);
    Serial.print(",");
    Serial.println(blue);

    pixels.setAll(red, green, blue, false);
    pixels.setGroup(LEFT_LED_GROUP, 255, 0, 0);

    pixels.show();
}
