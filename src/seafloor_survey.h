/*
 * OPTIONAL HELPER - EDIT OR REPLACE
 * You may change this class or use your own classes and files.
 */

#pragma once

#include <vector>

#include "mothership.h"

class SeafloorSurvey
{
public:
  void recordDebris(Location location);
  int getSignalStrength(Location location) const;

  /* Add your own public members as needed. */

private:
  /* BEGIN - TOY STORAGE (CHANGE OR REPLACE ANYTHING) */

  std::vector<Location> debrisLocations;

  /* END - TOY STORAGE */

  /* BEGIN EXAMPLE - ADD YOUR OWN DATA MEMBER

  int discoveriesRecorded = 0;

  END EXAMPLE - ADD YOUR OWN DATA MEMBER */

  /* BEGIN EXAMPLE - ADD YOUR OWN MEMBER FUNCTION

  bool hasDebris() const;

  END EXAMPLE - ADD YOUR OWN MEMBER FUNCTION */
};
