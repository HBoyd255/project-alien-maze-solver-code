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
 * @return The sum of the provided angles.
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
double Angle::toRadians() { return radians(this->_value); }

Angle Angle::closestOrthogonal() {
    int tempValue = this->_get360();

    tempValue += 45;

    int index = (tempValue / 90);

    int closest90 = index * 90;

    Angle angleToReturn = (Angle)closest90;

    return angleToReturn;
}

Angle Angle::OrthogonalOffset() {
    Angle OrthogonalOffset = this->closestOrthogonal() - *this;
    return OrthogonalOffset;
}

bool Angle::isOrthogonal(int tolerance) {
    bool isOrthogonal = (abs((int)this->OrthogonalOffset()) <= tolerance);

    return isOrthogonal;
}

bool Angle::isOrthogonallyDown(int tolerance) {
    const int targetAngle = -90;

    bool isOrthogonal = this->isOrthogonal(tolerance);

    bool pointingDown = this->closestOrthogonal() == targetAngle;

    return isOrthogonal && pointingDown;
}
bool Angle::isOrthogonallyLeft(int tolerance) {
    const int targetAngle = 180;

    bool isOrthogonal = this->isOrthogonal(tolerance);

    bool pointingLeft = this->closestOrthogonal() == targetAngle;

    return isOrthogonal && pointingLeft;
}
bool Angle::isOrthogonallyUp(int tolerance) {
    const int targetAngle = 90;

    bool isOrthogonal = this->isOrthogonal(tolerance);

    bool pointingUp = this->closestOrthogonal() == targetAngle;

    return isOrthogonal && pointingUp;
}
bool Angle::isOrthogonallyRight(int tolerance) {
    const int targetAngle = 0;

    bool isOrthogonal = this->isOrthogonal(tolerance);

    bool pointingRight = this->closestOrthogonal() == targetAngle;

    return isOrthogonal && pointingRight;
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
 * @brief Transforms the position by a given pose, constructed of a position
 * and an angle.
 *
 * The Position is first rotated by the given pose's angle, and then offset
 * by the pose's position.
 *
 * @param offsetPose (Pose) The pose to transform the position by.
 */
void Position::transformByPose(Pose offsetPose) {
    Angle angleToRotate = offsetPose.angle - 90;

    float sinAngle = sin(angleToRotate.toRadians());
    float cosAngle = cos(angleToRotate.toRadians());

    float tempX = this->x * cosAngle - this->y * sinAngle;
    float tempY = this->y * cosAngle + this->x * sinAngle;

    tempX += offsetPose.position.x;
    tempY += offsetPose.position.y;

    this->x = tempX;
    this->y = tempY;
}

/**
 * @brief Calculates the euclidean distance between this position and a
 * provided target position.
 *
 * @param target The other position to calculate the distance to.
 * @return (int) The euclidean distance to the target position in
 * millimeters.
 */
int Position::calculateDistanceTo(Position target) {
    return sqrt(this->calculateSquaredDistanceTo(target));
}

/**
 * @brief Calculates the square of the euclidean distance between this
 * position and a provided target position.
 *
 * This function uses less computation than calculateDistanceTo(), as no
 * square root required. For basic comparisons between two distances, this
 * function will suffice.
 *
 * c * c = a * a + b * b
 * is a lot easier to compute than
 * c = (a * a + b * b) ^ 0.5
 *
 * @param target The other position to calculate the squared distance to.
 * @return (int) The square of the euclidean distance to the target position
 * in millimeters.
 */
int Position::calculateSquaredDistanceTo(Position target) {
    int dx = target.x - this->x;
    int dy = target.y - this->y;

    int squaredDistance = dx * dx + dy * dy;

    return squaredDistance;
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