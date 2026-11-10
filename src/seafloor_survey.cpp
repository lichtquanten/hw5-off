/*
 * OPTIONAL HELPER - EDIT OR REPLACE
 * getSignalStrength() is a placeholder.
 */

#include "seafloor_survey.h"

/* BEGIN STUDENT IMPLEMENTATION */

void SeafloorSurvey::recordDebris(Location location)
{
  debrisLocations.push_back(location);
}

int SeafloorSurvey::getSignalStrength(Location location) const
{
  return 10;
}

/* END STUDENT IMPLEMENTATION */
