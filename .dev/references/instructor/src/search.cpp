#include "search.h"

#include <algorithm>

namespace
{
  constexpr int SWITCH_MARGIN = 2;
  constexpr int SWEEP_LENGTH = 5'000;
}

bool Search::Rank::operator<(const Rank &other) const
{
  if (signal != other.signal)
  {
    return signal > other.signal;
  }
  if (position != other.position)
  {
    return position < other.position;
  }
  return robot < other.robot;
}

Search::Search(Mothership &mothership)
    : mothership(mothership), robotCount(mothership.getRobotCount()), ends(robotCount),
      positions(robotCount), signals(robotCount), debris(robotCount)
{
  for (int robot = 0; robot < robotCount; ++robot)
  {
    ends[robot] = std::min(TETHER_LENGTH, mothership.getAbysses()[robot] - 1);
    positions[robot] = mothership.getRobotPosition(robot);
    if (isWaiting(robot))
    {
      waiting.insert(rank(robot));
    }
  }
}

int Search::leftOf(int robot) const
{
  return (robot + robotCount - 1) % robotCount;
}

int Search::rightOf(int robot) const
{
  return (robot + 1) % robotCount;
}

bool Search::hasRemainingPath(int robot) const
{
  return positions[robot] < ends[robot];
}

Search::Rank Search::rank(int robot) const
{
  return {signals[robot], positions[robot], robot};
}

int Search::measureSignal(int robot) const
{
  int count = 0;
  for (int path : {leftOf(robot), robot, rightOf(robot)})
  {
    const auto &found = debris[path];
    count += std::upper_bound(found.begin(), found.end(), positions[robot] + SIGNAL_RANGE) -
             std::lower_bound(found.begin(), found.end(), positions[robot] - SIGNAL_RANGE);
  }
  return count;
}

bool Search::isWaiting(int robot) const
{
  return robot != active && hasRemainingPath(robot);
}

void Search::refreshSignal(int robot)
{
  const int signal = measureSignal(robot);
  if (signal == signals[robot])
  {
    return;
  }
  const bool waitingRobot = isWaiting(robot);
  if (waitingRobot)
  {
    waiting.erase(rank(robot));
  }
  signals[robot] = signal;
  if (waitingRobot)
  {
    waiting.insert(rank(robot));
  }
}

bool Search::shouldKeepDriving() const
{
  if (!hasRemainingPath(active))
  {
    return false;
  }
  if (waiting.empty())
  {
    return true;
  }
  const Rank &rival = *waiting.begin();
  const bool stillUseful = signals[active] > 0 || drivenSinceSwitch < SWEEP_LENGTH;
  return (stillUseful && rival.signal - signals[active] < SWITCH_MARGIN) || rank(active) < rival;
}

void Search::switchTo(int robot)
{
  const int previous = active;
  waiting.erase(rank(robot));
  active = robot;
  if (previous >= 0 && isWaiting(previous))
  {
    waiting.insert(rank(previous));
  }
  mothership.switchToRobot(robot);
  drivenSinceSwitch = 0;
}

SearchResult Search::findShipwreck()
{
  if (waiting.empty())
  {
    return SearchResult{false, Location{}};
  }
  switchTo(waiting.begin()->robot);
  while (true)
  {
    const DriveResult result = mothership.driveActiveRobot(
        std::min(MAX_SEGMENTS_PER_DRIVE, ends[active] - positions[active]));
    positions[active] += result.segmentsMoved;
    drivenSinceSwitch += result.segmentsMoved;
    if (result.reason == DriveStopReason::ShipwreckFound)
    {
      return SearchResult{true, Location{active, positions[active]}};
    }
    if (result.reason == DriveStopReason::DebrisFound)
    {
      debris[active].push_back(positions[active]);
      refreshSignal(leftOf(active));
      refreshSignal(rightOf(active));
    }
    refreshSignal(active);
    if (shouldKeepDriving())
    {
      continue;
    }
    if (waiting.empty())
    {
      return SearchResult{false, Location{}};
    }
    switchTo(waiting.begin()->robot);
  }
}
