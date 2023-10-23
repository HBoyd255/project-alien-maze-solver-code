
#include "bumperControl.h"

#include <Arduino.h>


Bumper::Bumper(uint8_t data, uint8_t load, uint8_t clock) {
    this->data = data;
    this->load = load;
    this->clock = clock;

    this->currentValue = 0;
    this->previousValue = 0;

    pinMode(data, INPUT);
    pinMode(load, OUTPUT);
    pinMode(clock, OUTPUT);

    digitalWrite(load, LOW);
    digitalWrite(clock, LOW);
}

// Bumper* Bumper::setup(uint8_t data, uint8_t load, uint8_t clock) {
//     return new Bumper(data, load, clock);
// }

uint8_t Bumper::getInstant() {
    uint8_t shiftRegisterContents = 0;

    digitalWrite(load, LOW);
    delayMicroseconds(1);
    digitalWrite(load, HIGH);
    delayMicroseconds(1);

    for (int i = 0; i < 8; i++) {
        shiftRegisterContents <<= 1;

        shiftRegisterContents |= digitalRead(data);

        digitalWrite(clock, LOW);
        delayMicroseconds(1);
        digitalWrite(clock, HIGH);
        delayMicroseconds(1);
    }

    return shiftRegisterContents;
}