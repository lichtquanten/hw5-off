/*
 * PROVIDED CODE - READ AND USE CONSTANTS, TYPES, CLASS; DO NOT EDIT
 */

#pragma once

#include <array>

constexpr int ROBOT_COUNT = 100;
constexpr int TETHER_LENGTH = 10'000'000;
constexpr int SIGNAL_RANGE = 500;
constexpr int MAX_SEGMENTS_PER_DRIVE = 100;
constexpr int SWITCH_COST = 10;

enum class DriveStopReason
{
  DriveLimitReached,
  DebrisFound,
  ShipwreckFound,
  PathLimitReached
};

struct DriveResult
{
  int segmentsMoved;
  DriveStopReason reason;
};

struct Location
{
  int robot;
  int segment;
};

class Mothership
{
public:
  virtual ~Mothership() = default;

  virtual const std::array<int, ROBOT_COUNT> &getAbysses() const = 0;

  virtual int getActiveRobot() const = 0;
  virtual int getRobotPosition(int robot) const = 0;
  virtual long long getElapsedTicks() const = 0;

  virtual void switchToRobot(int robot) = 0;
  [[nodiscard]] virtual DriveResult driveActiveRobot() = 0;
};
