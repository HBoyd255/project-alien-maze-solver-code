/**
 * @file angleAndPosition.h
 * @brief Declaration for the Angle class and the Position and Pose Structs.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */
#ifndef ANGLE_AND_POSITION_H
#define ANGLE_AND_POSITION_H

#include <Arduino.h>

// The number of degrees in a full rotation.
#define DEGREES_PER_ROTATION 360

/**
 * @brief Angle class, a wrapper for the int16_t that automatically wraps the
 * value to between -179 and 180.
 *
 * Instances of this class can be implicitly converted to int16_t.
 *
 */
class Angle {
   public:
    /**
     * @brief Angle Constructor.
     *
     * @param value the provided int16_t to set the value to, this value gets
     * wrapped between -179 and 180.
     */
    Angle(int16_t value);

    /**
     * @brief Implicitly converts the angle into a int16_t
     *
     * @return The value of the angle in the form of a int16_t.
     */
    operator int16_t() const;

    /**
     * @brief Overloaded addition operator, used to add two angles together.
     *
     * @param otherAngle The angle (or uint16_t via implicit conversion) to sum
     * with the current angle.
     * @return The sum of the provided angles.
     */
    Angle operator+(Angle& otherAngle) const;

    /**
     * @brief Overloaded compound addition operator, used to add an angle to the
     * current angle.
     *
     * @param valueToAdd The angle (or uint16_t) to add to the current angle.
     * @return reference to the modified angle.
     */
    Angle operator+=(int16_t valueToAdd);

    /**
     * @brief Overloaded subtraction operator, used to subtract one angle from
     * another.
     *
     * @param otherAngle The angle (or uint16_t) to subtract from the current
     * angle.
     * @return The difference between the provided angles.
     */
    Angle operator-(Angle& otherAngle) const;

    /**
     * @brief Overloaded compound subtraction operator, used to subtract an
     * angle from the current angle.
     *
     * @param valueToSub The angle (or uint16_t) to add to subtract from the
     * current angle.
     * @return reference to the modified angle.
     */
    Angle operator-=(int16_t valueToSub);

    /**
     * @brief Retuns the angles equivelent value in radians.
     *
     * @return (double) The value of the angle in radians.
     */
    double getRadians();

    /**
     * @brief returns the closest multiple of 90 of this angle.
     *
     * @return (Angle) The closest right angle
     */
    Angle closestRightAngle();

    /**
     * @brief Calculates the difference between the current angle and the
     * closest right angle.
     *
     * @return (Angle) The difference between the current angle and the
     * closest right angle.
     */
    Angle OrthogonalOffset();

    /**
     * @brief Returns if the current angle is at a right angle, or within a
     * provided tolerance of a right angle.
     *
     * @param tolerance the inclusive acceptable tolerance to the closest right
     * angle.
     * @return (true) If the absolute offset is higher than the tolerance.
     * @return (false) If the absolute offset is is lower than the tolerance.
     */
    bool isOrthogonal(int tolerance = 0);

    /**
     * @brief Returns whether or not the angle is pointing down by checking
     * if closestRightAngle() is equal to -90°.
     *
     * @return (true) if the closest right angle is equal to -90°.
     * @return (false) if the closest right angle is not equal to -90°.
     */
    bool isPointingDown();

    /**
     * @brief Returns whether or not the angle is pointing left by checking
     * if closestRightAngle() is equal to 180°.
     *
     * @return (true) if the closest right angle is equal to 180°.
     * @return (false) if the closest right angle is not equal to 180°.
     */
    bool isPointingLeft();

    /**
     * @brief Returns whether  or not the angle is pointing up by checking
     * if closestRightAngle() is equal to 90°.
     *
     * @return (true) if the closest right angle is equal to 90°.
     * @return (false) if the closest right angle is not equal to 90°.
     */
    bool isPointingUp();

    /**
     * @brief Returns whether  or not the angle is pointing right by checking
     * if closestRightAngle() is equal to 0°.
     *
     * @return (true) if the closest right angle is equal to 0°.
     * @return (false) if the closest right angle is not equal to 0°.
     */
    bool isPointingRight();

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
    uint16_t segmentIndex(uint16_t segmentCount);

   private:
    /**
     * @brief the value of the angle, a int16_t wrapped between -179 and 180.
     */
    int16_t _value;

    /**
     * @brief wraps a provided int16_t between -179 and 180.
     *
     * @param valueToNormalize the value to wrap between -179 and 180.
     * @return int16_t the value after being wrapped between -179 and 180.
     */
    int16_t _normalize(int16_t valueToNormalize);

    /**
     * @brief returns the angle but within the range 0-359
     *
     * @return uint8_t angle wrapped to 0-359.
     */
    uint16_t _get360();
};

// Forward declaration of the Pose and Position structs, so that they can be
// used as arguments for methods of the Position struct;
struct Pose;
struct Position;

/**
 * @brief Struct for storing a position, where x and y represent
 * millimetres, by default set to (0,0)
 *
 */
struct Position {
    /**
     * @brief The x position, measured in millimetres, set to 0 by default.
     */
    float x = 0;
    /**
     * @brief The y position, measured in millimetres, set to 0 by default.
     */
    float y = 0;

    /**
     * @brief Construct a new Position object
     *
     * @param x The x position in millimetres.
     * @param y The y position in millimetres.
     */
    Position(float x = 0, float y = 0);

    /**
     * @brief Rotates the position around (0,0) by a given angle in degreed.
     *
     * @param rotationAngle The angle to rotate by.
     * @return (Position&) A reference to the modified object.
     */
    Position& rotate(Angle rotationAngle);

    /**
     * @brief Offsets the position around by a given position.
     *
     * @param offset The position to offset the current position by
     * @return (Position&) A reference to the modified object.
     */
    Position& offset(Position offset);

    /**
     * @brief Transforms the position by a given pose, constructed of a
     * position and an angle.
     *
     * The Position is first rotated by the given pose's angle, and then
     * offset by the pose's position.
     *
     * @param offsetPose (Pose) The pose to transform the position by.
     */
    void transformByPose(Pose offsetPose);

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
    float squaredDistanceTo(Position target, int* dx_P = nullptr,
                            int* dy_P = nullptr);

    /**
     * @brief Calculates the euclidean distance between this position and a
     * provided target position using the pythagorean theorem.
     *
     * @param target The other position to calculate the distance to.
     * @return (float) The euclidean distance to the target position in
     * millimeters.
     */
    float distanceTo(Position target);

    /**
     * @brief Calculates the angle from this position to a given target
     * position
     *
     * @param target The other position to calculate the angle to.
     * @return (Angle) The angle in degrees from this position to the target
     * position.
     */
    Angle calculateAngleTo(Position target);

    /**
     * @brief Converts the Position into a string, in the format (X,Y).
     *
     * @return (String) The Position as a string.
     */
    operator String() const;

    /**
     * @brief Overloaded addition operator, used to add two Positions
     * together by summing their x and y components.
     * @param positionToAdd The position on the right hand side of the
     * operator.
     * @return The Position created by summing the two provided Positions.
     */
    Position operator+(Position& positionToAdd) const;

    /**
     * @brief Overloaded compound addition operator, used to add a Position
     * to the current Position by summing their x and y components.
     *
     * @param positionToAdd The Position to add to the current Position.
     * @return reference to the modified Position.
     */
    Position& operator+=(Position& positionToAdd);
};

/**
 * @brief Struct for storing a 2D pose, consisting of a position(x and y
 * measured in millimetres) and an angle(measured in degrees);
 */
struct Pose {
    /**
     * @brief The position of the Pose,(x and y measured in millimetres),
     * set to (0,0) by default.
     */
    Position position = {0, 0};
    /**
     * @brief The Angle of the Pose,(measured in degrees), set to 0 by
     * default.
     */
    Angle angle = 0;
    /**
     * @brief Converts the Pose into a string, in the format (X,Y)@Angle°.
     *
     * @return (String) The Pose as a string.
     */
    operator String() const;
};

#endif  // ANGLE_AND_POSITION_H