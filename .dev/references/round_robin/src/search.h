#pragma once

#include <optional>

#include "mothership.h"

class Search
{
public:
    explicit Search(Mothership& mothership) : mothership(mothership) {}
    std::optional<Location> findShipwreck();

private:
    Mothership& mothership;
};
