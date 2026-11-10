/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "simulated_mothership.h"

#include "scenario.h"

#include <algorithm>
#include <utility>

namespace
{
  std::array<SimulatedRobot, ROBOT_COUNT> createRobots(const Scenario &scenario)
  {
    struct RobotSetup
    {
      int reachableEnd = TETHER_LENGTH;
      std::vector<int> debrisSegments;
      std::optional<int> shipwreckSegment;
    };

    std::array<RobotSetup, ROBOT_COUNT> setups;

    for (int robotIndex = 0; robotIndex < ROBOT_COUNT; ++robotIndex)
    {
      setups[robotIndex].reachableEnd = scenario.abysses[robotIndex] - 1;
    }

    for (Location debris : scenario.debris)
    {
      setups[debris.robot].debrisSegments.push_back(debris.segment);
    }

    if (scenario.shipwreck)
    {
      setups[scenario.shipwreck->robot].shipwreckSegment =
          scenario.shipwreck->segment;
    }

    std::array<SimulatedRobot, ROBOT_COUNT> robots;
    for (int robotIndex = 0; robotIndex < ROBOT_COUNT; ++robotIndex)
    {
      auto &setup = setups[robotIndex];
      robots[robotIndex] = SimulatedRobot(
          setup.reachableEnd, std::move(setup.debrisSegments), setup.shipwreckSegment);
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

DriveResult SimulatedMothership::driveActiveRobot()
{
  auto &robot = robots[activeRobotIndex];
  const DriveResult result = robot.advance();
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
