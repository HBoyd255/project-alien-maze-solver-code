

#include "binary.h"

void printByte(uint8_t byte) {
    for (int b = 7; b; b--) {
        Serial.print((byte >> b) & 1);
        Serial.print(",");
    }
    Serial.println(byte & 1);
}