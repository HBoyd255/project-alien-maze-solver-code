

#include "errorIndicator.h"

#define ERROR_INDICATOR_DELAY 500

ErrorIndicator::ErrorIndicator(uint8_t ledPin) { this->_ledPin = ledPin; }

void ErrorIndicator::setup() { pinMode(this->_ledPin, OUTPUT); }

void ErrorIndicator::assignPixels(Pixels* pixelsPtr) {
    this->pixelsPtr = pixelsPtr;
}
void ErrorIndicator::assignDrive(Drive* drivePtr) { this->drivePtr = drivePtr; }

void ErrorIndicator::errorOccurred(String errorMessage) {
    if (this->drivePtr != NULL) {
        this->drivePtr->stop();
    }

    bool serialAvailability = (bool)Serial;
    bool oldSerialAvailability = serialAvailability;

    bool pixelsAvailability = this->pixelsPtr != NULL;

    Serial.begin(SERIAL_BAUD_RATE);

    bool ledToggle = false;

    while (true) {
        if (pixelsAvailability && !serialAvailability) {
            this->pixelsPtr->clear();
            for (int i = 0; i < 8; i++) {
                this->pixelsPtr->setPixel((i * 2) + ledToggle, 255, 0, 0);
            }
            this->pixelsPtr->show();

        } else if (pixelsAvailability && serialAvailability) {
            pixelsPtr->clear(true);
        }

        if (serialAvailability && !oldSerialAvailability) {
            Serial.println("Error Occurred!");
            Serial.println(errorMessage);
        }

        digitalWrite(LED_BUILTIN, ledToggle);

        delay(ERROR_INDICATOR_DELAY);

        ledToggle = !ledToggle;

        oldSerialAvailability = serialAvailability;
        serialAvailability = (bool)Serial;
    }
}
