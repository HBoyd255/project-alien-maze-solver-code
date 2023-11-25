

#include "binary.h"

void printByte(byte byteToPrint) {
    for (int b = 7; b; b--) {
        Serial.print((byteToPrint >> b) & 1);
        Serial.print(",");
    }
    Serial.println(byteToPrint & 1);
}