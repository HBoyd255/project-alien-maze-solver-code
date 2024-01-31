/**
 * @file ultrasonic.h
 * @brief Declaration for the Ultrasonic class, responsible for reading data
 * from a given HCSR04 ultrasonic sensor.
 *
 * The data sheet for the HCSR04 can be found here,
 * https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf.
 *
 * @author Harry Boyd - github.com/HBoyd255
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

#include "schedule.h"

// All values are in milliseconds, unless stated otherwise.

/**
 * @brief Ultrasonic class, responsible for reading data from a given HCSR04
 * ultrasonic sensor.
 */
class Ultrasonic {
   public:
    /**
     * @brief Construct a new Ultrasonic object
     *
     * @param triggerPin (uint8_t) The pin connected to the trigger pin of the
     * HCSR04.
     * @param echoPin (uint8_t) The pin connected to the echo pin of the
     * HCSR04.
     * @param timeout (uint32_t) The number of microseconds that it takes for
     * the class gives up on taking a reading and returns -1.
     * @param maxRange (uint16_t) The maximum number of milometers that the
     * sensor can measure, before -1 is returned.
     * @param dataShelfLife (uint32_t) The maximum age that data can be before a
     * new reading must be taken.
     * @param distanceFromCentre (int) The distance between the sensor and the
     * centre of the robot.
     */
    Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout,
               uint16_t maxRange, uint32_t dataShelfLife,
               int distanceFromCentre);

    /**
     * @brief Sets up the class by attaching the interrupt pin to the provided
     * irs function.
     *
     * To use this class properly the provided isr pointer should be just call
     * the isr() method of the same class via a lamda function. This is just to
     * get around an issue with arduino's attachInterrupt.
     *
     * This should look like OBJECT_NAME.setup([]() { OBJECT_NAME.isr();});
     *
     * This isn't the cleanest workaround but works for the intended purpose.
     *
     * @param isr_P The pointer to the interrupt service routine.
     */
    void setup(voidFuncPtr isr_P);

    /**
     * @brief Reads the distance from the sensor to the nearest obstacle by
     * waiting for a new value.
     *
     * @return (int16_t) The distance from the nearest obstacle in millimeters
     * @return (-1) If the obstacle is out of range.
     */
    int16_t readNew();

    /**
     * @brief Reads the distance from the sensor to the nearest obstacle by
     * reading from the most recent value of pulse width.
     *
     * if the most recent value of pulse width is out of data, a call to
     * readNew() is made.
     *
     * read() is about 20 times faster than readNew() because it is non blocking
     * and relies on interrupts.
     *
     * @return (int16_t) The distance from the nearest obstacle in millimeters
     * @return (-1) If the obstacle is out of range.
     */
    int16_t read();

    /**
     * @brief Reads the distance read by the sensor plus the distance from the
     * sensor to the robots centre.
     *
     * @return (int) The total distance from the centre of the robot to the
     * obstacle seen by the ultrasonic sensor.
     * @return (-1) If the obstacle is out of range.
     */
    int readFromRobotCenter();

    /**
     * @brief The interrupt service routine that is called the echo pin
     * changes state.
     */
    void isr();

    /**
     * @brief Checks the scheduler to see if it is time to send a new
     * trigger pulse.
     */
    void poll();

   private:
    /**
     * @brief The pin connected to the trigger pin of the HCSR04.
     */
    uint8_t _triggerPin;
    /**
     * @brief The pin connected to the echo pin of the HCSR04.
     */
    uint8_t _echoPin;
    /**
     * @brief The number of microseconds that it takes for the class gives
     * up on taking a reading.
     */
    uint32_t _timeout;
    /**
     * @brief The maximum number of milometers that the sensor can measure.
     */
    uint16_t _maxRange;

    /**
     * @brief The maximum age that data can be before a new reading must be
     * taken.
     */
    uint32_t _dataShelfLife;

    /**
     * @brief The distance between the sensor and the centre of the robot.
     */
    int _distanceFromCentre;

    /**
     * @brief The time in microseconds at which the trigger pin was last
     * pulsed.
     */
    uint32_t _triggerTimeMicros = 0;
    /**
     * @brief The time in microseconds at which the echo pin last rose.
     */
    uint32_t _echoPinUpTimeMicros = 0;
    /**
     * @brief The time in microseconds at which the echo pin last fell.
     */
    uint32_t _echoPinDownTimeMicros = 0;
    /**
     * @brief The duration in microseconds of the last pulse to the echo
     * pin.
     */
    int16_t _pulseWidthMicros = 0;

    /**
     * @brief An instance of the PassiveScheduler class, used for ensuring that
     * the poll function only runs at designated time intervals.
     */
    PassiveSchedule _pollSchedule;

    /**
     * @brief Waits for a pulse to the echo pin.
     *
     * @return (int16_t) The duration of the pulse to the echo pin.
     * @return (-1) If the function times out.
     */
    int16_t _getNewPulse();

    /**
     * @brief Sends a pulse to the trigger pin.
     */
    void _trigger();

    /**
     * @brief Converts a given pulse duration into a distance
     *
     * @param pulseWidthMicros (int16_t) The duration in microseconds of the
     * given pulse.
     * @return (int16_t) The distance to an obstacle based on the pulse
     * width.
     * @return (-1) If the distance is out of range.
     */
    int16_t _pulseWidthToDistance(int16_t pulseWidthMicros);
};

#endif  // ULTRASONIC_H