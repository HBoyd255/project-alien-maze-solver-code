#ifndef COMPARISON_H
#define COMPARISON_H

#include <Arduino.h>

/**
 * @brief Checks wether the absolute difference between two given values is
 * within a given range(inclusive).
 *
 * @param valueA The first integer to compare.
 * @param valueB The second integer to compare
 * @param range The inclusive range that the valses should be in of each other.
 * @return (true) If abs(valueA - valueB) <= range.
 * @return (false) If abs(valueA - valueB) > range.
 */
bool within(int valueA, int valueB, int range);

#endif  // COMPARISON_H