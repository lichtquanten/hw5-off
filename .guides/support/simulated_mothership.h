/*
 * PROVIDED CODE - DO NOT EDIT
 */

#pragma once

#include <array>
#include <optional>
#include <vector>

#include "mothership.h"
#include "simulated_robot.h"

struct Scenario;

class SimulatedMothership final : public Mothership
{
public:
  explicit SimulatedMothership(const Scenario &scenario);

  const std::array<int, ROBOT_COUNT> &getAbysses() const override;
  int getActiveRobot() const override;
  int getRobotPosition(int robotIndex) const override;
  long long getElapsedTicks() const override;

  void switchToRobot(int robotIndex) override;
  DriveResult driveActiveRobot() override;

  std::optional<Location> getDiscoveredShipwreck() const;
  bool allPathsExplored() const;

private:
  std::array<int, ROBOT_COUNT> abysses;
  std::array<SimulatedRobot, ROBOT_COUNT> robots;
  std::optional<Location> discoveredShipwreck;
  int activeRobotIndex = 0;
  long long elapsedTicks = 0;
};
