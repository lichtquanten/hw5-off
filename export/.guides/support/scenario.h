/*
 * PROVIDED CODE - DO NOT EDIT
 */

#pragma once

#include <array>
#include <optional>
#include <vector>

#include "mothership.h"

struct Scenario
{
  std::array<int, ROBOT_COUNT> abysses = []
  {
    std::array<int, ROBOT_COUNT> positions;
    positions.fill(TETHER_LENGTH + 1);
    return positions;
  }();
  std::vector<Location> debris;
  std::optional<Location> shipwreck;
};

void validateRobotIndex(int robotIndex);
void validateScenario(const Scenario &scenario);
