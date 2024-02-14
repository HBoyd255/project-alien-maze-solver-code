
#include "pixels.h"

#include <regex>

bool isValidHexColour(String stringToValidate) {
    const std::regex colourPattern(R"(^#([0-9A-Fa-f]{6})$)");
    return std::regex_match(stringToValidate.c_str(), colourPattern);
}

Colour::Colour(byte red, byte green, byte blue)
    : red(red), green(green), blue(blue) {}

Colour::Colour(String nameOrHex) {
    nameOrHex.toLowerCase();

    if (nameOrHex == "red")
        this->set(255, 0, 0);
    else if (nameOrHex == "orange")
        this->set(255, 127, 0);
    else if (nameOrHex == "yellow")
        this->set(255, 255, 0);
    else if (nameOrHex == "green")
        this->set(0, 255, 0);
    else if (nameOrHex == "blue")
        this->set(0, 0, 255);
    else if (nameOrHex == "magenta")
        this->set(0, 0, 255);
    else if (nameOrHex == "cyan")
        this->set(0, 255, 255);
    else if (nameOrHex == "pink")
        this->set(255, 87, 51);
    else if (nameOrHex == "white")
        this->set(255, 255, 255);
    else if (nameOrHex == "black")
        this->set(0, 0, 0);

    else if (!isValidHexColour(nameOrHex)) {
        this->set(0, 0, 0);

        // TODO improve error handling
        while (true) {
            String ErrorMessage = "";
            ErrorMessage += "Cannot interpret ";
            ErrorMessage += nameOrHex;
            ErrorMessage += " as a colour.";
            Serial.println(ErrorMessage);
        }
    } else {
        red = strtoul(nameOrHex.substring(1, 3).c_str(), NULL, 16);
        green = strtoul(nameOrHex.substring(3, 5).c_str(), NULL, 16);
        blue = strtoul(nameOrHex.substring(5, 7).c_str(), NULL, 16);
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

void Pixels::clear(bool show) { this->setAll(Colour("Black"), show); }

void Pixels::show() { this->_ledStrip.Show(); }

void Pixels::point(Angle angle) {
    angle += this->_rotationOffset;
    uint8_t index = angle.segmentIndex(this->_ledCount);

    this->clear();
    this->setPixel(index, Colour("Red"), true);
}

uint16_t Pixels::getLedCount() { return this->_ledCount; }
//
// Pixels pixelsTester(5u, 16, 180);
//
// void pixelsTest_setup() { pixelsTester.setup(); }
//
// void pixelsTest_loop() {
//     Colour colourA = Colour(255, 255, 0);
//     Serial.println(colourA.toString());
//     pixelsTester.setPixel(0, colourA);
//
//     Colour colourB = Colour("#FF00FF");
//     Serial.println(colourB.toString());
//     pixelsTester.setPixel(1, colourB);
//
//     Colour colourC = Colour("Yellow");
//     Serial.println(colourC.toString());
//     pixelsTester.setPixel(2, colourC);
//
//     Colour colourD = Colour("GreeN");
//     Serial.println(colourD.toString());
//     pixelsTester.setPixel(3, colourD);
//
//     Colour colourE = Colour("#FFFFFF");
//     Serial.println(colourE.toString());
//     pixelsTester.setPixel(4, colourE);
//
//     pixelsTester.show();
//
//     Serial.println("AHHHHH");
// }
//
// void pixelsTest() {
//     pixelsTest_setup();
//
//     while (true) {
//         void pixelsTest_loop();
//     }
// }
