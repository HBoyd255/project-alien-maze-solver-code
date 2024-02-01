/**
 * @file schedule.h
 * @brief Declaration of the Schedule and PassiveSchedule classes, responsible
 * for keeping track of when to execute routine functions.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-20
 * @copyright Copyright (c) 2023
 */

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <Arduino.h>



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