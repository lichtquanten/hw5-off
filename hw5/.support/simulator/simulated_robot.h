#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "mothership.h"

class SimulatedRobot
{
public:
  SimulatedRobot(int reachableEnd, std::vector<int> debrisSegments,
                 std::optional<int> shipwreckSegment);

  [[nodiscard]] DriveResult advance(int maxSegments);
  int getPosition() const;
  bool hasReachedEnd() const;

private:
  int position = 0;
  int reachableEnd;
  std::vector<int> debrisSegments;
  std::size_t nextDebrisIndex = 0;
  std::optional<int> shipwreckSegment;
};
