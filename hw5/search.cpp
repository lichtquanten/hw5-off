/*
 * STUDENT CODE - EDIT THIS FILE
 */

#include "search.h"

#include <algorithm>

Search::Search(Mothership &mothership)
    : mothership(mothership)
// , robotSelector(mothership, seafloorSurvey) /* EXAMPLE
{
}

SearchResult Search::findShipwreck()
{
  /* TOY APPROACH - Depth First Search - (CHANGE OR REPLACE) */
  const std::vector<Location> &abysses = mothership.getAbysses();
  for (int robot = 0; robot < mothership.getRobotCount(); ++robot)
  {
    bool hasAbyss = false;
    int abyssSegment = 0;
    for (const Location &abyss : abysses)
    {
      if (abyss.robot == robot)
      {
        hasAbyss = true;
        abyssSegment = abyss.segment;
        break;
      }
    }

    int lastSegment = TETHER_LENGTH;
    if (hasAbyss)
    {
      lastSegment = std::min(TETHER_LENGTH, abyssSegment - 1);
    }

    if (mothership.getRobotPosition(robot) >= lastSegment)
    {
      continue;
    }
    mothership.switchToRobot(robot);

    while (mothership.getRobotPosition(robot) < lastSegment)
    {
      const int safeDistance = lastSegment - mothership.getRobotPosition(robot);
      DriveResult result = mothership.driveActiveRobot(
          std::min(MAX_SEGMENTS_PER_DRIVE, safeDistance));

      if (result.reason == DriveStopReason::ShipwreckFound)
      {
        return SearchResult{true, Location{robot, mothership.getRobotPosition(robot)}};
      }

      if (result.reason == DriveStopReason::DebrisFound)
      {
        seafloorSurvey.recordDebris({robot, mothership.getRobotPosition(robot)});
      }
    }
  }

  return SearchResult{false, Location{}};
}
