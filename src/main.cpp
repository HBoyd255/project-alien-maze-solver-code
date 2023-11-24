#include <Arduino.h>

#include "bumpers.h"
#include "drive.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

Drive drive;
Pixels pixels;
Bumper bumper;
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, 20000UL);

bool bumperUpdated = false;

void bumperCallback() { bumperUpdated = true; }

void setup() {
    Serial.begin(115200);
    //
    // drive.setup();
    pixels.setup();

    //     bumper.setup();
    //
    //     bumper.assignCallback(bumperCallback);
    //
    //     pixels.clear();

    ultrasonic.setup();
}

void loop() {
    // drive.checkEncoders();

    unsigned int distance = ultrasonic.read();

    Serial.println(distance);

//     if (distance < 1000) {
//         int diff = distance - 200;
// 
// 
//         drive.move(diff);
//     }
//     else
//     {
//         drive.move(0);
//     }
    
}
