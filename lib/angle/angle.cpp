/**
 * @file angle.h
 * @brief Definition of the Angle class.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-12
 * @copyright Copyright (c) 2023
 */

#include "angle.h"

// The lower bound of the angle wrap.
#define ANGLE_OFFSET -179

// The number of degrees in a full rotation.
#define DEGREES_PER_ROTATION 360

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
 * @brief wraps a provided int16_t between -179 and 180.
 *
 * @param valueToNormalize the value to wrap between -179 and 180.
 * @return int16_t the value after being wrapped between -179 and 180.
 */
int16_t Angle::_normalize(int16_t valueToNormalize) {
    valueToNormalize -= ANGLE_OFFSET;

    valueToNormalize %= DEGREES_PER_ROTATION;

    if (valueToNormalize < 0) {
        valueToNormalize += DEGREES_PER_ROTATION;
    }
    valueToNormalize += ANGLE_OFFSET;

    return valueToNormalize;
};
