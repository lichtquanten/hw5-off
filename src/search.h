/*
 * STUDENT CODE - EDIT THIS FILE
 * Keep the class name and required interface. You may change other code.
 */

#pragma once

#include <optional>

#include "mothership.h"
#include "seafloor_survey.h"

// #include "examples/robot_selector.h" // EXAMPLE - INCLUDER A HEADER FILE

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

  // int robotsExplored = 0; // EXAMPLE - ADD A DATA MEMBER
  // RobotSelector robotSelector; // EXAMPLE - ""

  // bool isReachable(int robot) const; // EXAMPLE - ADD A MEMBER FUNCTION
};
