

#include <Arduino.h>

#include "ble.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
#include "infrared.h"
#include "pixels.h"
#include "systemInfo.h"
#include "ultrasonic.h"

Drive drive;

BluetoothLowEnergy ble;
Pixels pixels(PIXELS_DATA_PIN);
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO, ULTRASONIC_TIMEOUT);
Infrared leftInfrared(LEFT_INFRARED_INDEX);
Infrared rightInfrared(RIGHT_INFRARED_INDEX);
Bumper bumper(BUMPER_SHIFT_REG_DATA, BUMPER_SHIFT_REG_LOAD,
              BUMPER_SHIFT_REG_CLOCK, BUMPER_INTERRUPT_PIN);

ErrorIndicator errorIndicator;

bool bumperUpdate = false;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    drive.setup();
    pixels.setup();

    ble.setup();

    leftInfrared.setup();
    rightInfrared.setup();
    ultrasonic.setup();

    ultrasonic.setup();

    bumper.setup();

    // Use a lambda function to set the bumperUpdate flag when the bumper is
    // pressed or released.
    bumper.assignCallback([]() { bumperUpdate = true; });

    errorIndicator.setup();
    errorIndicator.setPixels(&pixels);
    errorIndicator.setDrive(&drive);
    // errorIndicator.setImmobilizer(&drive);
}

void loop() {
    Serial.println(millis());

    drive.setVelocity(100);

    if (millis() > 5000) {
        errorIndicator.errorOccurred("millis too big");
    }

    //     ble.poll();
    //
    //     Serial.println("Hello World");

    //     drive.checkEncoders();
    //
    //     if (bumperUpdate) {
    //         bumperUpdate = false;
    //
    //         u_int8_t bumperData = bumper.read();
    //         pixels.displayBumperHard(bumperData);
    //
    //     }
    //
    //     unsigned int distance = ultrasonic.read();
    //
}
