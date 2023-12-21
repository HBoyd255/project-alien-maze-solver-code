

#include "ultrasonic.h"

// this module is for the HC-SR04

// The period(in microseconds) of the pulse sent to the trigger pin of the
// ultrasonic.
#define TRIGGER_PULSE_DURATION 10

Ultrasonic::Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout,
                       uint16_t maxRange)
    : _triggerPin(triggerPin),
      _echoPin(echoPin),
      _timeout(timeout),
      _maxRange(maxRange) {}

void Ultrasonic::setup(voidFuncPtr isrPtr) {
    pinMode(this->_triggerPin, OUTPUT);
    pinMode(this->_echoPin, INPUT);

    attachInterrupt(digitalPinToInterrupt(this->_echoPin), isrPtr, CHANGE);
}

int16_t Ultrasonic::_pulseDuration() {
    uint32_t functionStartTime = micros();

    while (this->_pinUpTime < functionStartTime ||
           this->_pinDownTime < functionStartTime) {
        if (micros() - functionStartTime > this->_timeout) {
            return -1;
        }
    }
    uint32_t pulseWidth = this->_pinDownTime - this->_pinUpTime;

    return pulseWidth;
}

/**
 * Reads the distance measured by the ultrasonic sensor.
 *
 * @return The distance measured in millimeters.
 */
int16_t Ultrasonic::read() {
    digitalWrite(this->_triggerPin, LOW);
    delayMicroseconds(TRIGGER_PULSE_DURATION);
    digitalWrite(this->_triggerPin, HIGH);
    delayMicroseconds(TRIGGER_PULSE_DURATION);
    digitalWrite(this->_triggerPin, LOW);

    int16_t reflectionDuration = this->_pulseDuration();

    if (reflectionDuration == -1) {
        return -1;
    }

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

    return (distance < this->_maxRange) ? distance : -1;
}

void Ultrasonic::isr() {
    if (digitalRead(this->_echoPin) == HIGH) {
        this->_pinUpTime = micros();
    } else {
        this->_pinDownTime = micros();
    }
}