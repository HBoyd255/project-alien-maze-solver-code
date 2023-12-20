#ifndef SCHEDULE_H
#define SCHEDULE_H

/**
 * @file schedule.h
 * @brief Declaration of the Schedule class, the class responsible for keeping
 * track of when to execute routine functions.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#include <Arduino.h>

/**
 * @brief Schedule Class, designed to record the times at which a given function
 * was last called, and to call the function again if a specified mahout of time
 * has passed.
 *
 */
class Schedule {
   public:
    /**
     * @brief Construct a new Schedule object
     *
     * @param functionPtr A pointer to the function to execute.
     * @param period The number of milliseconds that should pass between the end
     * of the last execution, and the start of the next one.
     */
    Schedule(voidFuncPtr functionPtr, uint32_t period);

    /**
     * @brief The method that calls the functions and records the execution
     * time.
     *
     */
    void execute();

    /**
     * @brief The method that check if enough time has passed to call the
     * function.
     *
     */
    void poll();

   private:
    /**
     * @brief A pointer to the function to execute.
     *
     */
    voidFuncPtr _functionPtr;

    /**
     * @brief The number of milliseconds to wait before calling teh function
     * again
     *
     */
    uint32_t _period;

    /**
     * @brief The time at which the last function was called.
     *
     */
    uint32_t _lastExecutionTime = 0;
};

#endif  // SCHEDULE_H
