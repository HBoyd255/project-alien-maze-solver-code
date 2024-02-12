/**
 * @file schedule.cpp
 * @brief Definitions of the Schedule  and PassiveSchedule classes, the class
 * responsible for keeping track of when to execute routine functions.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#include "schedule.h"

/**
 * @brief Construct a new Passive Schedule object
 *
 * @param period (uint32_t) The number of milliseconds to wait before
 * calling allowing the function to run again.
 */
PassiveSchedule::PassiveSchedule(uint32_t period)
    : _period(period), _nextExecutionTime(millis() + period) {}

/**
 * @brief Check if the enough time has passed since the last time the a
 * function ran. if enough time has passed, set the timer to one period in
 * the future and return true.
 *
 * @return (true) If enough time has passed.
 * @return (false) If enough time has not passed.
 */
bool PassiveSchedule::isReadyToRun() {
    if (millis() > this->_nextExecutionTime) {
        this->_nextExecutionTime = millis() + this->_period;
        return true;
    }
    return false;
}