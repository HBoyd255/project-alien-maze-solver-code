#ifndef HISTORY_H
#define HISTORY_H
/**
 * @file history.h
 * @brief Declaration of the History class, a class responsible for storing a
 * queue of values, and calculating their median.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#include <Arduino.h>

#include <vector>

/**
 * @brief History Class, Used for storing a queue of values repressing the
 * history of signed 16 bit integer.
 *
 *
 * This class acts as a wrapper for the vector, with the ability to get the
 * median and automatically pop the last values from the queue.
 *
 */
class History {
   public:
    /**
     * @brief Construct a new History object.
     *
     * @param maxValues (uint16_t) The maximum amount of int16_ts that can be in
     * the queue before items start getting removed from the front.
     */
    History(uint16_t maxValues);

    /**
     * @brief Implicitly converts the stored values into a string.
     *
     * @return (String) The values stores in _values as a comma separated list.
     */
    operator String() const;

    /**
     * @brief Adds a new value to the queue.
     *
     * @param newValue (int32_t) the value to add.
     */
    void add(int32_t newValue);

    /**
     * @brief Get the Median object
     *
     * @return (int16_t) The median of the values in the queue.
     */
    int16_t getMedian();

    /**
     * @brief Counts the number of times that the value -1 exists in the queue.
     *
     * @return (uint16_t) The number of times that the value -1 exists in the
     * queue.
     */
    uint16_t countErrors();

   private:
    /**
     * @brief The vector that stores the queue of int16_ts.
     */
    std::vector<int16_t> _values;

    /**
     * @brief The maximum amount of int16_ts that can be in the queue.
     *
     */
    uint16_t _maxValues;
};

#endif  // HISTORY_H
