#include <gtest/gtest.h>

#include "scenario.h"
#include "simulated_mothership.h"
#include "simulated_robot.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

TEST(Simulator, AbyssLocationsKeepTheirRobotIdentityInShuffledOrder)
{
  Scenario scenario;
  scenario.abysses = {75, 1, 202, 501, 1001, TETHER_LENGTH + 500};
  SimulatedMothership mothership(scenario);
  const auto &abysses = mothership.getAbysses();
  ASSERT_EQ(abysses.size(), scenario.abysses.size());
  EXPECT_FALSE(std::is_sorted(abysses.begin(), abysses.end(),
                              [](Location a, Location b) { return a.robot < b.robot; }));

  std::vector<bool> seen(scenario.abysses.size());
  for (Location abyss : abysses)
  {
    ASSERT_GE(abyss.robot, 0);
    ASSERT_LT(abyss.robot, mothership.getRobotCount());
    EXPECT_FALSE(seen[abyss.robot]);
    seen[abyss.robot] = true;
    EXPECT_EQ(abyss.segment, scenario.abysses[abyss.robot]);
  }
  EXPECT_EQ(&mothership.getAbysses(), &abysses);
}

TEST(Simulator, LastSafeSegmentHasNoSpecialNotification)
{
  SimulatedRobot robot(74, {}, std::nullopt);
  const DriveResult result = robot.advance(74);
  EXPECT_EQ(result.segmentsMoved, 74);
  EXPECT_EQ(result.reason, DriveStopReason::DriveLimitReached);
  EXPECT_EQ(robot.getPosition(), 74);
  EXPECT_THROW(robot.advance(1), std::runtime_error);
}

TEST(Simulator, UnsafeDriveThrowsInsteadOfStoppingBeforeAbyss)
{
  Scenario scenario;
  scenario.abysses = {75};
  SimulatedMothership mothership(scenario);
  EXPECT_THROW(mothership.driveActiveRobot(75), std::runtime_error);
  EXPECT_EQ(mothership.getRobotPosition(0), 0);
  EXPECT_EQ(mothership.getElapsedTicks(), 0);
}

TEST(Simulator, DiscoveriesStillStopAnOtherwiseUnsafeDrive)
{
  SimulatedRobot robot(74, {30}, 74);
  const DriveResult debris = robot.advance(100);
  EXPECT_EQ(debris.segmentsMoved, 30);
  EXPECT_EQ(debris.reason, DriveStopReason::DebrisFound);
  const DriveResult wreck = robot.advance(100);
  EXPECT_EQ(wreck.segmentsMoved, 44);
  EXPECT_EQ(wreck.reason, DriveStopReason::ShipwreckFound);
}

TEST(Simulator, DebrisAtLastSafeSegmentKeepsDiscoveryReason)
{
  SimulatedRobot robot(74, {74}, std::nullopt);
  const DriveResult result = robot.advance(74);
  EXPECT_EQ(result.segmentsMoved, 74);
  EXPECT_EQ(result.reason, DriveStopReason::DebrisFound);
  EXPECT_THROW(robot.advance(1), std::runtime_error);
}

TEST(Simulator, TetherAppliesEvenWhenAbyssIsFartherAway)
{
  Scenario scenario;
  scenario.abysses = {TETHER_LENGTH + 500};
  SimulatedMothership mothership(scenario);
  while (mothership.getRobotPosition(0) < TETHER_LENGTH)
  {
    const DriveResult result = mothership.driveActiveRobot();
    ASSERT_EQ(result.segmentsMoved, MAX_SEGMENTS_PER_DRIVE);
    ASSERT_EQ(result.reason, DriveStopReason::DriveLimitReached);
  }
  EXPECT_EQ(mothership.getRobotPosition(0), TETHER_LENGTH);
  EXPECT_EQ(mothership.getElapsedTicks(), TETHER_LENGTH);
  EXPECT_TRUE(mothership.allPathsExplored());
  EXPECT_THROW(mothership.driveActiveRobot(1), std::runtime_error);
}
