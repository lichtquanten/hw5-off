#pragma once

#include "mothership.h"

class Search
{
public:
  explicit Search(Mothership &mothership)
      : mothership(mothership) {}
  SearchResult findShipwreck();

private:
  Mothership &mothership;
};
