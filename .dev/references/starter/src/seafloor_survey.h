#pragma once

#include <vector>

#include "mothership.h"

class SeafloorSurvey
{
public:
  void recordDebris(Location location);
  int getSignalStrength(Location location) const;

private:

  std::vector<Location> debrisLocations;
};
