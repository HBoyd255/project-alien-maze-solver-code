

#include "brick.h"

const String rotationStrings[2] = {"horizontal", "vertical"};

/**
 * @brief Converts the Pose into a string.
 *
 * @return (String) The Pose as a string, in the format (X,Y)@Angle°
 */
Brick::operator String() const {
    String stringToReturn;
    stringToReturn += "Brick[";
    stringToReturn += this->position;
    stringToReturn += ",";
    stringToReturn += rotationStrings[this->isVertical];
    stringToReturn += "]";
    return stringToReturn;
}