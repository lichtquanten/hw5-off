/*
 * STUDENT CODE - EDIT THIS FILE
 * Keep the class name and required interface. You may change other code.
 */

#pragma once

#include <optional>

#include "mothership.h"
#include "seafloor_survey.h"

/* BEGIN EXAMPLE - INCLUDE YOUR OWN HEADER FILE
#include "robot_selector.h"
END EXAMPLE */

class Search
{
public:
  /* BEGIN - REQUIRED INTERFACE */

  explicit Search(Mothership &mothership);

  std::optional<Location> findShipwreck();

  /* END - REQUIRED INTERFACE */

  /* Add your own public members as needed. */

private:
  Mothership &mothership;

  SeafloorSurvey seafloorSurvey;

  /* BEGIN EXAMPLE - ADD YOUR OWN DATA MEMBER

  int robotsExplored = 0;

  END EXAMPLE - ADD YOUR OWN DATA MEMBER */

  /* BEGIN EXAMPLE - ADD YOUR OWN MEMBER FUNCTION

  bool isReachable(int robot) const;
  RobotSelector robotSelector;

  END EXAMPLE - ADD YOUR OWN MEMBER FUNCTION */
};
