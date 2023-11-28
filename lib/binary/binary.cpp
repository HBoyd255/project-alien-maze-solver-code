

#include "binary.hpp"

void printByte(uint8_t byteToPrint) {
    for (uint8_t b = 7; b; b--) {
        Serial.print((byteToPrint >> b) & 1);
        Serial.print(",");
    }
    Serial.println(byteToPrint & 1);
}