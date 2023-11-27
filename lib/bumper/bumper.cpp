
#include "bumper.h"

#include <Arduino.h>

Bumper::Bumper(int dataPin, int loadPin, int clockPin, int interruptPin) {
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
    attachInterrupt(digitalPinToInterrupt(interruptPin), function,
                    CHANGE);
}

uint8_t Bumper::read() {
    uint8_t shiftRegisterContents = 0;

    digitalWrite(loadPin, LOW);
    delayMicroseconds(1);
    digitalWrite(loadPin, HIGH);
    delayMicroseconds(1);

    for (int i = 0; i < 8; i++) {
        shiftRegisterContents <<= 1;

        shiftRegisterContents |= digitalRead(dataPin);

        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);
    }

    return shiftRegisterContents;
}