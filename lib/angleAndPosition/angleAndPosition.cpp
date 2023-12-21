/**
 * @file angleAndPosition.cpp
 * @brief Definitions for the Angle class and the Position and Pose Structs.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */

#include "angleAndPosition.h"

// The lower bound of the angle wrap.
#define ANGLE_LOWER_BOUND -179

/**
 * @brief Angle Constructor.
 *
 * @param value the provided int16_t to set the value to, this value gets
 * wrapped between -179 and 180.
 */
Angle::Angle(int16_t value) : _value(_normalize(value)) {}

/**
 * @brief Implicitly converts the angle into a int16_t
 *
 * @return The value of the angle in the form of a int16_t.
 */
Angle::operator int16_t() const { return _value; }

/**
 * @brief Overloaded addition operator, used to add two angles together.
 *
 * @param otherAngle The angle (or uint16_t via implicit conversion) to sum
 * with the current angle.
 * @return The sum of the provided angles.
 */
Angle Angle::operator+(Angle& otherAngle) const {
    return Angle(_value + otherAngle._value);
}
/**
 * @brief Overloaded compound addition operator, used to add an angle to the
 * current angle.
 *
 * @param valueToAdd The angle (or uint16_t) to add to the current angle.
 * @return reference to the modified angle.
 */
Angle Angle ::operator+=(int16_t valueToAdd) {
    _value = _normalize(_value + valueToAdd);
    return *this;
}

/**
 * @brief Overloaded subtraction operator, used to subtract one angle from
 * another.
 *
 * @param otherAngle The angle (or uint16_t) to subtract from the current
 * angle.
 * @return The sum of the provided angles.
 */
Angle Angle::operator-(Angle& otherAngle) const {
    return Angle(_value - otherAngle._value);
}

/**
 * @brief Overloaded compound subtraction operator, used to subtract an
 * angle from the current angle.
 *
 * @param valueToSub The angle (or uint16_t) to add to subtract from the
 * current angle.
 * @return reference to the modified angle.
 */
Angle Angle::operator-=(int16_t valueToSub) {
    _value = _normalize(_value - valueToSub);
    return *this;
}

/**
 * @brief returns the angle but within the range 0-359
 *
 * @return uint8_t angle wrapped to 0-359.
 */
uint16_t Angle::get360() {
    if (_value < 0) {
        return _value + DEGREES_PER_ROTATION;
    } else {
        return _value;
    }
}

/**
 * @brief Returns the index of the segments at the current angle.
 *
 * for example for the angles 0 , 45, 90,
 * segmentIndex(90) would return 0, 0, 1 respectively,
 * segmentIndex(45) would return 0, 1, 2 respectively,
 * and segmentIndex(10) would return 0, 4, 9
 *
 * @param segmentCount the number of segments the the hypothetical circle is
 * being divided into.
 * @return (uint16_t) The index of the hypothetical segment.
 */
uint16_t Angle::segmentIndex(uint16_t segmentCount) {
    uint16_t unwrappedAngle = this->get360();

    uint16_t segmentIndex =
        unwrappedAngle * segmentCount / DEGREES_PER_ROTATION;

    return segmentIndex;
}

/**
 * @brief wraps a provided int16_t between -179 and 180.
 *
 * @param valueToNormalize the value to wrap between -179 and 180.
 * @return int16_t the value after being wrapped between -179 and 180.
 */
int16_t Angle::_normalize(int16_t valueToNormalize) {
    valueToNormalize -= ANGLE_LOWER_BOUND;

    valueToNormalize %= DEGREES_PER_ROTATION;

    if (valueToNormalize < 0) {
        valueToNormalize += DEGREES_PER_ROTATION;
    }
    valueToNormalize += ANGLE_LOWER_BOUND;

    return valueToNormalize;
};

/**
 * @brief Converts the Position into a string, in the format (X,Y).
 *
 * @return (String) The Position as a string.
 */
Position::operator String() const {
    String stringToReturn = "(";
    stringToReturn += (int)this->x;
    stringToReturn += ",";
    stringToReturn += (int)this->y;
    stringToReturn += ")";
    return stringToReturn;
}

/**
 * @brief Converts the Pose into a string.
 *
 * @return (String) The Pose as a string, in the format (X,Y)@Angle°
 */
Pose::operator String() const {
    String stringToReturn;
    stringToReturn += this->position;
    stringToReturn += "@";
    stringToReturn += this->angle;
    stringToReturn += "°";
    return stringToReturn;
}