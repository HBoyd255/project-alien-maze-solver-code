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
 * @brief Sets up the object by assigning a function to call periodically,
 * and the period that should be waited between function calls.
 *
 * @param periodicFunctionPtr A pointer to the function to execute.
 * @param period The number of milliseconds that should pass between the end
 * of the last execution, and the start of the next one.
 */
void Schedule::setup(voidFuncPtr functionPtr, uint32_t period) {
    this->_periodicFunctionPtr = functionPtr;
    this->_period = period;
}

/**
 * @brief The method that calls the functions and records the execution
 * time.
 *
 */
void Schedule::execute() {
    this->_periodicFunctionPtr();
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
