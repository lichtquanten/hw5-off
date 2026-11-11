/*
 * OPTIONAL HELPER - EDIT OR REPLACE
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
  /* BEGIN - TOY STORAGE (CHANGE OR REPLACE) */

  std::vector<Location> debrisLocations;

  /* END - TOY STORAGE */

  // int discoveriesRecorded = 0; // EXAMPLE - DATA MEMBER

  // bool hasDebris() const; // EXAMPLE - MEMBER FUNCTION
};
