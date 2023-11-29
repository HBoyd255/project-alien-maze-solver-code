

#include "ultrasonic.h"

#include "systemInfo.h"

#define TRIGGER_PULSE_DURATION 10

Ultrasonic::Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout) {
    this->triggerPin = triggerPin;
    this->echoPin = echoPin;
    this->timeout = timeout;
}

void Ultrasonic::setup(voidFuncPtr isr) {
    pinMode(this->triggerPin, OUTPUT);
    pinMode(this->echoPin, INPUT);

    attachInterrupt(digitalPinToInterrupt(this->echoPin), isr, CHANGE);

}

uint16_t Ultrasonic::myPulseFunction(uint32_t timeout) {
    uint32_t functionStartTime = micros();

    //     while (digitalRead(pin) != HIGH) {
    //         if (micros() - functionStartTime > timeout) {
    //             return 0;
    //             // TODO throw an error, or return "MAX distance" or something
    //         }
    //     }
    //     uint32_t highStartTime = micros();
    //
    //     while (digitalRead(pin) != LOW) {
    //         if (micros() - functionStartTime > timeout) {
    //             return 0;
    //             // TODO throw an error, or return "MAX distance" or something
    //         }
    //     }

    while (this->pinUpTime < functionStartTime ||
           this->pinDownTime < functionStartTime) {
        if (micros() - functionStartTime > timeout) {
            return 0;
            // TODO throw an error, or return "MAX distance" or something
        }
    }
    uint32_t pulseWidth = pinDownTime - pinUpTime;

    // Serial.print("started at: ");
    // Serial.print(functionStartTime);
    // Serial.print(" up: ");
    // Serial.print(pinUpTime);
    // Serial.print(" down: ");
    // Serial.print(pinDownTime);
    // Serial.print(" duration: ");
    // Serial.println(pulseWidth);

    return pulseWidth;
}

/**
 * Reads the distance measured by the ultrasonic sensor.
 *
 * @return The distance measured in millimeters.
 */
uint16_t Ultrasonic::read() {
    digitalWrite(this->triggerPin, LOW);
    delayMicroseconds(TRIGGER_PULSE_DURATION);
    digitalWrite(this->triggerPin, HIGH);
    delayMicroseconds(TRIGGER_PULSE_DURATION);
    digitalWrite(this->triggerPin, LOW);

    uint16_t reflectionDuration = this->myPulseFunction(10000);

    uint16_t duration = reflectionDuration >> 1;

    // distance = speed * time

    // Speed of sound - 343 meters per second
    // speed = 0.343 meters per millisecond

    // distance = speed * time / 2

    // millimeters = (meters per millisecond) * microseconds

    uint16_t distance = constrain(duration * 0.343, 0x0000, 0xFFFF);

    // cap distance at 1000mm

    // so cap microseconds at 6000
    // call the timeout 10000 microseconds to add a bit

    return distance;
}

void Ultrasonic::isr() {
    if (digitalRead(this->echoPin) == HIGH) {
        this->pinUpTime = micros();
    } else {
        this->pinDownTime = micros();
    }
}