
#include "bumper.h"

#include <Arduino.h>

// TODO the bumper is now rotated by 180, fix this in code, make it more modular

Bumper::Bumper(uint8_t dataPin, uint8_t loadPin, uint8_t clockPin,
               uint8_t interruptPin) {
    this->dataPin = dataPin;
    this->loadPin = loadPin;
    this->clockPin = clockPin;
    this->interruptPin = interruptPin;
}

void Bumper::setup() {
    pinMode(dataPin, INPUT);
    pinMode(loadPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
}

void Bumper::assignCallback(voidFuncPtr function) {
    attachInterrupt(digitalPinToInterrupt(interruptPin), function, CHANGE);
}

uint8_t Bumper::read() {
    uint8_t shiftRegisterContents = 0;

    digitalWrite(loadPin, LOW);
    delayMicroseconds(1);
    digitalWrite(loadPin, HIGH);
    delayMicroseconds(1);

    for (uint8_t i = 0; i < 8; i++) {
        shiftRegisterContents <<= 1;

        shiftRegisterContents |= digitalRead(dataPin);

        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);
    }

    return shiftRegisterContents;
}