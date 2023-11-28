

#include "ultrasonic.h"

#define TRIGGER_PULSE_DURATION 10

Ultrasonic::Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout) {
    this->triggerPin = triggerPin;
    this->echoPin = echoPin;
    this->timeout = timeout;
}

void Ultrasonic::setup() {
    pinMode(this->triggerPin, OUTPUT);
    pinMode(this->echoPin, INPUT);
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

    uint16_t reflectionDuration =
        pulseIn(this->echoPin, HIGH, this->timeout);

    uint16_t duration = reflectionDuration >> 1;

    // distance = speed * time

    // Speed of sound - 343 meters per second
    // speed = 0.343 meters per millisecond

    // distance = speed * time / 2

    // millimeters = (meters per millisecond) * milliseconds

    uint16_t distance = constrain(duration * 0.343, 0x0000, 0xFFFF);

    return distance;
}