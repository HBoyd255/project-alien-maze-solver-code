/**
 * @file binary.cpp
 * @brief Defines the functions responsible for formatting binary data into a
 * more readable format.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2023-12-08
 * @copyright Copyright (c) 2023
 */
#include "binary.h"

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
 * @param byteToPrint The unsigned 8 bits of data to print to the serial port.
 * @param separator (Optional) The string to be printed between each bit, by
 * default this is an empty string.
 */
void printByte(uint8_t byteToPrint, String separator) {
    // Print the 7 leading bits of the byte, followed by the optional separator.
    for (uint8_t b = 7; b != 0; b--) {
        Serial.print((byteToPrint >> b) & 1);
        Serial.print(separator);
    }
    // Print the final bit, followed by a newline character.
    Serial.println(byteToPrint & 1);
}

/**
 * @brief Prints a 4 bytes of data as its individual bits.
 *
 * Works the same as printByte, but with a long.
 *
 * This function assumes that a serial connection has been initialised.
 *
 * @param longToPrint The unsigned 32 bits of data to print to the serial port.
 * @param separator (Optional) The string to be printed between each bit, by
 * default this is an empty string.
 */
void printLong(uint32_t longToPrint, String separator) {
    for (uint8_t b = 31; b != 0; b--) {
        Serial.print((longToPrint >> b) & 1);
        Serial.print(separator);
    }
    // Print the final bit, followed by a newline character.
    Serial.println(longToPrint & 1);
}

/**
 * @brief Rotates a byte by shifting it left and adding the lost bits onto the
 * right side.
 *
 * @param byteToRotate The byte to rotate.
 * @param rotationAmount The number of bits to get shifted by.
 * @return (uint8_t) The rotated byte.
 */
uint8_t rotateLeft(uint8_t byteToRotate, uint8_t rotationAmount) {
    uint8_t topPart = byteToRotate << rotationAmount;
    uint8_t bottomPart = byteToRotate >> (8 - rotationAmount);
    uint8_t rotatedByte = topPart | bottomPart;
    return rotatedByte;
}