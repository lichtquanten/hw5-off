/*
 * PROVIDED CODE
      - READ CLOSELY
      - USE CONSTANTS, TYPES, CLASS
      - DO NOT EDIT
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
  // Request 1–100 segments. Debris or the wreck stops a drive early;
  // reaching an abyss or traveling beyond the tether causes failure.
  [[nodiscard]] virtual DriveResult driveActiveRobot(
      int maxSegments = MAX_SEGMENTS_PER_DRIVE) = 0;
};
