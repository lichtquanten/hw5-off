#include "seafloor_survey.h"

void SeafloorSurvey::recordDebris(Location location)
{
  debrisLocations.push_back(location);
}

int SeafloorSurvey::getSignalStrength(Location location) const
{
  if (location.robot == 0)
  {
    return 10;
  }
  return 0;
}
