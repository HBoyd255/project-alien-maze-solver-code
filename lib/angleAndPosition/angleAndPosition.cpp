/**
 * @file angleAndPosition.cpp
 * @brief Definitions for the Angle class and the Position and Pose Structs.
 *
 * @author Harry Boyd - github.com/HBoyd255
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
Angle::operator int16_t() const { return this->_value; }

/**
 * @brief Overloaded addition operator, used to add two angles together.
 *
 * @param otherAngle The angle (or uint16_t via implicit conversion) to sum
 * with the current angle.
 * @return The sum of the provided angles.
 */
Angle Angle::operator+(Angle& otherAngle) const {
    return Angle(this->_value + otherAngle._value);
}
/**
 * @brief Overloaded compound addition operator, used to add an angle to the
 * current angle.
 *
 * @param valueToAdd The angle (or uint16_t) to add to the current angle.
 * @return reference to the modified angle.
 */
Angle Angle ::operator+=(int16_t valueToAdd) {
    this->_value = _normalize(this->_value + valueToAdd);
    return *this;
}

/**
 * @brief Overloaded subtraction operator, used to subtract one angle from
 * another.
 *
 * @param otherAngle The angle (or uint16_t) to subtract from the current
 * angle.
 * @return The difference between the provided angles.
 */
Angle Angle::operator-(Angle& otherAngle) const {
    return Angle(this->_value - otherAngle._value);
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
    this->_value = _normalize(this->_value - valueToSub);
    return *this;
}

/**
 * @brief Retuns the angle's equivelent value in radians.
 *
 * @return (double) The value of the angle in radians.
 */
double Angle::getRadians() { return radians(this->_value); }

/**
 * @brief returns the closest multiple of 90 of this angle.
 *
 * @return (Angle) The closest right angle
 */
Angle Angle::closestRightAngle() {
    int tempValue = this->_get360();

    tempValue += 45;

    int index = (tempValue / 90);

    int closest90 = index * 90;

    Angle angleToReturn = (Angle)closest90;

    return angleToReturn;
}

/**
 * @brief Calculates the difference between the current angle and the
 * closest right angle.
 *
 * @return (Angle) The difference between the current angle and the
 * closest right angle.
 */
Angle Angle::OrthogonalOffset() {
    Angle OrthogonalOffset = this->closestRightAngle() - *this;
    return OrthogonalOffset;
}

/**
 * @brief Returns if the current angle is at a right angle, or within a
 * provided tolerance of a right angle.
 *
 * @param tolerance the inclusive acceptable tolerance to the closest right
 * angle.
 * @return (true) If the absolute offset is higher than the tolerance.
 * @return (false) If the absolute offset is is lower than the tolerance.
 */
bool Angle::isOrthogonal(int tolerance) {
    bool isOrthogonal = (abs((int)this->OrthogonalOffset()) <= tolerance);

    return isOrthogonal;
}

/**
 * @brief Returns whether or not the angle is pointing down by checking
 * if closestRightAngle() is equal to -90°.
 *
 * @return (true) if the closest right angle is equal to -90°.
 * @return (false) if the closest right angle is not equal to -90°.
 */
bool Angle::isPointingDown() { return (this->closestRightAngle() == -90); }

/**
 * @brief Returns whether or not the angle is pointing left by checking
 * if closestRightAngle() is equal to 180°.
 *
 * @return (true) if the closest right angle is equal to 180°.
 * @return (false) if the closest right angle is not equal to 180°.
 */
bool Angle::isPointingLeft() { return (this->closestRightAngle() == 180); }

/**
 * @brief Returns whether  or not the angle is pointing up by checking
 * if closestRightAngle() is equal to 90°.
 *
 * @return (true) if the closest right angle is equal to 90°.
 * @return (false) if the closest right angle is not equal to 90°.
 */
bool Angle::isPointingUp() { return (this->closestRightAngle() == 90); }

/**
 * @brief Returns whether  or not the angle is pointing right by checking
 * if closestRightAngle() is equal to 0°.
 *
 * @return (true) if the closest right angle is equal to 0°.
 * @return (false) if the closest right angle is not equal to 0°.
 */
bool Angle::isPointingRight() { return (this->closestRightAngle() == 0); }

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
    uint16_t unwrappedAngle = this->_get360();

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
 * @brief returns the angle but within the range 0-359
 *
 * @return uint8_t angle wrapped to 0-359.
 */
uint16_t Angle::_get360() {
    if (_value < 0) {
        return _value + DEGREES_PER_ROTATION;
    } else {
        return _value;
    }
}

/**
 * @brief Construct a new Position object
 *
 * @param x The x position in millimetres.
 * @param y The y position in millimetres.
 */
Position::Position(float x, float y) : x(x), y(y) {}

/**
 * @brief Rotates the position around (0,0) by a given angle in degreed.
 *
 * @param rotationAngle The angle to rotate by.
 * @return Position& returns a reference to the modified object;
 */
Position& Position::rotate(Angle rotationAngle) {
    float sinAngle = sin(rotationAngle.getRadians());
    float cosAngle = cos(rotationAngle.getRadians());

    float tempX = this->x * cosAngle - this->y * sinAngle;
    float tempY = this->y * cosAngle + this->x * sinAngle;

    this->x = tempX;
    this->y = tempY;

    return *this;
}

/**
 * @brief Offsets the position around by a given position.
 *
 * @param offset The position to offset the current position by
 * @return (Position&) A reference to the modified object.
 */
Position& Position::offset(Position offset) {
    *this += offset;
    return *this;
}

/**
 * @brief Transforms the position by a given pose, constructed of a position
 * and an angle.
 *
 * The Position is first rotated by the given pose's angle, and then offset
 * by the pose's position.
 *
 * @param offsetPose (Pose) The pose to transform the position by.
 */
void Position::transformByPose(Pose offsetPose) {
    this->rotate(offsetPose.angle - 90);

    this->offset(offsetPose.position);
}

/**
 * @brief Calculates the square of the euclidean distance between this
 * position and a provided target position.
 *
 * This function uses less computation than distanceTo(), as no
 * square root required. For basic comparisons between two distances,
 * this function is more efficient.
 *
 * c * c == a * a + b * b
 * is a lot easier to compute than
 * c = (a * a + b * b) ^ 0.5
 *
 * @param target The other position to calculate the squared distance
 * @param dx_P A pointer to return the difference in x positions.
 * @param dy_P A pointer to return the difference in y positions.
 * @return (float) The square of the euclidean distance to the target
 * position in millimeters.
 */
float Position::squaredDistanceTo(Position target, int* dx_P, int* dy_P) {
    float dx = target.x - this->x;
    float dy = target.y - this->y;

    *dx_P = dx;
    *dy_P = dy;

    float squaredDistance = dx * dx + dy * dy;

    return squaredDistance;
}

/**
 * @brief Calculates the euclidean distance between this position and a
 * provided target position using the pythagorean theorem.
 *
 * @param target The other position to calculate the distance to.
 * @return (float) The euclidean distance to the target position in
 * millimeters.
 */
float Position::distanceTo(Position target) {
    // a, b and cSquared are from the Pythagorean theorem.
    // A^2 + B^2 = C^2,
    // If A == 0, C = B
    // If B == 0, C = A

    int a = -1;  // -1 represents an uninitialized state.
    int b = -1;
    int c;

    // calculate the squared distance from this brick to the target, and get the
    // x and y difference by reference.
    int cSquared = this->squaredDistanceTo(target, &a, &b);

    // A^2 + B^2 = C^2,

    // If a is 0,(and b is initialized) c is equal to b.
    if ((a == 0) && (b != -1)) {
        c = b;
    }
    // If b is 0, c is equal to a.
    else if ((b == 0) && (a != -1)) {
        c = a;
    }
    // If a and b are both not zero, calculate c
    else {
        c = sqrt(cSquared);
    }

    return c;
}

/**
 * @brief Calculates the angle from this position to a given target position
 *
 * @param target The other position to calculate the angle to.
 * @return (Angle) The angle in degrees from this position to the target
 * position.
 */
Angle Position::calculateAngleTo(Position target) {
    int dx = target.x - this->x;
    int dy = target.y - this->y;

    Angle angleToOtherPosition = degrees(atan2(dy, dx));

    return angleToOtherPosition;
}

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
 * @brief Overloaded addition operator, used to add two Positions together
 * by summing their x and y components.
 * @param positionToAdd The position on the right hand side of the
 * operator.
 * @return The Position created by summing the two provided Positions.
 */
Position Position::operator+(Position& positionToAdd) const {
    return Position(this->x + positionToAdd.x, this->y + positionToAdd.y);
}

/**
 * @brief Overloaded compound addition operator, used to add a Position to
 * the current Position by summing their x and y components.
 *
 * @param positionToAdd The Position to add to the current Position.
 * @return reference to the modified Position.
 */
Position& Position::operator+=(Position& positionToAdd) {
    this->x += positionToAdd.x;
    this->y += positionToAdd.y;
    return *this;
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