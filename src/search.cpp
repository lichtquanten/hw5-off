/*
 * STUDENT CODE - EDIT THIS FILE
 */

#include "search.h"

Search::Search(Mothership &mothership)
    : mothership(mothership)
// , robotSelector(mothership, seafloorSurvey) /* EXAMPLE
{
}

std::optional<Location> Search::findShipwreck()
{
  /* TOY APPROACH - Depth First Search - (CHANGE OR REPLACE) */
  for (int robot = 0; robot < ROBOT_COUNT; ++robot)
  {
    const int lastSegment = mothership.getAbysses()[robot] - 1;
    if (mothership.getRobotPosition(robot) >= lastSegment)
    {
      continue;
    }
    mothership.switchToRobot(robot);

    while (mothership.getRobotPosition(robot) < lastSegment)
    {
      DriveResult result = mothership.driveActiveRobot();

      if (result.reason == DriveStopReason::ShipwreckFound)
      {
        return Location{robot, mothership.getRobotPosition(robot)};
      }

      if (result.reason == DriveStopReason::DebrisFound)
      {
        seafloorSurvey.recordDebris({robot, mothership.getRobotPosition(robot)});
      }
    }
  }

  return std::nullopt;
}
