#pragma once

#include <array>

#include "mothership.h"

inline std::array<int, 2> neighborsOf(int robot)
{
    return {(robot + ROBOT_COUNT - 1) % ROBOT_COUNT, (robot + 1) % ROBOT_COUNT};
}

inline std::array<int, 3> robotAndNeighbors(int robot)
{
    const auto neighbors = neighborsOf(robot);
    return {neighbors[0], robot, neighbors[1]};
}
