/* BEGIN EXAMPLE - ADD YOUR OWN CLASS

#pragma once

#include <optional>
#include <random>

#include "mothership.h"
#include "seafloor_survey.h"

class RobotSelector
{
public:
  RobotSelector(const Mothership &ship, const SeafloorSurvey &survey);
  std::optional<int> choose();

private:
  const Mothership &ship;
  const SeafloorSurvey &survey;
  std::mt19937 generator{42};
};

END EXAMPLE - ADD YOUR OWN CLASS */
