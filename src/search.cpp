/*
 * STUDENT CODE - EDIT THIS FILE
 */

#include "search.h"

Search::Search(Mothership &mothership)
    : mothership(mothership)
// , robotSelector(mothership, seafloorSurvey) /* EXAMPLE - INITIALIZE A DATA MEMBER */
{
}

std::optional<Location> Search::findShipwreck()
{
  /* BEGIN - TOY APPROACH (CHANGE OR REPLACE ANYTHING) */

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
  /* END - TOY APPROACH*/

  /* BEGIN - ANOTHER TOY APPROACH

  auto robot = robotSelector.choose(); // EXAMPLE - USING DATA MEMBER
  if (robot)
  {
    mothership.switchToRobot(*robot);
    DriveResult result = mothership.driveActiveRobot();
    if (result.reason == DriveStopReason::ShipwreckFound)
    {
      return Location{*robot, mothership.getRobotPosition(*robot)};
    }
  }
  return std::nullopt;

  END - ANOTHER TOY APPROACH */
}
