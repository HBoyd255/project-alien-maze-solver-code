

#include "vectorUpgrades.h"

void printAngleVector(AngleVector& vectorToPrint) {
    uint16_t itemCount = vectorToPrint.size();

    Serial.print("Angle Vector Containing ");
    Serial.print(itemCount);
    Serial.print(" Items - ");

    for (const auto& item : vectorToPrint) {
        Serial.print(item);
        Serial.print(",");
    }
    Serial.println();
}
