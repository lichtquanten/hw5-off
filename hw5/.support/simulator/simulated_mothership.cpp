/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "simulated_mothership.h"

#include "scenario.h"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace
{
  std::array<SimulatedRobot, ROBOT_COUNT> createRobots(const Scenario &scenario)
  {
    std::array<std::vector<int>, ROBOT_COUNT> debrisByRobot;
    std::array<std::size_t, ROBOT_COUNT> debrisCounts{};

    for (Location debris : scenario.debris)
    {
      ++debrisCounts[debris.robot];
    }

    for (int robotIndex = 0; robotIndex < ROBOT_COUNT; ++robotIndex)
    {
      debrisByRobot[robotIndex].reserve(debrisCounts[robotIndex]);
    }

    for (Location debris : scenario.debris)
    {
      debrisByRobot[debris.robot].push_back(debris.segment);
    }

    std::array<SimulatedRobot, ROBOT_COUNT> robots;
    for (int robotIndex = 0; robotIndex < ROBOT_COUNT; ++robotIndex)
    {
      std::optional<int> shipwreckSegment;
      if (scenario.shipwreck && scenario.shipwreck->robot == robotIndex)
      {
        shipwreckSegment = scenario.shipwreck->segment;
      }
      robots[robotIndex] = SimulatedRobot(
          scenario.abysses[robotIndex] - 1, std::move(debrisByRobot[robotIndex]), shipwreckSegment);
    }

    return robots;
  }
}

SimulatedMothership::SimulatedMothership(const Scenario &scenario)
    : abysses(scenario.abysses)
{
  validateScenario(scenario);
  robots = createRobots(scenario);
}

const std::array<int, ROBOT_COUNT> &SimulatedMothership::getAbysses() const
{
  return abysses;
}

int SimulatedMothership::getActiveRobot() const
{
  return activeRobotIndex;
}

int SimulatedMothership::getRobotPosition(int robotIndex) const
{
  validateRobotIndex(robotIndex);
  return robots[robotIndex].getPosition();
}

long long SimulatedMothership::getElapsedTicks() const
{
  return elapsedTicks;
}

void SimulatedMothership::switchToRobot(int robotIndex)
{
  validateRobotIndex(robotIndex);

  if (robotIndex != activeRobotIndex)
  {
    elapsedTicks += SWITCH_COST;
    activeRobotIndex = robotIndex;
  }
}

DriveResult SimulatedMothership::driveActiveRobot(int maxSegments)
{
  auto &robot = robots[activeRobotIndex];
  const DriveResult result = robot.advance(maxSegments);
  elapsedTicks += result.segmentsMoved;

  if (result.reason == DriveStopReason::ShipwreckFound)
  {
    discoveredShipwreck = Location{activeRobotIndex, robot.getPosition()};
  }

  return result;
}

std::optional<Location> SimulatedMothership::getDiscoveredShipwreck() const
{
  return discoveredShipwreck;
}

bool SimulatedMothership::allPathsExplored() const
{
  return std::all_of(robots.begin(), robots.end(), [](const SimulatedRobot &robot)
  {
    return robot.hasReachedEnd();
  });
}
