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

  /* Add your own public methods here if needed. */

private:
  /* BEGIN EXAMPLE STORAGE */

  std::vector<Location> debrisLocations;

  /* END EXAMPLE STORAGE */
};
