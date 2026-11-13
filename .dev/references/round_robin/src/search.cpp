#include "search.h"

#include <algorithm>

SearchResult Search::findShipwreck()
{
  bool workRemaining = true;
  while (workRemaining)
  {
    workRemaining = false;
    for (int robot = 0; robot < mothership.getRobotCount(); ++robot)
    {
      const int end = std::min(TETHER_LENGTH, mothership.getAbysses()[robot] - 1);
      if (mothership.getRobotPosition(robot) >= end)
      {
        continue;
      }
      workRemaining = true;
      mothership.switchToRobot(robot);
      int moved = 0;
      while (mothership.getRobotPosition(robot) < end && moved < SIGNAL_RANGE)
      {
        const int safeDistance = end - mothership.getRobotPosition(robot);
        const DriveResult result = mothership.driveActiveRobot(
            std::min(MAX_SEGMENTS_PER_DRIVE, safeDistance));
        moved += result.segmentsMoved;
        if (result.reason == DriveStopReason::ShipwreckFound)
        {
          return SearchResult{true, Location{robot, mothership.getRobotPosition(robot)}};
        }
      }
    }
  }
  return SearchResult{false, Location{}};
}
