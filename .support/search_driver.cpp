/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "search_driver.h"

#include "search.h"

#include <stdexcept>

namespace
{
  using Clock = std::chrono::steady_clock;

  bool canAdvance(const Mothership &mothership, int robot)
  {
    if (robot < 0 || robot >= ROBOT_COUNT)
    {
      throw std::out_of_range("Selected robot index is outside the valid range.");
    }
    return mothership.getRobotPosition(robot) < mothership.getAbysses()[robot] - 1;
  }

  void requireCompletion(const Mothership &mothership)
  {
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
      if (canAdvance(mothership, robot))
      {
        throw std::runtime_error(
            "chooseNextRobot() returned nullopt before every path was finished.");
      }
    }
  }
}

SearchRun runSearch(Search &search, Mothership &mothership,
                    DecisionCadence cadence, long long driveLimit)
{
  SearchRun run;

  while (true)
  {
    const auto selectionStart = Clock::now();
    const auto robot = search.chooseNextRobot();
    run.selectionTime += Clock::now() - selectionStart;
    ++run.selections;

    if (!robot)
    {
      requireCompletion(mothership);
      return run;
    }
    if (!canAdvance(mothership, *robot))
    {
      throw std::runtime_error("chooseNextRobot() selected a finished robot.");
    }

    mothership.switchToRobot(*robot);

    do
    {
      if (run.drives >= driveLimit)
      {
        throw std::runtime_error("Search exceeded the drive limit.");
      }
      const DriveResult result = mothership.driveActiveRobot();
      ++run.drives;

      const auto recordingStart = Clock::now();
      search.recordDriveResult(*robot, result);
      run.recordingTime += Clock::now() - recordingStart;

      if (result.reason == DriveStopReason::ShipwreckFound)
      {
        run.shipwreck = Location{*robot, mothership.getRobotPosition(*robot)};
        return run;
      }
      if (cadence == DecisionCadence::EveryDrive || !canAdvance(mothership, *robot))
      {
        break;
      }

      const auto continuationStart = Clock::now();
      const bool keepDriving = search.shouldKeepDriving(*robot, result);
      run.continuationTime += Clock::now() - continuationStart;
      ++run.continuationChecks;
      if (!keepDriving)
      {
        break;
      }
    } while (true);
  }
}

std::optional<Location> Search::findShipwreck()
{
  return runSearch(*this, mothership, DecisionCadence::Student).shipwreck;
}
