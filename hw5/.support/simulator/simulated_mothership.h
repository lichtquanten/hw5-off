#pragma once

#include <optional>
#include <vector>

#include "mothership.h"
#include "simulated_robot.h"

struct Scenario;

class SimulatedMothership final : public Mothership
{
public:
  explicit SimulatedMothership(const Scenario &scenario);

  int getRobotCount() const override;
  const std::vector<int> &getAbysses() const override;
  int getActiveRobot() const override;
  int getRobotPosition(int robotIndex) const override;
  long long getElapsedTicks() const override;

  void switchToRobot(int robotIndex) override;
  DriveResult driveActiveRobot(
      int maxSegments = MAX_SEGMENTS_PER_DRIVE) override;

  std::optional<Location> getDiscoveredShipwreck() const;
  bool allPathsExplored() const;

private:
  std::vector<int> abysses;
  std::vector<SimulatedRobot> robots;
  std::optional<Location> discoveredShipwreck;
  int activeRobotIndex = 0;
  long long elapsedTicks = 0;
};
