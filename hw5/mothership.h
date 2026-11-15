/*
 * PROVIDED CODE
      - READ CLOSELY
      - USE CONSTANTS, TYPES, CLASS
      - DO NOT EDIT
 */

#pragma once

#include <cstddef>
#include <functional>
#include <vector>

constexpr int TETHER_LENGTH = 10'000'000;
constexpr int SIGNAL_RANGE = 500;
constexpr int MAX_SEGMENTS_PER_DRIVE = 100;
constexpr int SWITCH_COST = 200;

enum class DriveStopReason
{
  DriveLimitReached,
  DebrisFound,
  ShipwreckFound
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

  bool operator<(const Location &other) const
  {
    if (robot != other.robot)
    {
      return robot < other.robot;
    }
    return segment < other.segment;
  }

  bool operator==(const Location &other) const
  {
    return robot == other.robot && segment == other.segment;
  }
};

struct LocationHash
{
  std::size_t operator()(const Location &location) const
  {
    return std::hash<long long>()(
        static_cast<long long>(location.robot) * (TETHER_LENGTH + 1) + location.segment);
  }
};

struct SearchResult
{
  bool isFound;
  Location location;
};

class Mothership
{
public:
  virtual ~Mothership() = default;

  virtual int getRobotCount() const = 0;
  virtual const std::vector<Location> &getAbysses() const = 0;

  virtual int getActiveRobot() const = 0;
  virtual int getRobotPosition(int robot) const = 0;
  virtual long long getElapsedTicks() const = 0;

  virtual void switchToRobot(int robot) = 0;

  virtual DriveResult driveActiveRobot(
      int maxSegments = MAX_SEGMENTS_PER_DRIVE) = 0;
};
