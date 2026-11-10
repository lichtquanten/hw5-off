/*
 * STUDENT CODE - EDIT THIS FILE
 * Keep the class name, required interface, and mothership reference.
 * The provided search loop uses all three methods below. The benchmark
 * calls recordDriveResult() and chooseNextRobot() after each drive.
 */

#pragma once

#include <optional>

#include "mothership.h"
#include "seafloor_survey.h"

class Search
{
public:
  /* BEGIN REQUIRED INTERFACE */

  explicit Search(Mothership &mothership);

  // Implemented in .support/. Do not define this method yourself.
  std::optional<Location> findShipwreck();

  // Return a robot that can advance; nullopt only when all paths are finished.
  // Must work without calling findShipwreck() first.
  std::optional<int> chooseNextRobot();

  // Called after every drive, including discoveries and path limits.
  void recordDriveResult(int robot, const DriveResult &result);

  // Called after recording a result, if this robot can still advance.
  // Return false to select again. Ignored by the benchmark.
  bool shouldKeepDriving(int robot, const DriveResult &result);

  /* END REQUIRED INTERFACE */

  /* Add your own public methods here if needed. */

private:
  /* BEGIN REQUIRED MEMBER */

  Mothership &mothership;

  /* END REQUIRED MEMBER */

  /* BEGIN EXAMPLE MEMBERS */

  SeafloorSurvey seafloorSurvey;

  /* END EXAMPLE MEMBERS */

  /*
  bool isReachable(int robot) const;
  int robotsExplored = 0;
  */
};
