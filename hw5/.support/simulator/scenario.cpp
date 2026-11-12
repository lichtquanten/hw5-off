/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "scenario.h"

#include <algorithm>
#include <stdexcept>

void validateRobotIndex(int robotIndex)
{
  if (robotIndex < 0 || robotIndex >= ROBOT_COUNT)
  {
    throw std::out_of_range("Robot index is outside the valid range.");
  }
}

namespace
{
  void validateLocation(Location location)
  {
    validateRobotIndex(location.robot);

    if (location.segment < 1 || location.segment > TETHER_LENGTH)
    {
      throw std::out_of_range("Segment number is outside the valid range.");
    }
  }
}

void validateScenario(const Scenario &scenario)
{
  for (int segment : scenario.abysses)
  {
    if (segment < 1 || segment > TETHER_LENGTH + 1)
    {
      throw std::out_of_range("Abyss segment is outside the valid range.");
    }
  }
  for (Location debris : scenario.debris)
  {
    validateLocation(debris);
  }

  if (!scenario.shipwreck)
  {
    return;
  }

  const Location shipwreck = *scenario.shipwreck;
  validateLocation(shipwreck);

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
