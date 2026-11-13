#pragma once

#include <optional>
#include <vector>

#include "mothership.h"

constexpr int DEFAULT_ROBOT_COUNT = 100;

struct Scenario
{
  std::vector<int> abysses = std::vector<int>(DEFAULT_ROBOT_COUNT, TETHER_LENGTH + 1);
  std::vector<Location> debris;
  std::optional<Location> shipwreck;
};

void validateRobotIndex(int robotIndex, int robotCount);
