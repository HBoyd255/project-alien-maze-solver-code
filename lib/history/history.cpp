/**
 * @file history.cpp
 * @brief Definitions of the History class, a class responsible for storing a
 * queue of uint16_t values, and calculating their median.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */
#include "history.h"

#include <algorithm>

/**
 * @brief Construct a new History object.
 *
 * @param maxValues (uint16_t) The maximum amount of int16_ts that can be in
 * the queue before items start getting removed from the front.
 */
History::History(uint16_t maxValues) : _maxValues(maxValues) {}

/**
 * @brief Implicitly converts the stored values into a string.
 *
 * @return (String) The values stores in _values as a comma separated list.
 */
History::operator String() const {
    String stringToReturn = "(";

    for (const auto& element : this->_values) {
        stringToReturn += element;
        stringToReturn += ",";
    }

    if (stringToReturn.length() > 1) {
        stringToReturn.remove(stringToReturn.length() - 1);
    }

    stringToReturn += ")";

    return stringToReturn;
}

/**
 * @brief Adds a new value to the queue.
 *
 * @param newValue (int32_t) the value to add.
 */
void History::add(int32_t newValue) {
    if (this->_values.size() == this->_maxValues) {
        this->_values.erase(this->_values.begin());
    }
    this->_values.push_back(newValue);
}

    /**
     * @brief Gets the median value from the queue.
     *
     * @return (int16_t) The median of the values in the queue.
     */
int16_t History::getMedian() {
    uint16_t medianIndex = this->_values.size() >> 1;

    std::vector<int16_t> valuesCopy = this->_values;

    std::sort(valuesCopy.begin(), valuesCopy.end());

    return valuesCopy[medianIndex];
}

/**
 * @brief Get the last value added to the queue.
 *
 * @return (int16_t) The last value added to the queue.
 */
int16_t History::getLast() { return this->_values.back(); }

/**
 * @brief Counts the number of times that the value -1 exists in the queue.
 *
 * @return (uint16_t) The number of times that the value -1 exists in the
 * queue.
 */
uint16_t History::countErrors() {
    uint16_t occurrences = 0;

    // Iterate through each item in the queue, if the item item is equal to -1,
    // increment the counter
    for (const auto& item : this->_values) {
        if (item == -1) {
            occurrences++;
        }
    }
    return occurrences;
}
