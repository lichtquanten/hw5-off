/*
 * PROVIDED CODE - DO NOT EDIT
 */

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

DriveResult SimulatedRobot::advance()
{
  if (hasReachedEnd())
  {
    throw std::logic_error("Cannot drive a robot that has reached its path limit.");
  }

  int destination = position +
                    std::min(MAX_SEGMENTS_PER_DRIVE, reachableEnd - position);

  if (nextDebrisIndex < debrisSegments.size())
  {
    destination = std::min(destination, debrisSegments[nextDebrisIndex]);
  }

  if (shipwreckSegment && *shipwreckSegment > position)
  {
    destination = std::min(destination, *shipwreckSegment);
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
