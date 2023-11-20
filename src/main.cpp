#include <Arduino.h>

#include "drive.h"
#include "pixels.h"
#include "systemInfo.h"

Drive drive;
Pixels pixels;

void setup() {
    Serial.begin(115200);

    drive.setup();
    pixels.setup();

    pixels.clear();
}

int waituntill = 1000;

int counter = 0;

void loop() {
    drive.checkEncoders();

    if (!drive.stepsRemaining()) {

        counter++;
        counter &= 7;

        pixels.clear();
        pixels.setGroup(counter, 255, 0, 0, true);

        drive.rotate(-45);

    }



    

    if (Serial.available()) {
        char input = Serial.read();
        switch (input) {
            case 'w':
                drive.move(100);
                break;
            case 'a':
                drive.rotate(-90);
                break;
            case 's':
                drive.move(-100);
                break;
            case 'd':
                drive.rotate(90);
                break;
            default:
                break;
        }
    }
}
