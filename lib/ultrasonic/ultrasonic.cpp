/**
 * @file ultrasonic.cpp
 * @brief Definition for the Ultrasonic class, responsible for reading data
 * from a given HCSR04 ultrasonic sensor.
 *
 * The data sheet for the HCSR04 can be found here,
 * https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-21
 * @copyright Copyright (c) 2023
 */

#include "ultrasonic.h"

// All values are in milliseconds, unless stated otherwise.

// The period(in microseconds) of the pulse sent to the trigger pin of the
// ultrasonic.
#define TRIGGER_PULSE_WIDTH_MICROSECONDS 10

// The period to wait between sending pulses to the trigger pin.
#define TRIGGER_POLL_PERIOD 100

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
 */
Ultrasonic::Ultrasonic(uint8_t triggerPin, uint8_t echoPin, uint32_t timeout,
                       uint16_t maxRange, uint32_t dataShelfLife)
    : _triggerPin(triggerPin),
      _echoPin(echoPin),
      _timeout(timeout),
      _maxRange(maxRange),
      _dataShelfLife(dataShelfLife) {}

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
 * @param isrPtr The pointer to the interrupt service routine.
 */
void Ultrasonic::setup(voidFuncPtr isrPtr) {
    pinMode(this->_triggerPin, OUTPUT);
    pinMode(this->_echoPin, INPUT);

    attachInterrupt(digitalPinToInterrupt(this->_echoPin), isrPtr, CHANGE);
}

/**
 * @brief Reads the distance from the sensor to the nearest obstacle by
 * waiting for a new value.
 *
 * @return (int16_t) The distance from the nearest obstacle in millimeters
 * @return (-1) If the obstacle is out of range.
 */
int16_t Ultrasonic::readNew() {
    this->_trigger();

    int16_t newWidth = _getNewPulse();

    return _pulseWidthToDistance(newWidth);
}

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
int16_t Ultrasonic::read() {
    // Get the age of the last reading in milliseconds.
    uint32_t dataAge = (micros() - this->_echoPinUpTimeMicros) / 1000;

    // Id the data is older than the required shelf life, take a new reading.
    if (dataAge > this->_dataShelfLife) {
        return readNew();
    }

    // Calculate and return the distance to an obstacle based on the width of
    // the most resent pulse to the echo pin.
    return _pulseWidthToDistance(this->_pulseWidthMicros);
}

/**
 * @brief The interrupt service routine that is called the echo pin changes
 * state.
 */
void Ultrasonic::isr() {
    // Record the times in microseconds at which the echo pin either rose or
    // fell.
    if (digitalRead(this->_echoPin) == HIGH) {
        this->_echoPinUpTimeMicros = micros();
    } else {
        this->_echoPinDownTimeMicros = micros();

        // When the echo pin falls. calculate the duration of the most recent
        // pulse.
        this->_pulseWidthMicros =
            this->_echoPinDownTimeMicros - this->_echoPinUpTimeMicros;
    }
}

/**
 * @brief Checks the scheduler to see if it is time to send a new trigger
 * pulse.
 */
void Ultrasonic::poll() {
    // Create static scheduler that will run wait for a given minimum duration
    // before running a function.
    static PassiveSchedule ultrasonicSchedule(TRIGGER_POLL_PERIOD);

    // If enough time has passed since the last time the function ran,
    if (ultrasonicSchedule.isReadyToRun()) {
        // Pulse the trigger pin.
        this->_trigger();
        // Record the time at which the trigger pin was pulsed.
        this->_triggerTimeMicros = millis();
    }
}

/**
 * @brief Waits for a pulse to the echo pin.
 *
 * @return (int16_t) The duration of the pulse to the echo pin.
 * @return (-1) If the function times out.
 */
int16_t Ultrasonic::_getNewPulse() {
    // Record the time in microseconds which this function started.
    uint32_t functionStartTime = micros();

    // Wait until the echo pin has both rose and fell.
    while (this->_echoPinUpTimeMicros < functionStartTime ||
           this->_echoPinDownTimeMicros < functionStartTime) {
        // If the function times out, return -1.
        if (micros() - functionStartTime > this->_timeout) {
            return -1;
        }
    }

    uint32_t pulseWidth =
        this->_echoPinDownTimeMicros - this->_echoPinUpTimeMicros;

    return pulseWidth;
}

/**
 * @brief Sends a pulse to the trigger pin.
 */
void Ultrasonic::_trigger() {
    digitalWrite(this->_triggerPin, LOW);
    delayMicroseconds(TRIGGER_PULSE_WIDTH_MICROSECONDS);
    digitalWrite(this->_triggerPin, HIGH);
    delayMicroseconds(TRIGGER_PULSE_WIDTH_MICROSECONDS);
    digitalWrite(this->_triggerPin, LOW);
}

/**
 * @brief Converts a given pulse duration into a distance
 *
 * @param pulseWidthMicros (int16_t) The duration in microseconds of the
 * given pulse.
 * @return (int16_t) The distance to an obstacle based on the pulse width.
 * @return (-1) If the distance is out of range.
 */
int16_t Ultrasonic::_pulseWidthToDistance(int16_t pulseWidthMicros) {
    if (pulseWidthMicros < 0) {
        return -1;
    }

    // The time it took for sound to travels from the sensor to the obstacle.
    uint16_t durationMicros = pulseWidthMicros / 2;

    // distance = speed * time

    // Speed of sound - 343 meters per second
    // speed = 0.343 meters per millisecond

    // distance = speed * time / 2

    // millimeters = (meters per millisecond) * microseconds

    uint16_t distance = (durationMicros * 0.343);

    // If the distance is out of range, return -1.
    return (distance < this->_maxRange) ? distance : -1;
}