

#include <Arduino.h>

#include "bumper.h"
#include "drive.h"
#include "infrared.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

Drive drive;
Pixels pixels;




Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, 20000UL);
Infrared leftInfrared(LEFT_INFRARED_INDEX);
Infrared rightInfrared(RIGHT_INFRARED_INDEX);
Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);


bool bumperUpdate = false;


//TODO do some pid stuff

void setup() {
    Serial.begin(115200);
    drive.setup();
    pixels.setup();

    leftInfrared.setup();
    rightInfrared.setup();

    ultrasonic.setup();

    bumper.setup();
    bumper.assignCallback([]() { bumperUpdate = true; });

}

void loop() {
    // Print out values over serial
    Serial.print("Left: ");
    Serial.print(leftInfrared.read());
    // Serial.print("mm, Front: ");
    // Serial.print(ultrasonic.read());
    Serial.print("mm, Right:");
    Serial.print(rightInfrared.read());
    Serial.println("mm");

//     int leftDistance = leftInfrared.read();
// 
//     int intensity = constrain(300 - leftDistance, 0, 255);
// 
//     pixels.setGroup(LEFT_LED_GROUP, intensity, 0, 0, true);
// 
//     unsigned int distance = ultrasonic.read();
// 
//     if (distance < 600) {
//         int diff = distance - 200;
// 
//         Serial.println(diff);
// 
//         drive.setVelocity(constrain(diff, -100, 100));
// 
//     } else {
//         drive.stop();
//     }
// 
//     delay(10);
// 
//     if (bumperUpdate) {
//         bumperUpdate = false;
// 
//         uint8_t bumperData = Bumper.read();
// 
//         Serial.println(bumperData, BIN);
// 
//         if (bumperData & 0b00000001) {
//             pixels.setGroup(LEFT_LED_GROUP, 255, 0, 0, true);
//         } else {
//             pixels.setGroup(LEFT_LED_GROUP, 0, 0, 0, true);
//         }
// 
//         if (bumperData & 0b00000010) {
//             pixels.setGroup(RIGHT_LED_GROUP, 255, 0, 0, true);
//         } else {
//             pixels.setGroup(RIGHT_LED_GROUP, 0, 0, 0, true);
//         }
//     }

//     static int i = 0;
// 
//     pixels.setGroup(i, 255, 0, 0);
// 
//     pixels.show();
// 
//     delay(1000);
// 
//     i++;

 


}