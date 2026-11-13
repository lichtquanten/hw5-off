#include "simulated_mothership.h"

#include "scenario.h"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace
{
  std::vector<SimulatedRobot> createRobots(const Scenario &scenario)
  {
    const int robotCount = static_cast<int>(scenario.abysses.size());
    std::vector<std::vector<int>> debrisByRobot(robotCount);
    std::vector<std::size_t> debrisCounts(robotCount);

    for (Location debris : scenario.debris)
    {
      ++debrisCounts[debris.robot];
    }

    for (int robotIndex = 0; robotIndex < robotCount; ++robotIndex)
    {
      debrisByRobot[robotIndex].reserve(debrisCounts[robotIndex]);
    }

    for (Location debris : scenario.debris)
    {
      debrisByRobot[debris.robot].push_back(debris.segment);
    }

    std::vector<SimulatedRobot> robots;
    robots.reserve(robotCount);
    for (int robotIndex = 0; robotIndex < robotCount; ++robotIndex)
    {
      std::optional<int> shipwreckSegment;
      if (scenario.shipwreck && scenario.shipwreck->robot == robotIndex)
      {
        shipwreckSegment = scenario.shipwreck->segment;
      }
      robots.emplace_back(scenario.abysses[robotIndex] - 1, std::move(debrisByRobot[robotIndex]), shipwreckSegment);
    }

    return robots;
  }
}

SimulatedMothership::SimulatedMothership(const Scenario &scenario)
    : abysses(scenario.abysses)
{
  robots = createRobots(scenario);
}

int SimulatedMothership::getRobotCount() const
{
  return static_cast<int>(robots.size());
}

const std::vector<int> &SimulatedMothership::getAbysses() const
{
  return abysses;
}

int SimulatedMothership::getActiveRobot() const
{
  return activeRobotIndex;
}

int SimulatedMothership::getRobotPosition(int robotIndex) const
{
  validateRobotIndex(robotIndex, getRobotCount());
  return robots[robotIndex].getPosition();
}

long long SimulatedMothership::getElapsedTicks() const
{
  return elapsedTicks;
}

void SimulatedMothership::switchToRobot(int robotIndex)
{
  validateRobotIndex(robotIndex, getRobotCount());

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
