/**
 * @file brick.h
 * @brief Declarations for the Brick struct and the BrickList Class.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2024-01-24
 * @copyright Copyright (c) 2024
 */

#ifndef BRICK_H
#define BRICK_H

#include "angleAndPosition.h"

// There should be 19 bricks (15 bricks + 4 walls) in the maze, but its better
// to have too many bricks than not enough, so enough memory is allocated to
// store 50 bricks.
#define MAX_BRICK_COUNT 50

/**
 * @brief Enumeration representing various zones, relative to a brick.
 */
enum Zone {
    CentreZone,
    RightZone,
    TopRightZone,
    TopZone,
    TopLeftZone,
    LeftZone,
    BottomLeftZone,
    BottomZone,
    BottomRightZone,
};

// Forward declaration of the Map class.
class Map;

/**
 * @brief The Brick struct, Storing the location of a brick, its orientation, as
 * well as its width and height.
 */
struct Brick {
    /**
     * @brief The position of the brick
     */
    Position position;

    /**
     * @brief The rotation of the brick, the brick can only be positioned in 90
     * degree increments.
     *
     */
    bool isVertical;

    /**
     * @brief The length of the brick, Set to a Macro defined default value when
     * constructed.
     */
    int length;

    /**
     * @brief The width of the brick, Set to a Macro defined default value when
     * constructed.
     */
    int width;

    /**
     * @brief Construct a new Brick object
     *
     * @param brickPosition The position of the brick.
     * @param isVertical The rotation of the brick, horizontal by default.
     */
    Brick(Position brickPosition = Position(0, 0), bool isVertical = 0);

    /**
     * @brief Get the position of the bricks bottom left corner.
     *
     * @return (Position) The bricks bottom left corner.
     */
    Position getBottomLeft();

    /**
     * @brief Get the position of the bricks top right corner.
     *
     * @return (Position) The bricks top right corner.
     */
    Position getTopRight();

    /**
     * @brief Calculates the zone that a given position is in, relative to the
     * current brick.
     *
     * @param target the position to test.
     * @return (Zone) The zone of the position, relative to the current brick.
     */
    Zone calculateZone(Position target);

    /**
     * @brief Calculates the square of the euclidean distance between the edge
     * of this brick and a provided target position.
     *
     * This function uses less computation than distanceTo(), as no
     * square root required. For basic comparisons between two distances,
     * this function is more efficient.
     *
     * c * c == a * a + b * b
     * is a lot easier to compute than
     * c = (a * a + b * b) ^ 0.5
     *
     * @param target The other position to calculate the squared distance
     * @param zone_IP The pointer to return the zone from the brick.
     * @param dx_P The pointer to return the difference in x positions.
     * @param dy_P The pointer to return the difference in y positions.
     * @return (float) The square of the euclidean distance to the target
     * position from the edge of this brick, in millimeters.
     */
    float squaredDistanceTo(Position target, int* zone_IP = nullptr,
                            int* dx_P = nullptr, int* dy_P = nullptr);

    /**
     * @brief Calculates the euclidean distance between the edge of this brick
     * and a provided target position using the pythagorean theorem.
     *
     * @param target The other position to calculate the distance to.
     * @return (float) The euclidean distance to the target position in
     * millimeters.
     */
    float distanceTo(Position target);

    /**
     * @brief Converts the current brick into a human readable string.
     *
     * @return (String) The current brick as a human readable string.
     */
    String toString();
};

/**
 * @brief The BrickList class, responsible for storing, manipulating and
 * handling a collection of bricks.
 *

 *
 */
class BrickList {
   public:
    /**
     * @brief Construct a new BrickList object
     */
    BrickList();

    /**
     * @brief Get the number of bricks in the list.
     *
     * @return (int) The number of bricks in the list.
     */
    int getBrickCount();

    /**
     * @brief Get the Brick at a given index in the list.
     *
     * @param index The index of the brick to return.
     * @return (Brick) The brick at the given index in the list.
     */
    Brick getBrick(int index);

    /**
     * @brief Converts the BrickList into a human readable string.
     *
     * @return (String) The BrickList as a human readable string.
     */
    String toString();

    /**
     * @brief Takes the position of a corner seen by the robot, and decided if
     * it is along one of the walls of the maze.
     *
     * @param brickCorner The corner position seen by the robot,
     * @return (true) If a brick is added to the list.
     * @return (false) If a brick is not added to the list.
     */
    bool handleBrickFromWallPosition(Position brickCorner);

    /**
     * @brief Takes a line, provided as a start and end position, and calculates
     * if a brick can be approximated from it.
     *
     * @param robotPosition The current position of the robot.
     * @param statingCorner The position that the seen line starts.
     * @param lineEnd  The position that the seen line ends.
     * @return (true) If a brick has been added to the list.
     * @return (false) If a brick has been not been added to the list.
     */
    bool handleBrickFromLine(Position robotPosition, Position lineStart,
                             Position lineEnd);

    /**
     * @brief Takes a sensor reading from a position, and compared it to the
     * curent map data and brick list, and if applicable adds a new brick to
     * the list.
     *
     * @param robotPosition The current position on the robot.
     * @param angleOfSensor The direction that the sensor that captured the data
     * is pointing.
     * @param measuredDistance The distance read by the sensor.
     * @param tolerance The maximum offset that the sensor can be from an
     * orthogonal direction, before readings start getting rejected;
     * @param map_P A pointer to the map data.
     * @return (-1) If the robot's position needs recalibrating.
     * @return (0) If no changes have been made.
     * @return (1) If a brick has been added to the list.
     */
    int handleBrickFromSensorAndMap(Position robotPosition, Angle angleOfSensor,
                                    int measuredDistance, int tolerance,
                                    Map* map_P);

    /**
     * @brief Returns the distance to the closest brick in the list, from a
     * given target position.
     *
     * Also returns via reference, the index of this closest brick, and the zone
     * from this closest brick.
     *
     * @param target The position to calculate the distance to,
     * @param indexOfClosestBrick_P The pointer used to return the index of the
     * closest brick.
     * @param zoneFromClosestBrick_P The pointer used to return the zone from
     * the closest brick.
     * @return (int) The distance to the closest brick.
     */
    int lowestDistance(Position target, int* indexOfClosestBrick_P = nullptr,
                       int* zoneFromClosestBrick_P = nullptr);

#if 0  // Blocked out until it is needed again for testing.
    /**
     * @brief populates the brick list with a set of hard coded bricks that
     * represent the data in the provided "Seen Maze".
     *
     * This function should only be used for algorithm testing,
     */
    void setPreprogrammedMazeData();
#endif

   private:
    /**
     * @brief The array that holds the bricks in the list.
     *
     * Due to the current simplicity of the BrickList storing the data as an
     * array works well. If, later in development, it becomes necessary to
     * remove bricks from the list or preform other dynamic operation, the data
     * could be refactored into a list.
     *
     * For the sake of consistency, this class as a whole will be referred to
     * as the BrickList, regardless of the way that it is internally storing
     * the bricks.
     */
    Brick _brickArray[MAX_BRICK_COUNT];

    /**
     * @brief The number of bricks in the list.

     */
    int _brickCount = 0;

    /**
     * @brief Compares a sensors reading the expected reading, obtained from the
     * existing items in the brick list.
     *
     * @param robotPose The current pose of the robot.
     * @param angleOfSensor The direction that the sensor that captured the data
     * is pointing.
     * @param measuredDistance The distance read by the sensor.
     * @return (1) If a brick need placing.
     * @return (0) If no changes need making.
     * @return (-1) If the robot's position need recalibrating.
     */
    int _compare(Position robotPose, Angle angleOfSensor, int measuredDistance);

    /**
     * @brief Creates a brick based on the position of its edge, and the angle
     * of the sensor that saw it.
     *
     * The brick will be places so that the position seen (brickEdgePosition)
     * will be on the centre of its long side, and will extrude away from the
     * sensor.
     *
     * @param brickEdgePosition The position seen by the sensor.
     * @param angleOfSensor The angle that the sensor was facing when the brick
     * was seen.
     * @return (Brick) The newly created brick.
     */
    Brick _getBrickFromEdge(Position brickEdgePosition, Angle angleOfSensor);

    /**
     * @brief Get the distance to the closest brick in a chosen direction,
     * Only works with orthogonal directions.
     *
     * @param robotPosition The position of the robot.
     * @param directionOfBrick The direction to look in, will be rounded to the
     * nearest right angle.
     * @return (int) The distance to the brick in the chosen direction.
     * Returns -1 if no brick was seen.
     */
    int _getOrthogonalBrickDistance(Position robotPosition,
                                    Angle directionOfBrick);

    /**
     * @brief Add a brick to the end of the brick list, if there is space
     * remaining.
     *
     * @param brickToAdd The brick to add.
     * @return (true) If the brick was added to the list.
     * @return (false) If the list was already full.
     */
    bool _appendBrick(Brick brickToAdd);

    /**
     * @brief Adds a brick the the end of the brick list, if it does not collide
     * with an existing brick in the list.
     *
     * @param brickToAdd  The brick to add.
     * @return (true) If the brick was added to the list.
     * @return (false) If the brick could not be added.
     */
    bool _attemptAppendBrick(Brick brickToAdd);

    /**
     * @brief Append 4 brick to the list, representing the 4 boundary walls.
     */
    void _addWalls();
};

#endif  // BRICK_H
