/*
 * STUDENT CODE - EDIT THIS FILE
 * Both drivers use these methods. Do not drive or switch robots here.
 * The benchmark calls chooseNextRobot() after every drive.
 */

#include "search.h"

Search::Search(Mothership &mothership) : mothership(mothership) {}

std::optional<int> Search::chooseNextRobot()
{
  /* BEGIN EXAMPLE IMPLEMENTATION -- first robot with an unfinished path */

  for (int robot = 0; robot < ROBOT_COUNT; ++robot)
  {
    if (mothership.getRobotPosition(robot) < mothership.getAbysses()[robot] - 1)
    {
      return robot;
    }
  }
  return std::nullopt;

  /* END EXAMPLE IMPLEMENTATION */
}

void Search::recordDriveResult(int robot, const DriveResult &result)
{
  /* BEGIN EXAMPLE IMPLEMENTATION */

  if (result.reason == DriveStopReason::DebrisFound)
  {
    seafloorSurvey.recordDebris({robot, mothership.getRobotPosition(robot)});
  }

  /* END EXAMPLE IMPLEMENTATION */
}

bool Search::shouldKeepDriving(int, const DriveResult &)
{
  /* BEGIN EXAMPLE IMPLEMENTATION -- stay until this path is finished */

  return true;

  /* END EXAMPLE IMPLEMENTATION */
}

/* BEGIN METHOD EXAMPLE

bool Search::isReachable(int robot) const
{
  return mothership.getRobotPosition(robot) < mothership.getAbysses()[robot] - 1;
}

END METHOD EXAMPLE */
