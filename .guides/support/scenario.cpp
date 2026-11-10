/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "scenario.h"

#include <algorithm>
#include <stdexcept>
#include <tuple>

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

  void validateLocations(const std::vector<Location> &locations,
                         const char *duplicateMessage)
  {
    for (Location location : locations)
    {
      validateLocation(location);
    }

    std::vector<Location> sorted(locations);
    std::sort(sorted.begin(), sorted.end(), [](Location a, Location b)
    {
      return std::tie(a.robot, a.segment) < std::tie(b.robot, b.segment);
    });

    if (std::adjacent_find(sorted.begin(), sorted.end(), [](Location a, Location b)
    {
      return a.robot == b.robot && a.segment == b.segment;
    }) != sorted.end())
    {
      throw std::invalid_argument(duplicateMessage);
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
  validateLocations(scenario.debris, "Duplicate debris location.");

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
