/*
 * OPTIONAL HELPER - EDIT OR REPLACE
 * getSignalStrength() is a placeholder.
 */

#include "seafloor_survey.h"

void SeafloorSurvey::recordDebris(Location location)
{
  /* BEGIN - TOY APPROACH (CHANGE OR REPLACE ANYTHING) */

  debrisLocations.push_back(location);

  /* END - TOY APPROACH */
}

int SeafloorSurvey::getSignalStrength(Location location) const
{
  /* BEGIN - TOY APPROACH (CHANGE OR REPLACE ANYTHING) */

  if (location.robot == 0)
  {
    return 10;
  }
  return 0;

  /* END - TOY APPROACH */
}

/* BEGIN EXAMPLE - IMPLEMENT YOUR OWN MEMBER FUNCTION

bool SeafloorSurvey::hasDebris() const
{
  return !debrisLocations.empty();
}

END EXAMPLE - IMPLEMENT YOUR OWN MEMBER FUNCTION */
