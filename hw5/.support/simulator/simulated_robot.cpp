#include "simulated_robot.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

SimulatedRobot::SimulatedRobot(int reachableEnd, std::vector<int> debrisSegments,
                               std::optional<int> shipwreckSegment)
    : reachableEnd(reachableEnd), debrisSegments(std::move(debrisSegments)),
      shipwreckSegment(shipwreckSegment)
{
  std::sort(this->debrisSegments.begin(), this->debrisSegments.end());
}

DriveResult SimulatedRobot::advance(int maxSegments)
{
  if (maxSegments < 1 || maxSegments > MAX_SEGMENTS_PER_DRIVE)
  {
    throw std::invalid_argument("Drive distance must be between 1 and 100 segments.");
  }

  int destination = position + maxSegments;

  if (nextDebrisIndex < debrisSegments.size())
  {
    destination = std::min(destination, debrisSegments[nextDebrisIndex]);
  }

  if (shipwreckSegment && *shipwreckSegment > position)
  {
    destination = std::min(destination, *shipwreckSegment);
  }

  if (destination > reachableEnd)
  {
    throw std::runtime_error(reachableEnd < TETHER_LENGTH
                                 ? "Robot drove into an abyss."
                                 : "Robot drove beyond its tether length.");
  }

  const int moved = destination - position;
  position = destination;

  if (shipwreckSegment && *shipwreckSegment == position)
  {
    return {moved, DriveStopReason::ShipwreckFound};
  }

  if (nextDebrisIndex < debrisSegments.size() &&
      debrisSegments[nextDebrisIndex] == position)
  {
    ++nextDebrisIndex;
    return {moved, DriveStopReason::DebrisFound};
  }

  if (hasReachedEnd())
  {
    return {moved, DriveStopReason::PathLimitReached};
  }

  return {moved, DriveStopReason::DriveLimitReached};
}

int SimulatedRobot::getPosition() const
{
  return position;
}

bool SimulatedRobot::hasReachedEnd() const
{
  return position == reachableEnd;
}
