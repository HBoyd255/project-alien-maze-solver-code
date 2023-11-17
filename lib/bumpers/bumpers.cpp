
#include "bumpers.h"

#include <Arduino.h>

Bumper::Bumper() {}

void Bumper::setup() {
    pinMode(BUMPERS_SHIFT_REG_DATA, INPUT);
    pinMode(BUMPERS_SHIFT_REG_LOAD, OUTPUT);
    pinMode(BUMPERS_SHIFT_REG_CLOCK, OUTPUT);
}

void Bumper::assignCallback(voidFuncPtr function) {
    attachInterrupt(digitalPinToInterrupt(BUMPERS_INTERRUPT_PIN), function,
                    CHANGE);
}

uint8_t Bumper::read() {
    uint8_t shiftRegisterContents = 0;

    digitalWrite(BUMPERS_SHIFT_REG_LOAD, LOW);
    delayMicroseconds(1);
    digitalWrite(BUMPERS_SHIFT_REG_LOAD, HIGH);
    delayMicroseconds(1);

    for (int i = 0; i < 8; i++) {
        shiftRegisterContents <<= 1;

        shiftRegisterContents |= digitalRead(BUMPERS_SHIFT_REG_DATA);

        digitalWrite(BUMPERS_SHIFT_REG_CLOCK, LOW);
        delayMicroseconds(1);
        digitalWrite(BUMPERS_SHIFT_REG_CLOCK, HIGH);
        delayMicroseconds(1);
    }

    return shiftRegisterContents;
}