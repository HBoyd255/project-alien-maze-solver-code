/**
 * @file schedule.h
 * @brief Declaration of the Schedule and PassiveSchedule classes, responsible
 * for keeping track of when to execute routine functions.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <Arduino.h>

/**
 * @brief Schedule class, designed to record the times at which a given function
 * was last called, and to call the function again if a specified mahout of time
 * has passed.
 *
 */
class Schedule {
   public:
    /**
     * @brief Construct a new Schedule object
     *
     */
    Schedule(){};

    /**
     * @brief Sets up the object by assigning a function to call periodically,
     * and the period that should be waited between function calls.
     *
     * @param periodicFunctionPtr A pointer to the function to execute.
     * @param period The number of milliseconds that should pass between the end
     * of the last execution, and the start of the next one.
     */
    void setup(voidFuncPtr periodicFunctionPtr, uint32_t period);

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
    voidFuncPtr _periodicFunctionPtr;

    /**
     * @brief The number of milliseconds to wait before calling the function
     * again
     */
    uint32_t _period;

    /**
     * @brief The time at which the last function was called.
     *
     */
    uint32_t _lastExecutionTime = 0;
};

/**
 * @brief PassiveSchedule class, Used to keep track of the last time that an
 * execution took place.
 */
class PassiveSchedule {
   public:
    /**
     * @brief Construct a new Passive Schedule object
     *
     * @param period (uint32_t) The number of milliseconds to wait before
     * calling allowing the function to run again.
     */
    PassiveSchedule(uint32_t period);

    /**
     * @brief Check if the enough time has passed since the last time the a
     * function ran. if enough time has passed, set the timer to one period in
     * the future and return true.
     *
     * @return (true) If enough time has passed.
     * @return (false) If enough time has not passed.
     */
    bool isReadyToRun();

   private:
    /**
     * @brief The number of milliseconds to wait before calling allowing the
     * function to run again.
     */
    uint32_t _period;

    /**
     * @brief The time at which the function will be allowed to run again.
     */
    uint32_t _nextExecutionTime;
};

#endif  // SCHEDULE_H