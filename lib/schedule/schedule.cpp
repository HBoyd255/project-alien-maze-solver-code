/**
 * @file schedule.cpp
 * @brief Definitions of the Schedule class, the class responsible for keeping
 * track of when to execute routine functions.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#include "schedule.h"

/**
 * @brief Construct a new Schedule object
 *
 * @param functionPtr A pointer to the function to execute.
 * @param period The number of milliseconds that should pass between the end
 * of the last execution, and the start of the next one.
 */
Schedule::Schedule(voidFuncPtr functionPtr, uint32_t period)
    : _functionPtr(functionPtr), _period(period) {}

/**
 * @brief The method that calls the functions and records the execution
 * time.
 *
 */
void Schedule::execute() {
    this->_functionPtr();
    this->_lastExecutionTime = millis();
}

/**
 * @brief The method that check if enough time has passed to call the
 * function.
 *
 */
void Schedule::poll() {
    uint32_t _nextExecutionTime = _lastExecutionTime + _period;

    if (millis() > _nextExecutionTime) {
        this->execute();
    }

    // The reasons there is nothing to handle millis overflowing if because the
    // max value of millis is equivelent to 49 days, which is far greater than
    // needed.
}
