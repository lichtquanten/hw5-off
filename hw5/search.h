/*
 * STUDENT CODE - EDIT THIS FILE
 * Keep the class name and required interface.
 * Otherwise, add or change any data members or
 * member functions.
 */

#pragma once

#include "mothership.h"
#include "seafloor_survey.h"

// #include "examples/robot_selector.h" // EXAMPLE

class Search
{
public:
  explicit Search(Mothership &mothership); // REQUIRED

  SearchResult findShipwreck(); // REQUIRED

private:
  Mothership &mothership; // REQUIRED

  SeafloorSurvey seafloorSurvey; // OPTIONAL

  // int robotsExplored = 0; // EXAMPLE
  // RobotSelector robotSelector; // EXAMPLE

  // bool isReachable(int robot) const; // EXAMPLE
};
