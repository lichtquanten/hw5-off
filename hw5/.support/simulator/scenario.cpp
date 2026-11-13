#include "scenario.h"

#include <algorithm>
#include <stdexcept>

void validateRobotIndex(int robotIndex, int robotCount)
{
  if (robotIndex < 0 || robotIndex >= robotCount)
  {
    throw std::out_of_range("Robot index is outside the valid range.");
  }
}

namespace
{
  void validateLocation(Location location, int robotCount)
  {
    validateRobotIndex(location.robot, robotCount);

    if (location.segment < 1 || location.segment > TETHER_LENGTH)
    {
      throw std::out_of_range("Segment number is outside the valid range.");
    }
  }
}

void validateScenario(const Scenario &scenario)
{
  if (scenario.abysses.empty())
  {
    throw std::invalid_argument("A scenario needs at least one robot.");
  }
  const int robotCount = static_cast<int>(scenario.abysses.size());
  for (int segment : scenario.abysses)
  {
    if (segment < 1 || segment > TETHER_LENGTH + 1)
    {
      throw std::out_of_range("Abyss segment is outside the valid range.");
    }
  }
  for (Location debris : scenario.debris)
  {
    validateLocation(debris, robotCount);
  }

  if (!scenario.shipwreck)
  {
    return;
  }

  const Location shipwreck = *scenario.shipwreck;
  validateLocation(shipwreck, robotCount);

  const auto overlapsShipwreck = [shipwreck](Location location)
  {
    return location.robot == shipwreck.robot && location.segment == shipwreck.segment;
  };

  if (std::any_of(scenario.debris.begin(), scenario.debris.end(), overlapsShipwreck) ||
      scenario.abysses[shipwreck.robot] == shipwreck.segment)
  {
    throw std::invalid_argument("Shipwreck overlaps debris or an abyss.");
  }
}
