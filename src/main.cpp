

#include <Arduino.h>

#include "ble.h"
#include "bumper.h"
#include "drive.h"
#include "errorIndicator.h"
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

BluetoothLowEnergy ble(&errorIndicator);

bool bumperUpdate = false;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    errorIndicator.assignPixels(&pixels);
    errorIndicator.assignDrive(&drive);
    errorIndicator.setup();

    drive.setup();
    pixels.setup();

    leftInfrared.setup();
    rightInfrared.setup();
    ultrasonic.setup();

    ultrasonic.setup();

    bumper.setup();

    // Use a lambda function to set the bumperUpdate flag when the bumper is
    // pressed or released.
    bumper.assignCallback([]() { bumperUpdate = true; });

    ble.setup();
}


void loop() {
    
    millis();

    ble.poll();

    Serial.print("Left:");
    Serial.print(leftInfrared.read());
    Serial.print(" Front:");
    Serial.print(ultrasonic.read());
    Serial.print(" Right:");
    Serial.println(rightInfrared.read());

    sensorData.left = leftInfrared.read();


}
