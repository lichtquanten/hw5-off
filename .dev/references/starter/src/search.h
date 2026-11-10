#pragma once

#include <optional>

#include "mothership.h"
#include "seafloor_survey.h"

class Search
{
public:

  explicit Search(Mothership &mothership);

  std::optional<Location> findShipwreck();

private:
  Mothership &mothership;

  SeafloorSurvey seafloorSurvey;
};
