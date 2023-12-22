/**
 * @file angleAndPosition.h
 * @brief Declaration for the Angle class and the Position and Pose Structs.
 *
 * @author Harry Boyd - hboyd255@gmail.com
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
     * @return The sum of the provided angles.
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
    double toRadians();

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

/**
 * @brief Struct for storing a position, where x and y represent millimetres, by
 * default set to (0,0)
 *
 */
struct Position {
    /**
     * @brief The x position, measured in millimetres, set to 0 by default.
     */
    float x = 0;
    /**
     * @brief The y  position, measured in millimetres, set to 0 by default.
     */
    float y = 0;

    /**
     * @brief Converts the Position into a string, in the format (X,Y).
     *
     * @return (String) The Position as a string.
     */
    operator String() const;

    /**
     * @brief Overloaded compound addition operator, used to add a Position to
     * the current Position by summing their x and y components.
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
     * @brief The position of the Pose,(x and y measured in millimetres), set to
     * (0,0) by default.
     */
    Position position = {0, 0};
    /**
     * @brief The Angle of the Pose,(measured in degrees), set to 0 by default.
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