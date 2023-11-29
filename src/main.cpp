

#include <Arduino.h>

#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "harrysBle.h"
#include "infrared.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

ErrorIndicator errorIndicator;

Drive drive;
Pixels pixels(PIXELS_DATA_PIN);
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, ULTRASONIC_TIMEOUT);
Infrared leftInfrared(LEFT_INFRARED_INDEX);
Infrared rightInfrared(RIGHT_INFRARED_INDEX);
Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);

BluetoothLowEnergy harrysBle(&errorIndicator);

volatile bool bumperUpdate = 0;

// void ISR() { ultrasonic.isr(); }

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    errorIndicator.assignPixels(&pixels);
    errorIndicator.assignDrive(&drive);
    errorIndicator.setup();

    drive.setup();
    pixels.setup();

    leftInfrared.setup();
    rightInfrared.setup();
    ultrasonic.setup([]() { ultrasonic.isr(); });

    bumper.setup();

    // Use a lambda function to set the bumperUpdate flag when the bumper is
    // pressed or released.
    bumper.assignCallback([]() { bumperUpdate = true; });

    harrysBle.setup();
}

void loop() {
    uint16_t leftSensor = leftInfrared.read();
    uint16_t frontSensor = ultrasonic.read();
    uint16_t rightSensor = rightInfrared.read();

    Serial.print("Left:");
    Serial.print(leftSensor);
    Serial.print(" Front:");
    Serial.print(frontSensor);
    Serial.print(" Right:");
    Serial.println(rightSensor);

    harrysBle.updateRangeSensors(leftSensor, frontSensor, rightSensor);
    harrysBle.updateBumper(bumper.read());
    harrysBle.poll();


    

    // delay(1000);

    // if (bumperUpdate) {
    //     bumperUpdate = false;
    //     Serial.print("Bumper:");
    //     Serial.println(bumper.read());
    // }
}
