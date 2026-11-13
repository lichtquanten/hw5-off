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
    if (hasRemainingPath(robot))
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

void Search::refreshWaitingSignal(int robot)
{
  const int signal = measureSignal(robot);
  if (signal == signals[robot])
  {
    return;
  }
  const bool isWaiting = hasRemainingPath(robot);
  if (isWaiting)
  {
    waiting.erase(rank(robot));
  }
  signals[robot] = signal;
  if (isWaiting)
  {
    waiting.insert(rank(robot));
  }
}

SearchResult Search::findShipwreck()
{
  if (waiting.empty())
  {
    return SearchResult{false, Location{}};
  }
  int active = waiting.begin()->robot;
  waiting.erase(waiting.begin());
  mothership.switchToRobot(active);
  int drivenSinceSwitch = 0;
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
      for (int neighbor : {leftOf(active), rightOf(active)})
      {
        if (neighbor != active)
        {
          refreshWaitingSignal(neighbor);
        }
      }
    }
    signals[active] = measureSignal(active);

    const bool hasPath = hasRemainingPath(active);
    const bool stillUseful = hasPath && (signals[active] > 0 || drivenSinceSwitch < SWEEP_LENGTH);
    const bool hasRival = !waiting.empty();
    if (stillUseful && (!hasRival || waiting.begin()->signal - signals[active] < SWITCH_MARGIN))
    {
      continue;
    }
    if (hasPath && (!hasRival || rank(active) < *waiting.begin()))
    {
      continue;
    }
    if (!hasRival)
    {
      return SearchResult{false, Location{}};
    }
    const int next = waiting.begin()->robot;
    waiting.erase(waiting.begin());
    if (hasPath)
    {
      waiting.insert(rank(active));
    }
    mothership.switchToRobot(next);
    active = next;
    drivenSinceSwitch = 0;
  }
}
