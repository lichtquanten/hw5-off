/*
 * STUDENT CODE - OPTIONAL - EDIT, REPLACE, OR DELETE
 */

#pragma once

#include <vector>

#include "mothership.h"

class SeafloorSurvey
{
public:
  void recordDebris(Location location);
  int getSignalStrength(Location location) const;

  // bool hasDebris() const; // EXAMPLE

private:
  std::vector<Location> debrisLocations; // TOY APPROACH

  // int discoveriesRecorded = 0; // EXAMPLE
};
