/**
 * @file brick.h
 * @brief Declarations for the Brick struct and the BrickList Class.
 *
 * @author Harry Boyd - github.com/HBoyd255
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
     * param zone_IP
     * @param dx_P A pointer to return the difference in x positions.
     * @param dy_P A pointer to return the difference in y positions.
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
 * handling a list of bricks.
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
     * @brief populates the brick list with a set of hard coded bricks that
     * represent the data in the provided "Seen Maze".
     *
     * This function should only be used for algorithm testing,
     */
    void setPreprogrammedMazeData();

    /**
     * @brief checks if a seen brick corner falls withing the outer bounds of
     * the maze, and if so places a brick in the brick list, flat against the
     * wall.
     *
     * @param brickCorner The position of the corner of the brick that was seen
     * by a sensor.
     * @return (true) If a brick has been added to the list.
     * @return (false) If a brick has been not been added to the list.
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

    bool handleBrickFromSensorAndMap(Position robotPosition,
                                     Angle angleOfSensor, int measuredDistance,
                                     int tolerance, Map* map_P);

    int lowestDistance(Position target, int* indexOfClosestBrickPrt = nullptr,
                       int* zoneFromClosestBrickPrt = nullptr);

   private:
    Brick _brickArray[MAX_BRICK_COUNT];
    int _brickCount = 0;

    int _compare(Position robotPose, Angle angleOfSensor, int measuredDistance);
    void _setBrickFromEdge(Position brickEdgePosition, Angle angleOfSensor,
                           int tolerance);

    int _getOrthogonalBrickDistance(Position robotPosition, Angle angleOfBrick);

    void _appendBrick(Brick brickToAdd);
    bool _attemptAppendBrick(Brick brickToAdd);

    void _addWalls();
};

#endif  // BRICK_H
