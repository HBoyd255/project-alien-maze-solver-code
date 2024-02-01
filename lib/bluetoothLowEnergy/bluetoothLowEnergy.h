/**
 * @file bluetoothLowEnergy.h
 * @brief A wrapper for the ArduinoBLE.h module used to simplify the process of
 * transmitting data using BLE.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-01-24
 * @copyright Copyright (c) 2024
 */

#ifndef BLUETOOTH_LOW_ENERGY_H
#define BLUETOOTH_LOW_ENERGY_H

// https://www.arduino.cc/reference/en/
#include <Arduino.h>

// https://www.arduino.cc/reference/en/libraries/arduinoble/
#include <ArduinoBLE.h>

// Forward declaration for the Pose, Brick and BrickList
// classes.
class Pose;
class Brick;
class BrickList;

/**
 * @brief A wrapper for the ArduinoBLE class "BLE" tailored to the specific
 * needs of this project.
 */
class BluetoothLowEnergy {
   public:
    /**
     * @brief Construct a new BluetoothLowEnergy object
     *
     * @param mainServiceUUID The UUID to attach the main service to.
     * @param robotPoseUUID  The UUID to attach the robotPose characteristic to.
     * @param brickUUID The UUID to attach the brick characteristic to.
     */
    BluetoothLowEnergy(const char* mainServiceUUID, const char* robotPoseUUID,
                       const char* brickUUID);

    /**
     * @brief Sets up the BLE events.
     *
     * @param deviceName The name of the robot to broadcast.
     * @param macAddress The mac address stored in the source code, to compare
     * to the one read from the device.
     */
    void setup(const char* deviceName, const char* macAddress);

    /**
     * @brief Transmits the current pose of the robot over BLE.
     *
     * @param robotPose The current pose of the robot.
     */
    void sendRobotPose(Pose robotPose);

    /**
     * @brief Transmits a list of bricks over BLE, brick by brick.
     *
     * @param brickListToSend The list of bricks to send.
     */
    void sendBrickList(BrickList brickListToSend);

    /**
     * @brief Polls the ble connection.
     */
    void poll();

    /**
     * @brief Checks if a central device is currently connected.
     *
     * @return (true) If a central device is currently connected.
     * @return (false) If a central device is not currently connected.
     */
    bool isConnected();

   private:
    /**
     * @brief The service to advertise as the main service.
     */
    BLEService _mainService;

    /**
     * @brief The Characteristic responsible for transmitting robot pose data.
     */
    BLECharacteristic _robotPoseCharacteristic;

    /**
     * @brief The Characteristic responsible for transmitting brick data.
     */
    BLECharacteristic _brickCharacteristic;

    /**
     * @brief Transmits data about a given brick over BLE.
     *
     * @param brickToSend The brick to send over BLE.
     * @param brickNumber The index of the current brick in the blacklist.
     */
    void _sendBrick(Brick brickToSend, int brickNumber);
};

#endif  // BLUETOOTH_LOW_ENERGY_H
