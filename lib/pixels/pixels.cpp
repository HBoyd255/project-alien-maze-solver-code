
#include "pixels.h"

#include <regex>

#include "errorIndicator.h"

bool isValidHexColour(String stringToValidate) {
    const std::regex colourPattern(R"(^#([0-9A-Fa-f]{6})$)");
    return std::regex_match(stringToValidate.c_str(), colourPattern);
}

Colour::Colour(byte red, byte green, byte blue)
    : red(red), green(green), blue(blue) {}

Colour::Colour(String nameOrHex) {
    String lowercaseName = nameOrHex;
    lowercaseName.toLowerCase();

    if (lowercaseName == "red")
        this->set(255, 0, 0);
    else if (lowercaseName == "orange")
        this->set(255, 127, 0);
    else if (lowercaseName == "yellow")
        this->set(255, 255, 0);
    else if (lowercaseName == "green")
        this->set(0, 255, 0);
    else if (lowercaseName == "blue")
        this->set(0, 0, 255);
    else if (lowercaseName == "magenta")
        this->set(0, 0, 255);
    else if (lowercaseName == "cyan")
        this->set(0, 255, 255);
    else if (lowercaseName == "pink")
        this->set(255, 87, 51);
    else if (lowercaseName == "white")
        this->set(255, 255, 255);
    else if (lowercaseName == "black")
        this->set(0, 0, 0);

    else if (isValidHexColour(lowercaseName)) {
        this->red = strtoul(lowercaseName.substring(1, 3).c_str(), NULL, 16);
        this->green = strtoul(lowercaseName.substring(3, 5).c_str(), NULL, 16);
        this->blue = strtoul(lowercaseName.substring(5, 7).c_str(), NULL, 16);
    } else {
        String ErrorMessage = "";
        ErrorMessage += "Cannot interpret \"";
        ErrorMessage += nameOrHex;
        ErrorMessage += "\" as a colour.";
        ErrorIndicator_G.errorOccurred(__FILE__, __LINE__, ErrorMessage);
    }
}

void Colour::set(byte red, byte green, byte blue) {
    this->red = red;
    this->green = green;
    this->blue = blue;
}

String Colour::toString() {
    String stringToReturn = "";

    stringToReturn += "RBG(";
    stringToReturn += this->red;
    stringToReturn += ",";
    stringToReturn += this->green;
    stringToReturn += ",";
    stringToReturn += this->blue;
    stringToReturn += ")";

    return stringToReturn;
}

Pixels::Pixels(uint8_t dataPin, uint8_t ledCount, Angle rotationOffset)
    : _ledCount(ledCount),
      _ledStrip(_ledCount, dataPin),
      _rotationOffset(rotationOffset) {}

void Pixels::setup() {
    this->_ledStrip.Begin();
    this->_ledStrip.Show();
}

void Pixels::setPixel(int index, Colour colour, bool show) {
    byte r = colour.red;
    byte g = colour.green;
    byte b = colour.blue;

    this->_ledStrip.SetPixelColor(index, RgbColor(r, g, b));

    if (show) {
        this->show();
    }
}

void Pixels::setAll(Colour colour, bool show) {
    for (uint8_t i = 0; i < this->_ledCount; i++) {
        this->setPixel(i, colour, show);
    }
}

// This function will need to be called routinely to keep the LEDs flashing,
// otherwise the LEDs will stop flashing and freeze on the last state.
void Pixels::flash(Colour onColour, bool alternate, int period,
                   Colour offColour) {
    // Set the toggle to true if the current time is withing the second half
    // of a given period.
    bool toggle = (millis() * 2 / period) % 2;

    // Clear the LEDs.
    this->clear();

    // For every LED in the strip,
    for (int led_I = 0; led_I < this->_ledCount; led_I++) {
        // Initial set the LED state to the toggle boolean. This will set
        // the LED to on if in the first half of the period and off if in
        // the second.
        bool ledState = toggle;

        // Id the alternate boolean is true, then invert all the odd LEDs.
        bool invertLed = (led_I & 1) && alternate;

        // Invert the LED state if required.
        ledState ^= invertLed;

        // Set the colour of the current LED.
        if (ledState) {
            this->setPixel(led_I, onColour);
        } else {
            this->setPixel(led_I, offColour);
        }
    }

    // Show the LEDs.
    this->show();
}

void Pixels::clear(bool show) { this->setAll(Colour("Black"), show); }

void Pixels::show() { this->_ledStrip.Show(); }

void Pixels::point(Angle angle) {
    angle += this->_rotationOffset;
    uint8_t index = angle.segmentIndex(this->_ledCount);

    this->clear();
    this->setPixel(index, Colour("Red"), true);
}

uint16_t Pixels::getLedCount() { return this->_ledCount; }
