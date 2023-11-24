

#include "ultrasonic.h"

#define TRIGGER_PULSE_DURATION 10

Ultrasonic::Ultrasonic(int triggerPin, int echoPin, unsigned long timeout) {
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
unsigned int Ultrasonic::read() {
    digitalWrite(this->triggerPin, LOW);
    delayMicroseconds(TRIGGER_PULSE_DURATION);
    digitalWrite(this->triggerPin, HIGH);
    delayMicroseconds(TRIGGER_PULSE_DURATION);
    digitalWrite(this->triggerPin, LOW);

    unsigned long reflectionDuration =
        pulseIn(this->echoPin, HIGH, this->timeout);

    unsigned long duration = reflectionDuration >> 1;

    // distance = speed * time

    // Speed of sound - 343 meters per second
    // speed = 0.343 meters per millisecond

    // distance = speed * time / 2

    // millimeters = (meters per millisecond) * milliseconds

    int distance = duration * 0.343;

    return distance;
}