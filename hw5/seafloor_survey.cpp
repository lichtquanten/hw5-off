/*
 * STUDENT CODE - OPTIONAL - EDIT, REPLACE, OR DELETE
 */

#include "seafloor_survey.h"

void SeafloorSurvey::recordDebris(Location location)
{
  /* TOY APPROACH*/
  debrisLocations.push_back(location);
}

int SeafloorSurvey::getSignalStrength(Location location) const
{
  /* TOY APPROACH */
  if (location.robot == 0)
  {
    return 10;
  }
  return 0;
}

/* EXAMPLE
bool SeafloorSurvey::hasDebris() const
{
  return !debrisLocations.empty();
}
*/