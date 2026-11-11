/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "simulated_mothership.h"
#include "scenario.h"
#include "search.h"

#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>

namespace
{

  Scenario makeDemoScenario()
  {
    Scenario scenario;

    scenario.abysses.fill(1'001);

    scenario.debris = {
        {0, 80},
        {0, 220},
        {1, 260},
        {ROBOT_COUNT - 1, 280},
    };
    scenario.shipwreck = Location{0, 300};

    return scenario;
  }

  void verifySearchResult(const SimulatedMothership &mothership,
                          std::optional<Location> result)
  {
    const auto discovered = mothership.getDiscoveredShipwreck();

    if (result)
    {
      if (!discovered || result->robot != discovered->robot ||
          result->segment != discovered->segment)
      {
        throw std::runtime_error(
            "The returned location was not a discovered shipwreck.");
      }
      return;
    }

    if (discovered)
    {
      throw std::runtime_error(
          "The search found the wreck but returned nullopt.");
    }

    if (!mothership.allPathsExplored())
    {
      throw std::runtime_error(
          "The search stopped before exploring every reachable path.");
    }
  }

}

int main()
{
  try
  {
    const Scenario scenario = makeDemoScenario();
    SimulatedMothership mothership(scenario);

    Search search(mothership);
    const auto result = search.findShipwreck();
    verifySearchResult(mothership, result);

    if (result)
    {
      std::cout << "PASS: found shipwreck at robot " << result->robot
                << ", segment " << result->segment << '\n';
    }
    else
    {
      std::cout << "PASS: no reachable shipwreck.\n";
    }

    return 0;
  }
  catch (const std::exception &error)
  {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
