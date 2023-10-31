#ifndef BLE_CONTROL_H
#define BLE_CONTROL_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

#include "motorControl.h"


/**
 * Initializes the BLE (Bluetooth Low Energy) module for communication with a
 * central device (e.g. a phone or laptop).
 *
 * This function should be called in the setup() function of the main program.
 *
 * Initialises the BLE module,
 * prints the mac address to the serial port,
 * sets the device name,
 * adds the characteristic to the sensor service,
 * add the sensor service to the ble service set,
 * adds the sensor service to the list of advertised services,
 * and starts advertising the service.
 */
void setupBle();



/**
 * takes a byte representing the 8 bumper values and sends it over BLE.
 *
 * @param value The value of the bumper sensors as a byte, where each bit
 * represents the boolean state of each bumper.
 */
void bleUpdateBumper(byte value);

//TODO write documentation
//This is just a test function anyway  
void bleUpdateMillis(unsigned long value);


//TODO write documentation
signed char getMotorOverrideValues(Motors motor);

//TODO improve and write documentation
unsigned int getLed();


/**
 * @brief Polls the BLE connection for incoming data.
 *
 * This function checks if there is any incoming data on the BLE connection and
 * processes it accordingly.
 * It should be called periodically to ensure that incoming data is not missed.
 */
void blePoll();



#endif  // BLE_CONTROL_H
