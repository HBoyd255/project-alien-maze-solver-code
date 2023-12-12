#ifndef BINARY_H
#define BINARY_H

/**
 * @file binary.h
 * @brief Declares the functions responsible for formatting binary data into a
 * more readable format.
 *
 * @author Harry Boyd - hboyd255@gmail.com
 * @date 2023-12-08
 * @copyright Copyright (c) 2023
 */

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

/**
 * @brief Prints a byte of data as its individual bits.
 *
 * This function takes a 8 bits of data (a byte) and prints it to the serial
 * port as its individual bits, starting with the most significant bit on the
 * left. Any leading zeros of the data are also printed. A separator can also be
 * provided to separate the bits, but is not required.
 *
 * for example,printByte(13) would print 00001101. and printByte(13,",") would
 * print 0,0,0,0,1,1,0,1.
 *
 * This function assumes that a serial connection has been initialised.
 *
 * @param byteOfData The unsigned 8 bits of data to print to the serial port.
 * @param separator (Optional) The string to be printed between each bit, by
 * default this is an empty string.
 */
void printByte(uint8_t byteOfData, String separator = "");

#endif  // BINARY_H